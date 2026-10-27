# AuraDll × UE 边界实施计划

- 日期：2026-10-12
- 依据设计：[2026-10-12-auradll-ue-boundary-design.md](../specs/2026-10-12-auradll-ue-boundary-design.md)
- 范围：只覆盖设计文档定义的边界与机制。DLL 的具体业务接口按同一契约增量添加，不在本计划内。

## 执行顺序与理由

1. **先 DLL 侧，后 UE 侧**。UE 侧要链的 `Aura_*` 符号必须先存在，否则一直在错符号上调试。
2. **先把编译配置统一，再写任何代码**。否则后面每一步都在错配置（Debug `/MDd`）上验证，会得出错误结论。
3. **回调贯通排在纯函数之后**。先确认最简链路（同步调用）通了，再引入线程与队列，出问题时范围可控。

---

## 阶段 0：DLL 侧收敛为可发布边界

### 0.1 统一编译配置

- 改 `Aura.sln` 中 `InterfaceAuraDll` 的 `Development Editor | Win64` 映射：`Debug | x64` 改为 `Release | x64`（当前在第 1736-1755 行）。
- 后构建事件改为只同步三项：`AuraDllApi.h`、`InterfaceAuraDll.dll`、`InterfaceAuraDll.lib`。
- 清理：删除仓库根 `Debug/InterfaceAuraDll.dll`（32 位残留）；把 `InterfaceAuraDll/Debug/`、`InterfaceAuraDll/x64/`、`InterfaceAuraDll/InterfaceAuraDll/` 写入 `.gitignore`。

**验收**：重新构建后 `Source/Aura/ThirdParty/InterfaceAuraDll.dll` 不含 `@ILT+` 增量链接跳转，且 `AuraDllApi.h` 与 DLL 来自同一批产出。

### 0.2 新增对外头 `AuraDllApi.h`

内容：导出宏（沿用 `INTERFACE_AURA_API`）、POD 定义（`AuraInitDesc` 等）、回调 typedef `AuraEventCallback`、全部 `extern "C"` 声明。形状见设计第 4.3 节。

**验收**：加入 `InterfaceAuraDll.vcxproj` 的 `ClInclude`；Debug 与 Release、Win32 与 x64 四个配置都能编译。

### 0.3 新增包装层 `AuraDllApi.cpp`

实现 `Aura_Initialize` / `Aura_Shutdown` / `Aura_IsInitialized` / `Aura_SetEventCallback` / `Aura_GetApiVersion`，内部通过 `Singleton<AuraMainEntrance>::Instance()` 取得单例。本单元需展开一次 `OV_DECLARE_DLL_SINGLETON(AuraMainEntrance)`（否则模板走隐式实例化，链接不到 `Singleton.cpp` 中的定义）。

**验收**：Release | x64 构建通过；`dumpbin /exports` 中全部 `Aura_*` 为未修饰 C 名。

### 0.4 收回 C++ 导出面

去掉 `AuraMainEntrance` 的 `INTERFACE_AURA_API`、以及 `Singleton.h` 两个宏中的 `INTERFACE_AURA_API`，使二者成为 DLL 内部符号。保留 `OV_DECLARE_DLL_SINGLETON` / `OV_DEFINE_DLL_SINGLETON` 机制本身（防误用的价值与是否导出无关）。

**验收**：导出表中不再出现 `?Instance@`、`?Initialize@`、`??4AuraMainEntrance`；UE 侧无法误链 C++ 符号。

---

## 阶段 1：UE 侧接线（最简链路）

### 1.1 确认构建接线与打包 staging

`Source/Aura/Aura.Build.cs` 现有三行保留。验证打包时 DLL 是否被 stage；不通过则把 `RuntimeDependencies` 改为 `$(BinaryOutputDir)/InterfaceAuraDll.dll` 形式。

**验收**：编辑器启动后 `Binaries/Win64/InterfaceAuraDll.dll` 存在。

