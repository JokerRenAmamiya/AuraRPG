// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interaction/CombatInterface.h"
#include "GameplayModMagnitudeCalculation.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "MMC_MaxHealth.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MaxHealth();

	/*效果计算实现*/
	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	/*活力属性捕捉定义*/
	FGameplayEffectAttributeCaptureDefinition VigorDef;
};
