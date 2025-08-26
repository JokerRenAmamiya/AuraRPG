// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 * Singleton containing native Gameplay Tags
 */
struct FAuraGameplayTags
{
public:
	/*实例获取getter*/
	static const FAuraGameplayTags& Get() { return GameplayTags; }
	/*初始化*/
	static void InitializeNativeGameplayTags();

	//---------------------------------主要属性---------------------------------
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;
	//---------------------------------主要属性---------------------------------

	//---------------------------------次要属性---------------------------------
	FGameplayTag Attributes_Secondary_Armor;
	FGameplayTag Attributes_Secondary_ArmorPenetration;
	FGameplayTag Attributes_Secondary_BlockChance;
	FGameplayTag Attributes_Secondary_CriticalHitChance;
	FGameplayTag Attributes_Secondary_CriticalHitDamage;
	FGameplayTag Attributes_Secondary_CriticalHitResistance;
	FGameplayTag Attributes_Secondary_HealthRegeneration;
	FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
	FGameplayTag Attributes_Secondary_MaxMana;
	//---------------------------------次要属性---------------------------------

	//---------------------------------HUdMessage---------------------------------
	FGameplayTag Message_PotionHealth;
	FGameplayTag Message_PotionMana;
	FGameplayTag Message_CrystalHealth;
	FGameplayTag Message_CrystalMana;
	//---------------------------------HUdMessage---------------------------------

	//---------------------------------键位输入---------------------------------
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	//---------------------------------键位输入---------------------------------

	//---------------------------------伤害---------------------------------
	FGameplayTag Damage;
	//---------------------------------伤害---------------------------------
	
	//---------------------------------击中效果---------------------------------
	FGameplayTag Effects_HitReact;
	//---------------------------------击中效果---------------------------------

protected:
private:
	static FAuraGameplayTags GameplayTags;
};
