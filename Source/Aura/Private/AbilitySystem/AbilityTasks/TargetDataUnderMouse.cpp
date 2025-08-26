// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwingAbility)
{
	UTargetDataUnderMouse* TargetData = NewAbilityTask<UTargetDataUnderMouse>(OwingAbility);
	return TargetData;
}

void UTargetDataUnderMouse::Activate()
{
	// 获取控制权限
	const bool isLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	// 判断是否为本地控制
	if (isLocallyControlled)
	{
		// 在本地，发送鼠标指针数据
		SendMouseCursorData();
	}
	else
	{
		// 转换弱引用指针为原始指针 因为需要直接调用成员函数
		UAbilitySystemComponent* AbilitySystemComponentPtr = AbilitySystemComponent.Get();
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey = GetActivationPredictionKey();
		// 在服务器上，监听目标数据
		AbilitySystemComponentPtr->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictionKey)
		                         .AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		// 告诉服务器复制数据
		const bool bCalledDelegate = AbilitySystemComponentPtr->CallReplicatedTargetDataDelegatesIfSet(
			SpecHandle, ActivationPredictionKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData() const
{
	// 预测窗口
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	const APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// 目标数据句柄
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	// 目标数据赋值
	Data->HitResult = CursorHit;
	// 打印点击位置
	UE_LOG(LogTemp, Warning, TEXT("UTargetDataUnderMouse位置: X=%.2f, Y=%.2f, Z=%.2f"),
	       CursorHit.Location.X, CursorHit.Location.Y, CursorHit.Location.Z);
	DataHandle.Add(Data);
	// 服务器设置复制的目标数据
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey
	);
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
                                                           FGameplayTag ActivationTag)
{
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
