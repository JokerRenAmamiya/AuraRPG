// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:
	UExecCalc_Damage();
	/**
	 * 执行计算函数
	 * @param ExecutionParams 计算参数
	 * @param OutExecutionOutput 计算输出
	 */
	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                    FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

	/**
	 * 返回对应属性定义的数值
	 * @tparam AttrDef 属性
	 * @param AttributeDef 属性定义
	 * @param ExecutionParams 执行参数
	 * @param EvaluateParameters 计算参数
	 * @return 对应属性数值
	 */
	template <typename AttrDef>
	static float GetCapturedAttributeMagnitude(AttrDef& AttributeDef,
	                                           const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	                                           const FAggregatorEvaluateParameters& EvaluateParameters);
};

template <typename AttrDef>
float UExecCalc_Damage::GetCapturedAttributeMagnitude(AttrDef& AttributeDef,
                                                      const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                                      const FAggregatorEvaluateParameters& EvaluateParameters)
{
	float Value = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AttributeDef, EvaluateParameters, Value);
	return FMath::Max<float>(Value, 0.f);
}
