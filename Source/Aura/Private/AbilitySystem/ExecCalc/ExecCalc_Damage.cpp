// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

struct AuraDamageStatics
{
	// 宏魔法！！！
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor)
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage)
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance)

	AuraDamageStatics()
	{
		// 宏魔法！！！
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
	}
};

/**
 * 创建静态伤害结构体函数
 * @return 静态伤害结构体
 */
static const AuraDamageStatics& DamageStatics()
{
	// 静态构建结构体
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	// 添加属性
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
                                              FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	// 组件获取
	const UAbilitySystemComponent* SourceAsc = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetAsc = ExecutionParams.GetTargetAbilitySystemComponent();

	// 获取演员
	AActor* SourceAvatar = SourceAsc ? SourceAsc->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetAsc ? TargetAsc->GetAvatarActor() : nullptr;
	// 转换接口
	ICombatInterface* SourceCombatInterface = SourceAvatar ? Cast<ICombatInterface>(SourceAvatar) : nullptr;
	ICombatInterface* TargetCombatInterface = TargetAvatar ? Cast<ICombatInterface>(TargetAvatar) : nullptr;

	// 持有的规范效果
	const FGameplayEffectSpec Spec = ExecutionParams.GetOwningSpec();
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	// 计算参数
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	// Get Damage Set by Caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	// 获取目标格挡概率
	float TargetBlockChance = GetCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, ExecutionParams,
	                                                        EvaluateParameters);
	// 判断格挡概率
	const bool bBlocked = FMath::RandRange(0.f, 100.f) < TargetBlockChance;
	// 判定成功衰减伤害
	Damage = bBlocked ? Damage / 2.f : 0.f;

	// 获取目标护甲值
	float TargetArmor = GetCapturedAttributeMagnitude(DamageStatics().ArmorDef, ExecutionParams, EvaluateParameters);

	// 获取自身穿甲值
	float SourceArmorPenetration = GetCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, ExecutionParams,
	                                                             EvaluateParameters);
	// 获取自身角色数据
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar);
	// 获取敌人角色数据
	const UCharacterClassInfo* EnemyClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(TargetAvatar);

	// 根据自身等级获取自身护甲值曲线
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("ArmorPenetration"), FString(""));
	// 获取对应级别的护甲基数
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel());

	// 计算穿甲值
	const float EffectiveArmor = TargetArmor * (100.f - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	// 根据自身等级获取目标（敌人）护甲曲线
	const FRealCurve* EffectiveArmorCurve = EnemyClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("EffectiveArmor"),
		FString(""));
	// 获取对应级别的穿甲基数
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel());
	// 计算最终伤害 Armor ignores a percentage of incoming damage.
	Damage *= (100.f - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;

	// 获取自身暴击几率
	float SourceCriticalChance = GetCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, ExecutionParams,
	                                                           EvaluateParameters);

	// 获取目标暴击抗性
	float TargetCriticalHitResistance = GetCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef,
	                                                                  ExecutionParams, EvaluateParameters);

	// 获取自身暴击伤害
	float SourceCriticalHitDamage = GetCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, ExecutionParams,
	                                                              EvaluateParameters);

	// 根据自身等级获取对应暴击抗性曲线
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(
		FName("CriticalHitResistance"), FString(""));
	const float EffectiveCriticalHitResistanceCoefficient =
		CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel());

	// Critical Hit Resistance reduces critical hit chance.
	const float EffectiveCriticalHitChance = SourceCriticalChance - TargetCriticalHitResistance *
		EffectiveCriticalHitResistanceCoefficient;
	const bool bCriticalHit = FMath::RandRange(1.f, 100.f) < EffectiveCriticalHitChance;
	// Double damage plus a bonus if critical hit.
	Damage = bCriticalHit ? 2.f * Damage + SourceCriticalHitDamage : Damage;

	// 构造输出数据
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(),
	                                                   EGameplayModOp::Additive, Damage);
	// 添加输出结果
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
