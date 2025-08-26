// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "Engine/Engine.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAttributeSet.generated.h"

/*
 *属性访问生成宏
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

//DECLARE_DELEGATE_RetVal(FGameplayAttribute, FAttributeSignatrue)


/*
 * 效果属性结构体
 */
USTRUCT()
struct FEffectProperties
{
	GENERATED_BODY()
	FEffectProperties();
	FGameplayEffectContextHandle EffectContextHandle;

	UPROPERTY()
	UAbilitySystemComponent* SourceAsc = nullptr;

	UPROPERTY()
	AActor* SourceAvatarActor = nullptr;

	UPROPERTY()
	AController* SourceController = nullptr;

	UPROPERTY()
	ACharacter* SourceCharacter = nullptr;

	UPROPERTY()
	UAbilitySystemComponent* TargetAsc = nullptr;

	UPROPERTY()
	AActor* TargetAvatarActor = nullptr;

	UPROPERTY()
	AController* TargetController = nullptr;

	UPROPERTY()
	ACharacter* TargetCharacter = nullptr;
};

/*
 * typedef is specific to the FGameplayAttribute() signature,but TStaticFuncPtr is generic to any signature chosen.
 */
template <class T>
using TStaticFuncPtr = typename TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr;

/**
 * 效果属性数据类
 * 存放各种效果的数据值
 * 服务器数据变化的监听
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAuraAttributeSet();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	//TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr FunctionPointer;
	//TMap<FGameplayTag, FAttributeSignatrue> TagsToAttributes;
	//TMap<FGameplayTag, TBaseStaticDelegateInstance<FGameplayAttribute(), FDefaultDelegateUserPolicy>::FFuncPtr> TagsToAttributes;
	//FGameplayAttribute(*)()
	//TStaticFuncPtr<float(int32, float, int32)> RandomFunctionPtr;
	//static float RandomFunction(int32 Base, float Multiplier, int32 Rating) { return 0.f; }

	/*
	 * 标签属性Map容器
	 */
	TMap<FGameplayTag, TStaticFuncPtr<FGameplayAttribute()>> TagsToAttributes;

	/*
	 * 预先属性检查
	 */
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/*应用执行效果属性*/
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;


	//-----------------------------------------Primary Attributes-----------------------------------------
	/**
	 * 力量
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_Strength, Category="Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);

	/**
	 * 智力
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence)

	/**
	 * 韧性
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_Resilience, Category = "Primary Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience)

	/**
	 * 活力
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor)

	UFUNCTION()
	void OnRsp_Strength(const FGameplayAttributeData& OldStrength) const;

	UFUNCTION()
	void OnRsp_Intelligence(const FGameplayAttributeData& OldIntelligence) const;

	UFUNCTION()
	void OnRsp_Resilience(const FGameplayAttributeData& OldResilience) const;

	UFUNCTION()
	void OnRsp_Vigor(const FGameplayAttributeData& OldVigor) const;
	//-----------------------------------------Primary Attributes-----------------------------------------

	//-----------------------------------------Secondary Attributes-----------------------------------------

	/**
	 * 法力回复
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_ManaRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration)

	/**
	 * 生命回复
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_HealthRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration)

	/**
	 * 暴击抗性
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_CriticalHitResistance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance)

	/**
	 * 暴击伤害
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_CriticalHitDamage, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage)

	/**
	 * 暴击几率
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_CriticalHitChance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitChance)

	/**
	 * 格挡几率
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_BlockChance, Category = "Secondary Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance)

	/**
	 * 护甲
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_Armor, Category = "Secondary Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor)

	/**
	 * 护甲穿透
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_ArmorPenetration, Category = "Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration)

	/**
	 * 最大生命值
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_MaxHealth, Category = "Secondary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth)

	/**
	 * 最大法力值
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_MaxMana, Category = "Secondary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana)


	UFUNCTION()
	void OnRsp_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;

	UFUNCTION()
	void OnRsp_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;

	UFUNCTION()
	void OnRsp_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;

	UFUNCTION()
	void OnRsp_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;

	UFUNCTION()
	void OnRsp_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;

	UFUNCTION()
	void OnRsp_BlockChance(const FGameplayAttributeData& OldBlockChance) const;

	UFUNCTION()
	void OnRsp_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;

	UFUNCTION()
	void OnRsp_Armor(const FGameplayAttributeData& OldArmor) const;

	UFUNCTION()
	void OnRsp_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	UFUNCTION()
	void OnRsp_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	//-----------------------------------------Secondary Attributes-----------------------------------------

	// -----------------------------------------Vital Attributes-----------------------------------------
	/**
	 * 生命值
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);

	/**
	 * 法力值
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRsp_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana)

	UFUNCTION()
	void OnRsp_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	void OnRsp_Mana(const FGameplayAttributeData& OldMana) const;
	// -----------------------------------------Vital Attributes-----------------------------------------

	// -----------------------------------------Meta Attributes-----------------------------------------
	/**
	 * 即将到来的伤害值
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage)
	// -----------------------------------------Meta Attributes-----------------------------------------

private:
	/*
	 * 设置效果的属性
	 */
	void SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const;

	/**
	 * 展示伤害值
	 * @param Properties 效果属性
	 * @param Damage 伤害值
	 */
	void ShowFloatingText(const FEffectProperties& Properties, float Damage) const;
};
