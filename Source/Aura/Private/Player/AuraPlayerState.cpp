// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"

#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AuraAbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AuraAbilitySystemComponent");
	AuraAbilitySystemComponent->SetIsReplicated(true);
	// 设置复制模式
	AuraAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	AuraAttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	// 设置网络更新频率
	SetNetUpdateFrequency(100.f);
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(AAuraPlayerState, Level, COND_None, REPNOTIFY_Always);
}

UAuraAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AuraAbilitySystemComponent;
}

void AAuraPlayerState::OnRsp_Level(int32 OldLevel)
{
}
