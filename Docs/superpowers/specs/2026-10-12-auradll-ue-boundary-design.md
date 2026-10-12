# AuraDll × UE 边界设计

- 日期：2026-10-12
- 状态：已与作者确认，待进入实施计划
- 选定方案：**A —— `extern "C"` 稳定层 + UE 子系统独占边界**
- 被否决方案：B（直接跨边界调 C++ 单例）、C（COM 风格纯虚接口）。理由见附录。

## 1. 背景与目标

`InterfaceAuraDll` 是一个独立的 MSVC x64 动态库工程（不属于 UE 模块），承载游戏逻辑侧的第三方能力。UE 侧模块 `Aura` 需要**在多个位置频繁调用**它提供的能力，能力形态覆盖：

1. 无状态纯函数；
2. 有状态服务（需要 Initialize / Shutdown 生命周期，内部持有资源）；
3. DLL 反向回调 UE（**同步与异步两者都有**）；
4. 复杂数据（字符串、结构体、数组、二进制块）。

目标：定义一条**稳定、可审计、不会随编译配置漂移**的调用边界，并让 UE 侧各处只通过一个封装层访问 DLL。

## 2. 现状（已核实的事实）

| 事实 | 证据 |
| --- | --- |
| UE 侧接线已存在：包含路径、导入库、运行时 DLL 拷贝 | `Source/Aura/Aura.Build.cs` 第 30-41 行 |
| 采用隐式链接（导入库），非 `LoadLibrary` | 同上：`PublicAdditionalLibraries` + `RuntimeDependencies` |
| UE 侧只能看到 `InterfaceAuraDll.h`，看不到 `Singleton.h` / `AuraMainEntrance.h` | `Source/Aura/ThirdParty/` 目录内容 |
| `ThirdParty` 内的 DLL 是 Debug \| x64 构建（x64、69632 字节、带 `@ILT+` 增量链接跳转） | 该文件 PE 头与导出表 |
| UE 各配置默认使用**发布版 CRT**（`bDebugBuildsActuallyUseDebugCRT` 未开启） | `Source/Aura.Target.cs`、`Source/AuraEditor.Target.cs` 均未设置该项，取默认值 |
| DLL 已正确导出单例与入口符号 | 导出表含 `?Instance@?$Singleton@VAuraMainEntrance@@@@SAAEAVAuraMainEntrance@@XZ`、`?Initialize@AuraMainEntrance@@QEAA_NXZ` |

**由此得出的第一个结论**：当前 `ThirdParty` 中 lib/DLL 是 Debug 发行（`/MDd`），而 UE 模块是发布 CRT（`/MD`），属于配置不匹配状态。轻则触发 `LNK2038` detect_mismatch 报错，重则埋成运行期跨堆释放崩溃。本设计要求统一为 **Release \| x64**。

## 3. 非目标（YAGNI）

- 不做 `LoadLibrary` / `GetProcAddress` 动态加载，不做热插拔（隐式链接已够用）。
- 不做蓝图接口的全量镜像，只暴露确实需要的 `UFUNCTION`。
- 不做跨线程同步直调的性能优化（先统一走队列，有实测瓶颈再议）。
- 不做接口版本化机制，只保留一个整数版本校验。
- 不定义 DLL 的具体业务接口清单。本文档只定义边界与机制；后续业务接口按同一契约增量添加。

## 4. 架构

### 4.1 DLL 侧

| 文件 | 角色 |
| --- | --- |
| `AuraDllApi.h` | **唯一对外头文件**。仅含导出宏（沿用 `INTERFACE_AURA_API`）、`extern "C"` 声明、POD 结构体、回调函数指针 typedef。 |
| `AuraDllApi.cpp` | 薄包装层。编译单元内部包含 `Singleton.h`、`AuraMainEntrance.h`，调用 `Singleton<AuraMainEntrance>::Instance()`。 |

- `Singleton.h`、`AuraMainEntrance.h` **不进入 UE 的包含路径**；`OV_DECLARE_DLL_SINGLETON` 只在 DLL 内部出现。
- `AuraMainEntrance::Initialize()` 保留为 DLL 内部实现，由 `Aura_Initialize()` 调用。
- 既有的 `InterfaceAura_GetVersion()` / `InterfaceAura_Add()` 迁入 `AuraDllApi.h`。若保留 `InterfaceAuraDll.h` 作为实现载体，则该文件降级为 DLL 内部头，不参与分发。
- 后构建事件只同步三个文件到 `Source/Aura/ThirdParty/`：`AuraDllApi.h`、`InterfaceAuraDll.dll`、`InterfaceAuraDll.lib`。

