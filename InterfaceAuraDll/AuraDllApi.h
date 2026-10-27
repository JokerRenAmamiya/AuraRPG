// AuraDllApi.h
// InterfaceAuraDll 的唯一对外头文件。
//
// 边界规矩（改这个头之前先读设计文档 Docs/superpowers/specs/ 第 6 节）：
//   1. 只传 POD：整型 / 浮点 / 布尔 / 指针+长度 / 不透明句柄
//   2. 不出现 std::string、std::vector、std::function 等 STL 类型，
//      也不出现任何 UE 类型（FString / TArray / UObject*）
//   3. 字符串一律 UTF-8，并显式带长度
//   4. 内存谁分配谁释放
//   5. 异常不跨边界
#pragma once

#include <stdint.h>
#ifndef __cplusplus
#include <stdbool.h>
#endif

// 边界版本：头文件与 DLL 不同步时，调用方应拒绝初始化。
#define AURA_API_VERSION 1

#if defined(_MSC_VER)
#  define AURA_CALL __cdecl
#else
#  define AURA_CALL
#endif

#ifndef INTERFACE_AURA_API
#  ifdef INTERFACE_AURA_DLL_EXPORTS
#    define INTERFACE_AURA_API __declspec(dllexport)
#  else
#    define INTERFACE_AURA_API __declspec(dllimport)
#  endif
#endif

typedef enum AuraEventType
{
	AURA_EVENT_NONE = 0,
	AURA_EVENT_LOG  = 1,
	AURA_EVENT_TEST = 2,   // 仅用于验证链路，验证通过后由真实业务事件替代
} AuraEventType;

// 事件载荷。Message 指向的内存在回调执行期间有效，不得保存。
typedef struct AuraEvent
{
	int32_t     Type;      // AuraEventType
	int32_t     Code;
	const char* Message;   // UTF-8，可为 NULL
} AuraEvent;

// 事件回调。
// 线程契约：可能在任意线程被调用；回调内不得触碰 UE 对象、不得阻塞、不得抛异常。
// UE 侧在回调里只做「拷贝进队列」，回到游戏线程再处理。
typedef void (AURA_CALL *AuraEventCallback)(const AuraEvent* Event, void* UserData);

typedef struct AuraInitDesc
{
	int32_t StructSize;   // 必须等于 sizeof(AuraInitDesc)，用于向后兼容校验
	int32_t LogLevel;     // 0 = 关闭，1 = 普通
} AuraInitDesc;

#ifdef __cplusplus
extern "C" {
#endif

// 返回 AURA_API_VERSION，供调用方校验头文件与 DLL 是否同步。
INTERFACE_AURA_API int32_t AURA_CALL Aura_GetApiVersion(void);

// 实现版本号。
INTERFACE_AURA_API int32_t AURA_CALL Aura_GetVersion(void);

// 生命周期。幂等：已初始化时再次调用返回 true。
INTERFACE_AURA_API bool AURA_CALL Aura_Initialize(const AuraInitDesc* Desc);
INTERFACE_AURA_API void AURA_CALL Aura_Shutdown(void);
INTERFACE_AURA_API bool AURA_CALL Aura_IsInitialized(void);

// 注册 / 注销事件回调（Cb 传 NULL 即注销）。
// 契约：Aura_Shutdown() 返回后，不会再触发任何回调。
INTERFACE_AURA_API void AURA_CALL Aura_SetEventCallback(AuraEventCallback Cb, void* UserData);

// 无状态示例函数。
INTERFACE_AURA_API int32_t AURA_CALL Aura_Add(int32_t A, int32_t B);

#ifdef __cplusplus
}
#endif