### 1.2 无状态纯函数包装

新增 `Source/Aura/Public/AuraDll/AuraDllFunctions.h` 与对应 `.cpp`，纯 C++ 静态类，内部只调 `Aura_*`。蓝图需要时再加 `UAuraDllLibrary` 薄壳。

**验收**：编辑器（或一个临时测试调用点）调用 `Aura_Add(2, 3)` 返回 5；`Aura_GetVersion` 返回值与 DLL 内一致。

### 1.3 `UAuraDllSubsystem`

新增 `Source/Aura/{Public,Private}/AuraDll/AuraDllSubsystem.h/.cpp`：`UGameInstanceSubsystem`，持有 Initialize / Deinitialize、加锁事件队列、`FTSTicker` 句柄。

**验收**：启动日志出现初始化成功；PIE 进出一次不崩、不重复初始化。

---

## 阶段 2：回调贯通

### 2.1 DLL 侧可验证的异步事件源（临时）

加一个后台线程，每 1 秒投递一个测试事件。**仅用于验证链路**，验证通过后由真实业务事件替换。

**验收**：事件在释放时不与关闭流程竞争（`Aura_Shutdown` 返回后线程已停）。

### 2.2 UE 侧 trampoline 与分发

`extern "C"` trampoline 只做入队；`FAuraDllEvent` 定义为 `USTRUCT`；`UAuraDllSubsystem` 用动态多播委托在游戏线程广播。需要即时处理的调用点在包装函数末尾显式调 `DrainEvents()`。

**验收**：编辑器 Output Log 每秒收到一次事件；事件顺序与投递顺序一致；游戏线程断言不触发。

### 2.3 关闭顺序

按设计第 5.3 节的五步实现。

**验收**：连续 PIE 进出 10 次无崩溃、无晚到事件穿透、无重复注册。

---

## 阶段 3：复杂数据

### 3.1 转换层 `AuraDllConvert.h`

POD 与 `FString` / `TArray` 互转集中于此；若有 DLL 分配的内存，实现 `Aura_FreeBuffer`。

**验收**：UTF-8 中文字符串往返一致；二进制块长度与内容逐字节一致。

### 3.2 用一条真实接口贯通

选一条实际的复杂数据接口（字符串往返 + 二进制块往返）作为首个真实用例。

**验收**：同一份数据在 UE 侧进出后等值。

---

## 阶段 4：打包与版本校验

### 4.1 打包验证

打包 Win64，确认包内 `Binaries/Win64/InterfaceAuraDll.dll` 存在且运行时可初始化、可收发事件。

### 4.2 版本校验验证

故意用旧 DLL 配新头，确认 `Aura_GetApiVersion()` 不匹配时初始化被拒绝并记录 Error 日志。

**验收**：拒绝初始化且 UE 侧不崩溃。

---

## 回滚与风险

- 阶段 0.4 会移除 C++ 符号导出。若日后需要 UE 直接调单例（设计附录的方案 B），只需把那两个宏与类声明上的 `INTERFACE_AURA_API` 加回，并把 `Singleton.h`、`AuraMainEntrance.h` 纳入分发。
- 阶段 0.1 之前不要写业务代码：在 Debug 配置上验证会得到误导性结论。
- 若 DLL 初始化开销大，按设计第 4.2 节的切换条件把宿主换成 `UEngineSubsystem`，边界本身不变。

---

## 执行记录（2026-10-14）

### 已完成并有证据

