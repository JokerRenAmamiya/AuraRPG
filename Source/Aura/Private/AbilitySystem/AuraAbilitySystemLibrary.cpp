// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();
			UAttributeSet* As = PS->GetAttributeSet();
			const FWidgetControllerParams FWidgetControllerParams(PC, PS, Asc, As);
			return AuraHUD->GetOverlayWidgetController(FWidgetControllerParams);
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			UAbilitySystemComponent* Asc = PS->GetAbilitySystemComponent();
			UAttributeSet* As = PS->GetAttributeSet();
			const FWidgetControllerParams FWidgetControllerParams(PC, PS, Asc, As);
			return AuraHUD->GetAttributeMenuWidgetController(FWidgetControllerParams);
		}
	}
	return nullptr;
}

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
                                                            const ECharacterClass CharacterClass,
                                                            const float Level,
                                                            UAbilitySystemComponent* Asc)
{
	// 角色
	const AActor* AvatarActor = Asc->GetAvatarActor();
	// 角色信息
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	// 效果上下文
	FGameplayEffectContextHandle EffectContext = Asc->MakeEffectContext();
	// 添加到角色源
	EffectContext.AddSourceObject(AvatarActor);
	// 默认主要属性
	auto [PrimaryAttributes] = CharacterClassInfo->GetClassDefaultInfo(
		CharacterClass);
	const FGameplayEffectSpecHandle PrimaryEffectSpecHandle = Asc->MakeOutgoingSpec(
		PrimaryAttributes, Level, EffectContext);
	Asc->ApplyGameplayEffectSpecToSelf(*PrimaryEffectSpecHandle.Data.Get());

	// 次要属性
	const FGameplayEffectSpecHandle SecondaryEffectSpecHandle = Asc->MakeOutgoingSpec(
		CharacterClassInfo->SecondaryAttributes, Level, EffectContext);
	Asc->ApplyGameplayEffectSpecToSelf(*SecondaryEffectSpecHandle.Data.Get());

	// 必要属性
	const FGameplayEffectSpecHandle VitalEffectSpecHandle = Asc->MakeOutgoingSpec(
		CharacterClassInfo->VitalAttributes, Level, EffectContext);
	Asc->ApplyGameplayEffectSpecToSelf(*VitalEffectSpecHandle.Data.Get());
}

void UAuraAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* Asc)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr)
		return;
	// 赋予敌人能力
	for (const TSubclassOf<UGameplayAbility> Ability : CharacterClassInfo->Abilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Ability, 1);
		Asc->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(WorldContextObject);
	const AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(GameModeBase);
	if (AuraGameMode == nullptr)
		return nullptr;

	return AuraGameMode->CharacterClassInfo;
}
