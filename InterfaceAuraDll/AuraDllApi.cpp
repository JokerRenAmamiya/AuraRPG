// AuraDllApi.cpp
// 边界包装层的唯一实现单元：把 C ABI 调用转成 DLL 内部的 C++ 单例调用。
//
// 单例与 AuraMainEntrance 只在这里出现。UE 侧看不到 Singleton.h / AuraMainEntrance.h，
// 这两个头也不随 DLL 分发。
#include "AuraDllApi.h"

#include <atomic>

#include "Singleton.h"
#include "AuraMainEntrance.h"

// 显式特化必须在调用它的编译单元里声明一次，否则编译器会尝试实例化模板并报链接错误。
OV_DECLARE_DLL_SINGLETON(AuraMainEntrance)

namespace
{
	std::atomic<bool>              GInitialized{ false };
	std::atomic<AuraEventCallback> GEventCallback{ nullptr };
	std::atomic<void*>             GEventUserData{ nullptr };

	// 事件出口。同步场景下在调用栈内触发，异步场景下由后台线程触发；
	// 两种情况走同一条路径，调用方（UE 侧）自行负责线程切换。
	void EmitEvent(const AuraEvent& Event)
	{
		AuraEventCallback Callback = GEventCallback.load(std::memory_order_acquire);
		if (Callback != nullptr)
		{
			Callback(&Event, GEventUserData.load(std::memory_order_relaxed));
		}
	}
}

extern "C"
{
	int32_t AURA_CALL Aura_GetApiVersion(void)
	{
		return AURA_API_VERSION;
	}

	int32_t AURA_CALL Aura_GetVersion(void)
	{
		return 1;
	}

	bool AURA_CALL Aura_Initialize(const AuraInitDesc* Desc)
	{
		if (Desc != nullptr && Desc->StructSize != static_cast<int32_t>(sizeof(AuraInitDesc)))
		{
			return false; // 头文件与 DLL 不同步
		}

		if (GInitialized.load(std::memory_order_acquire))
		{
			return true; // 幂等
		}

		try
		{
			if (!Singleton<AuraMainEntrance>::Instance().Initialize())
			{
				return false;
			}
		}
		catch (...)
		{
			return false; // 异常不跨边界
		}

		GInitialized.store(true, std::memory_order_release);

		AuraEvent Event = {};
		Event.Type = AURA_EVENT_LOG;
		Event.Code = 0;
		Event.Message = "AuraDll initialized";
		EmitEvent(Event);

		return true;
	}

	void AURA_CALL Aura_Shutdown(void)
	{
		// 当前没有内部线程，置位即可。
		// 加入后台线程后，这里必须先停线程并等待其结束，再返回 —— 这是契约的一部分。
		GInitialized.store(false, std::memory_order_release);
	}

	bool AURA_CALL Aura_IsInitialized(void)
	{
		return GInitialized.load(std::memory_order_acquire);
	}

	void AURA_CALL Aura_SetEventCallback(AuraEventCallback Cb, void* UserData)
	{
		GEventUserData.store(UserData, std::memory_order_relaxed);
		GEventCallback.store(Cb, std::memory_order_release);
	}

	int32_t AURA_CALL Aura_Add(int32_t A, int32_t B)
	{
		return A + B;
	}
}