### 4.2 UE 侧

| 组件 | 位置 | 职责 |
| --- | --- | --- |
| `UAuraDllSubsystem` | `Source/Aura/{Public,Private}/AuraDll/AuraDllSubsystem.h/.cpp` | **全模块唯一接触 DLL 的地方**。负责生命周期、事件队列、游戏线程分发、UE 类型转换。 |
| `FAuraDll` 静态类 | `Source/Aura/Public/AuraDll/AuraDllFunctions.h` | 无状态纯函数。纯 C++，不依赖 GameInstance。 |
| `UAuraDllLibrary` | 同上的薄壳 | 仅在蓝图需要调用无状态函数时添加。 |
| `AuraDllConvert.h` | `Source/Aura/Private/AuraDll/` | POD 与 `FString` / `TArray` 的转换集中于此，便于审计边界。 |

**生命周期宿主的选择：`UGameInstanceSubsystem`。**

理由：PIE 与打包均可用；生命周期精确等于"一局游戏"；可挂动态多播委托暴露给蓝图。

切换条件（明确规则）：**若 DLL 初始化开销显著（常驻线程池、大块内存、进程级单例资源），且希望跨关卡、跨 PIE 会话只初始化一次，则改用 `UEngineSubsystem`。** 迁移代价仅为宿主基类与访问器，边界本身不变。

### 4.3 对外 API 形状

本节约定形状与命名规则；逐个签名在实施计划中定稿。

```c
extern "C" {
  int32_t Aura_GetApiVersion(void);                     // 版本校验
  bool    Aura_Initialize(const AuraInitDesc* Desc);    // 生命周期
  void    Aura_Shutdown(void);                          // 契约：返回后不再触发任何回调
  bool    Aura_IsInitialized(void);

  void    Aura_SetEventCallback(AuraEventCallback Cb, void* UserData);  // 事件订阅 / 注销

  int32_t Aura_Add(int32_t A, int32_t B);               // 无状态纯函数示例
}
```

## 5. 数据流

### 5.1 同步调用

UE 代码 → `UAuraDllSubsystem::DoXxx()` →（游戏线程）`Aura_Xxx()` → DLL 内部单例 → 返回 POD → 转换为 UE 类型返回。

### 5.2 回调（同步与异步统一处理）

DLL 侧的 trampoline 是自由函数，**可能在任意线程执行**，它只做一件事：把 POD 事件按值拷入加锁队列。游戏线程 Tick 中 drain 队列 → 转 UE 类型 → 广播动态多播委托。

**关键取舍**：同步回调产生的事件也入队，代价是最多一帧延迟，换来只有一条分发路径、零重入风险（回调在 DLL 调用栈内重入 DLL 是危险状态）。若某处确实需要"本次调用返回前事件即被处理"，在该包装函数末尾显式调用一次 `DrainEvents()` —— 此时 DLL 调用栈已退出，重入是可控的。

Tick 驱动采用 `FTSTicker`（`FTSTicker::GetCoreTicker()`，游戏线程执行，句柄在 `Deinitialize` 中移除）。备选 `FTickableGameObject` 因多一层 UObject 生命周期约束而不采用。

### 5.3 关闭顺序（缺一不可）

1. 置原子标志 `bAcceptingEvents = false`；
2. `Aura_SetEventCallback(nullptr, nullptr)` 注销；
3. `Aura_Shutdown()` —— 阻塞直至 DLL 全部线程停止；
4. 清空并丢弃队列余量；
5. 移除 ticker 句柄。

## 6. 边界契约

以下为硬性规矩，同时作为实施验收项。

