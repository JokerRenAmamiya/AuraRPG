// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * 全局技能系统类
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()

	/**
	 * 分配一个游戏效果上下文
	 * @return 分配的游戏效果上下文
	 */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
