// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	/**
	 * 初始化游戏效果上下文
	 * @return 自定义游戏效果上下文
	 */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