1. 跨边界只传 POD：`int32`/`uint32`/`float`/`double`/`bool`/指针+长度/不透明句柄。
2. 边界上**禁止**出现 `std::string`、`std::vector`、`std::function`、`std::shared_ptr`；DLL 头文件中**禁止**出现任何 UE 类型（`FString`、`TArray`、`UObject*` 等）。
3. 字符串一律以 `const char*` 加长度（UTF-8）传递，UE 侧用 `UTF8_TO_TCHAR` 转换。
4. 内存所有权单一：谁分配谁释放。DLL 返回需释放的缓冲时必须同时提供 `Aura_FreeBuffer(void*)`；UE 侧不得 `delete[]` DLL 的内存。
5. 回调必须是 `__cdecl` C 函数指针加 `void* UserData`；DLL 内不得保存任何 UE 对象指针。
6. 异常不跨边界：DLL 内部 `try/catch` 兜住并转为错误码。
7. `Aura_GetApiVersion()` 与 UE 侧期望值不一致时，直接拒绝初始化并记 Error 日志。
8. lib、DLL、头文件三者必须来自同一次构建；UE 侧统一链接 Release \| x64。

## 7. 与现有代码的衔接

1. `Source/Aura/Aura.Build.cs` 现有三行接线保留；补充验证打包 staging 是否包含 `InterfaceAuraDll.dll`。若未包含，改用 `$(BinaryOutputDir)/InterfaceAuraDll.dll` 形式的 `RuntimeDependencies`。
2. 后构建事件改为只同步 `AuraDllApi.h`、DLL、LIB 三项（修掉"公开头只拷了一个"的问题）。
3. **修改 `Aura.sln` 中 `InterfaceAuraDll` 的 `Development Editor | Win64` 映射**：由当前的 `Debug | x64` 改为 `Release | x64`，使编辑器与打包都链接发布版 CRT 的 DLL。当前映射见 `Aura.sln` 第 1736-1755 行；`ThirdParty` 里那个 Debug DLL 正是由此产生。
4. 仓库根 `Debug/InterfaceAuraDll.dll` 为 32 位历史残留，应从工作区移除，避免误取。

## 8. 验证方式

**编译期**：`Aura.Build.cs` 链接通过；UE 模块编译通过。

**运行期（编辑器）**：

1. `Initialize` 返回成功，日志可见；
2. 调用无状态纯函数，返回值正确；
3. 由 DLL 后台线程按固定间隔投递事件，确认能在游戏线程 Tick 中收到、顺序正确；
4. PIE 退出后再次进入：不崩溃、不重复注册回调、无晚到事件穿透。

**打包**：Win64 包内包含 `InterfaceAuraDll.dll`，运行时可正常初始化与收发事件。

## 9. 风险与应对

| 项 | 说明 | 应对 |
| --- | --- | --- |
| 打包 staging | `RuntimeDependencies` 使用 `$(TargetOutputDir)` 时，DLL 是否被正确 stage 进包内需要实测 | 在打包验证阶段确认；不通过则改用 `$(BinaryOutputDir)` 形式 |
| lib/DLL 配置漂移 | 人工在 VS 中切换配置构建，会再次把 Debug 版 DLL 拷进 `ThirdParty` | 统一 sln 映射为 Release \| x64；把"lib/DLL/头同源"写入验收项 |
| 宿主基类 | 若 DLL 初始化开销大，需要 `UEngineSubsystem` | 按 4.2 的切换条件评估；边界不受影响 |

## 附录：被否决方案

**方案 B —— 直接跨边界调 C++ 单例。** 把 `Singleton.h`、`AuraMainEntrance.h` 拷入 `ThirdParty`，在**一个**编译单元中展开 `OV_DECLARE_DLL_SINGLETON(AuraMainEntrance)`（不展开会走模板隐式实例化，导致链接错误），随后调用 `Singleton<AuraMainEntrance>::Instance().Initialize()`。

优点：零包装代码，改 DLL 接口无需同步修改包装层。缺点：ABI 与 MSVC 版本、CRT、STL 实现绑定；边界上不得出现任何 STL 或 UE 类型；lib/DLL 必须严格同配置。在"回调加复杂数据"两者都需要的场景下，这些约束极易被无意打破。

**方案 C —— COM 风格纯虚接口。** `extern "C" Aura_GetApi(void)` 返回纯虚接口指针，所有方法走 vtable。

优点：单一稳定入口、可版本化、类型安全、易于改造成 `LoadLibrary` 热插拔。缺点：需要设计并维护完整接口层，对当前规模属过度设计。

**选择 A 的理由**：它把 Debug/Release 混用、回调线程模型、复杂数据所有权三类风险一次性挡在边界外；UE 侧只需看到一个头文件和一个子系统；DLL 工程保持引擎无关。