- **阶段 0 全部完成**：新增 `AuraDllApi.h` / `AuraDllApi.cpp`；`Singleton.h`、`AuraMainEntrance.h` 的 C++ 导出收回；导出面收敛为 **7 个未修饰的 `Aura_*` C 名**（`dumpbin /exports` 实测，C++ 符号残留 0）；`Source/Aura/ThirdParty/` 恰好三个文件；`.gitignore` 补了 DLL 工程产物忽略与 ThirdParty 白名单；仓库根 `Debug/` 残留清除。
- **阶段 1/2 的 UE 侧管道已实现并编译通过**：`AuraDllFunctions.h/.cpp`、`AuraDllSubsystem.h/.cpp`；`UnrealEditor-Aura.dll` 链接成功，我的文件零 warning。
- **sln 补丁脚本已验证**：模拟 UBT 整份重写后修复结果与原始**逐字节一致**，且重复运行幂等。

### 踩到的两个坑（都已实测修复）

**坑 1：模块级 `RuntimeDependencies` 用 `$(TargetOutputDir)` 不会真的拷贝。**

必须写 `$(BinaryOutputDir)`，否则 `InterfaceAuraDll.dll` 不会出现在 `Binaries/Win64`，编辑器加载 Aura 模块时会直接找不到该 DLL。

```csharp
RuntimeDependencies.Add("$(BinaryOutputDir)/InterfaceAuraDll.dll", RuntimeDllPath);
```

**坑 2：工作区里的非 ASCII 路径会让 UBT 崩溃（UE 5.6 的 bug）。**

现象：`Build.bat` / VS 构建以退出码 `-532462766`（`0xE0434352`）**静默死亡**，无异常文本，UBT 日志停在哪一行每次都不一样。

根因（`Engine/Source/Programs/UnrealBuildTool/System/SourceFileWorkingSet.cs`）：UBT 用
`git --no-optional-locks status --porcelain` 获取 working set，`OutputDataReceived` 把每行第 3 个字符之后的**整段字符串**交给 `AddPath`，而 `AddPath` 直接调用 `FileReference.Combine`。git 默认 `core.quotepath=true`，会把含非 ASCII 字符的路径输出成**带引号并八进制转义**的形式，于是：

```
Unhandled exception. System.ArgumentException: Path fragment
'"Docs/\347\261\273\345\271\270\345\255\230\350\200\205-\345\215\225\344\272\272\345\274\200\345\217\221\350\267\257\347\272\277.md"'
contains invalid directory separators.
   at EpicGames.Core.FileSystemReference.CombineStrings(...)
   at UnrealBuildTool.GitSourceFileWorkingSet.AddPath(String Path)
   at UnrealBuildTool.GitSourceFileWorkingSet.OutputDataReceived(...)
   at System.Threading.ThreadPoolWorkQueue.Dispatch()
```

异常发生在**线程池后台线程**上，所以既不打印、也没有 GPF 日志，整个进程被直接终止。

触发条件（两点同时成立）：

1. 工作区存在**未被跟踪或被修改**的非 ASCII 路径文件。本仓库有两个：`打开Aura项目.bat`（仓库根）、`Docs/类幸存者-单人开发路线.md`。
2. UBT **需要重建 makefile** 才会去查 `git status`。UBT 的失效条件之一是"新增了源码目录"（日志原文 `Invalidating makefile for AuraEditor (source directory added)`）；只改已有目录里的文件不会触发。

这也解释了时间线：两个中文名文件自 2026-09-28 就在，但 9/28~10/10 期间一直只改已有目录里的文件，makefile 不失效、UBT 从不查 git status，构建正常（`Log.uba` 显示最后一次成功构建是 10/10 14:48）。10/13 本方案新增 `Source/Aura/Public/AuraDll/`、`Source/Aura/Private/AuraDll/` 两个**新目录** → 触发 makefile 重建 → 查 git status → 撞上中文路径 → 从此每次构建都崩。

修法（已应用，仓库级）：

```
git config core.quotepath false
```

注意：该配置写在 `.git/config`，**不随仓库分发**——新 clone 若再有非 ASCII 路径的未跟踪/已改文件，需要重新设置。彻底规避的办法是把这类文件名改成纯 ASCII（`打开Aura项目.bat` 可改为 `OpenAuraProject.bat`）。

