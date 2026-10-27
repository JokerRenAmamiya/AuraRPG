// AuraDll 的 UE 侧门面：全模块唯一接触 DLL 的地方。
// 生命周期 = 一局游戏；负责初始化 / 关闭、事件跨线程搬运、游戏线程分发。
//
// 边界规矩见 Docs/superpowers/specs/2026-10-12-auradll-ue-boundary-design.md
#pragma once

#include "CoreMinimal.h"
#include "Containers/Queue.h"
#include "Containers/Ticker.h"
#include "Subsystems/GameInstanceSubsystem.h"

#include <atomic>

#include "AuraDllApi.h"
#include "AuraDllSubsystem.generated.h"

/** 事件载荷的 UE 侧形式。 */
USTRUCT(BlueprintType)
struct FAuraDllEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "AuraDll")
	int32 Type = 0;

	UPROPERTY(BlueprintReadOnly, Category = "AuraDll")
	int32 Code = 0;

	UPROPERTY(BlueprintReadOnly, Category = "AuraDll")
	FString Message;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAuraDllEventDelegate, const FAuraDllEvent&, Event);

/**
 * 队列里存的是 POD 快照：DLL 线程只做一次定长拷贝，不构造任何 UE 类型。
 * Message 只在 DLL 回调执行期间有效，所以这里按固定长度截断保存。
 */
struct FAuraDllEventRaw
{
	int32 Type = 0;
	int32 Code = 0;
	ANSICHAR Message[256] = {};
};

UCLASS()
class AURA_API UAuraDllSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	/** DLL 是否已成功初始化。 */
	UFUNCTION(BlueprintPure, Category = "AuraDll")
	bool IsDllReady() const { return bDllReady; }

	/** 事件广播。只在游戏线程触发。 */
	UPROPERTY(BlueprintAssignable, Category = "AuraDll")
	FAuraDllEventDelegate OnDllEvent;

	/** 立即派发队列中的事件。通常由 Tick 驱动。 */
	void DrainEvents();

	/** 便捷取用，WorldContextObject 可以是任意 UE 对象。 */
	static UAuraDllSubsystem* Get(const UObject* WorldContextObject);

private:
	bool Tick(float DeltaTime);

	/**
	 * DLL 回调入口：可能在任意线程被调用，只做拷贝入队。
	 * 这里绝不触碰 UE 对象、不记日志、不阻塞。
	 */
	static void AURA_CALL EventTrampoline(const AuraEvent* Event, void* UserData);

	/** 关闭流程一旦开始就停止接受事件。 */
	std::atomic<bool> bAcceptingEvents{false};

	bool bDllReady = false;

	TQueue<FAuraDllEventRaw, EQueueMode::Mpsc> PendingEvents;

	FTSTicker::FDelegateHandle TickerHandle;
};
