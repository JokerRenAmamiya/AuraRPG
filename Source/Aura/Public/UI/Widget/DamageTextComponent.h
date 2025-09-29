// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "DamageTextComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UDamageTextComponent : public UWidgetComponent
{
	GENERATED_BODY()

public:
	/**
	 * 设置伤害漂浮字体数值
	 * @param Damage 伤害数值
	 * @param bIsBlockedHit 是否格挡
	 * @param bIsCriticalHit 是否命中
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage, bool bIsBlockedHit, bool bIsCriticalHit);
};
