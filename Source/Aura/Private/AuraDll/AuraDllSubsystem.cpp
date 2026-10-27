#include "AuraDll/AuraDllSubsystem.h"

#include "AuraDll/AuraDllFunctions.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"

DEFINE_LOG_CATEGORY_STATIC(LogAuraDll, Log, All);

void UAuraDllSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (!AuraDll::CheckApiVersion())
	{
		UE_LOG(LogAuraDll, Error,
			TEXT("边界版本不一致：头文件期望 %d，运行时 DLL 返回 %d。")
			TEXT("请重新构建 InterfaceAuraDll（Release|x64）并同步 Source/Aura/ThirdParty/。"),
			static_cast<int32>(AURA_API_VERSION), static_cast<int32>(Aura_GetApiVersion()));
		return;
	}

	// 先注册回调，再初始化 —— 否则初始化期间产生的事件会丢。
	bAcceptingEvents.store(true, std::memory_order_release);
	Aura_SetEventCallback(&UAuraDllSubsystem::EventTrampoline, this);

	AuraInitDesc Desc = {};
	Desc.StructSize = static_cast<int32>(sizeof(AuraInitDesc));
	Desc.LogLevel = 1;

	if (!Aura_Initialize(&Desc))
	{
		UE_LOG(LogAuraDll, Error, TEXT("Aura_Initialize 失败。"));
		Aura_SetEventCallback(nullptr, nullptr);
		bAcceptingEvents.store(false, std::memory_order_release);
		return;
	}

	bDllReady = true;

	TickerHandle = FTSTicker::GetCoreTicker().AddTicker(
		FTickerDelegate::CreateUObject(this, &UAuraDllSubsystem::Tick));

	UE_LOG(LogAuraDll, Log, TEXT("初始化完成。DLL 版本 %d，边界版本 %d。"),
		AuraDll::GetVersion(), static_cast<int32>(Aura_GetApiVersion()));
}

void UAuraDllSubsystem::Deinitialize()
{
	// 关闭顺序（设计文档 5.3），顺序不能改：
	// 1) 停止接受新事件  2) 注销回调  3) 阻塞式关停（返回后不应再有任何回调）
	// 4) 丢弃队列余量    5) 移除 ticker
	bAcceptingEvents.store(false, std::memory_order_release);
	Aura_SetEventCallback(nullptr, nullptr);
	Aura_Shutdown();

	int32 Discarded = 0;
	FAuraDllEventRaw Ignored;
	while (PendingEvents.Dequeue(Ignored))
	{
		++Discarded;
	}
	if (Discarded > 0)
	{
		UE_LOG(LogAuraDll, Verbose, TEXT("关闭时丢弃了 %d 个未派发事件。"), Discarded);
	}

	if (TickerHandle.IsValid())
	{
		FTSTicker::RemoveTicker(TickerHandle);
		TickerHandle.Reset();
	}

	bDllReady = false;

	Super::Deinitialize();
}

void UAuraDllSubsystem::DrainEvents()
{
	check(IsInGameThread());

	FAuraDllEventRaw Raw;
	while (PendingEvents.Dequeue(Raw))
	{
		FAuraDllEvent Event;
		Event.Type = Raw.Type;
		Event.Code = Raw.Code;
		Event.Message = FString(UTF8_TO_TCHAR(Raw.Message));

		OnDllEvent.Broadcast(Event);
	}
}

bool UAuraDllSubsystem::Tick(float DeltaTime)
{
	DrainEvents();
	return true;
}

UAuraDllSubsystem* UAuraDllSubsystem::Get(const UObject* WorldContextObject)
{
	if (GEngine == nullptr)
	{
		return nullptr;
	}

	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::ReturnNull))
	{
		if (UGameInstance* GameInstance = World->GetGameInstance())
		{
			return GameInstance->GetSubsystem<UAuraDllSubsystem>();
		}
	}

	return nullptr;
}

void AURA_CALL UAuraDllSubsystem::EventTrampoline(const AuraEvent* Event, void* UserData)
{
	UAuraDllSubsystem* Self = static_cast<UAuraDllSubsystem*>(UserData);
	if (Self == nullptr || Event == nullptr)
	{
		return;
	}

	if (!Self->bAcceptingEvents.load(std::memory_order_acquire))
	{
		return; // 关闭中：直接丢弃
	}

	// 定长拷贝。Message 指向的内存在回调返回后即失效，所以必须在这里拷走。
	FAuraDllEventRaw Raw;
	Raw.Type = static_cast<int32>(Event->Type);
	Raw.Code = static_cast<int32>(Event->Code);
	if (Event->Message != nullptr)
	{
		const int32 MaxLength = static_cast<int32>(UE_ARRAY_COUNT(Raw.Message)) - 1;
		const int32 Length = FMath::Min<int32>(FCStringAnsi::Strlen(Event->Message), MaxLength);
		FMemory::Memcpy(Raw.Message, Event->Message, Length);
		Raw.Message[Length] = '\0';
	}

	Self->PendingEvents.Enqueue(Raw);
}
