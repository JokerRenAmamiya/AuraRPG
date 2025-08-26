// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameplayTags.h"
#include "GameplayTagsManager.h"

FAuraGameplayTags FAuraGameplayTags::GameplayTags;

void FAuraGameplayTags::InitializeNativeGameplayTags()
{
	/*获取单例指针*/
	UGameplayTagsManager* GameplayTagsManager = &UGameplayTagsManager::Get();
	if (GameplayTagsManager == nullptr)
		return;

	//------------------------------------------添加游戏标签主要属性------------------------------------------
	GameplayTags.Attributes_Primary_Strength = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Primary.Strength"), FString("Increases physical damage"));
	GameplayTags.Attributes_Primary_Intelligence = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Primary.Intelligence"), FString("Increases magical damage"));
	GameplayTags.Attributes_Primary_Resilience = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Primary.Resilience"), FString("Increases armor and armor penetration"));
	GameplayTags.Attributes_Primary_Vigor = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Primary.Vigor"), FString("Increases health"));
	//------------------------------------------添加游戏标签主要属性------------------------------------------


	//------------------------------------------添加游戏标签次要属性------------------------------------------
	GameplayTags.Attributes_Secondary_MaxHealth = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxHealth"), FString("Maximum amount of Health obtainable"));
	GameplayTags.Attributes_Secondary_MaxMana = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.MaxMana"), FString("Maximum amount of Mana obtainable"));
	GameplayTags.Attributes_Secondary_Armor = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.Armor"), FString("Reduces damage taken,improves Block Chance"));
	GameplayTags.Attributes_Secondary_ArmorPenetration = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.ArmorPenetration"),
		FString("Ignores percentage of enemy Armor,increases Crit Hit Chance"));
	GameplayTags.Attributes_Secondary_BlockChance = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.BlockChance"),
		FString("Chance to cut incoming damage in hal"));
	GameplayTags.Attributes_Secondary_CriticalHitChance = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitChance"),
		FString("Chance to double damage plus critical hit bonus"));
	GameplayTags.Attributes_Secondary_CriticalHitDamage = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitDamage"),
		FString("Bonus damage added when a critical hit is scored"));
	GameplayTags.Attributes_Secondary_CriticalHitResistance = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.CriticalHitResistance"),
		FString("Reduces critical hit change of attacking enemies"));
	GameplayTags.Attributes_Secondary_HealthRegeneration = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.HealthRegeneration"),
		FString("Amount of Health regenerated every 1 second"));
	GameplayTags.Attributes_Secondary_ManaRegeneration = GameplayTagsManager->AddNativeGameplayTag(
		FName("Attributes.Secondary.ManaRegeneration"),
		FString("Amount of Mana regenerated every 1 second"));
	//------------------------------------------添加游戏标签次要属性------------------------------------------

	//------------------------------------------添加消息标签------------------------------------------
	GameplayTags.Message_PotionHealth = GameplayTagsManager->AddNativeGameplayTag(
		FName("Message.PotionHealth"),
		FString("PotionHealth Msg"));
	GameplayTags.Message_PotionMana = GameplayTagsManager->AddNativeGameplayTag(
		FName("Message.PotionMana"),
		FString("PotionMana Msg"));
	GameplayTags.Message_CrystalHealth = GameplayTagsManager->AddNativeGameplayTag(
		FName("Message.CrystalHealth"),
		FString("CrystalHealth Msg"));
	GameplayTags.Message_CrystalMana = GameplayTagsManager->AddNativeGameplayTag(
		FName("Message.CrystalMana"),
		FString("CrystalMana Msg"));
	//------------------------------------------添加消息标签------------------------------------------

	//------------------------------------------添加游戏输入标签------------------------------------------
	GameplayTags.InputTag_LMB = GameplayTagsManager->AddNativeGameplayTag(
		FName("InputTag.LMB"), FString("Input Tag for Left Mouse Button"));
	GameplayTags.InputTag_RMB = GameplayTagsManager->AddNativeGameplayTag(
		FName("InputTag.RMB"), FString("Input Tag for Right Mouse Button"));
	GameplayTags.InputTag_1 = GameplayTagsManager->AddNativeGameplayTag(
		FName("InputTag.1"), FString("Input Tag for 1 Key"));
	GameplayTags.InputTag_2 = GameplayTagsManager->AddNativeGameplayTag(
		FName("InputTag.2"), FString("Input Tag for 2 Key"));
	GameplayTags.InputTag_3 = GameplayTagsManager->AddNativeGameplayTag(
		FName("InputTag.3"), FString("Input Tag for 3 Key"));
	GameplayTags.InputTag_4 = GameplayTagsManager->AddNativeGameplayTag(
		FName("InputTag.4"), FString("Input Tag for 4 Key"));
	//------------------------------------------添加游戏输入标签------------------------------------------

	//------------------------------------------添加伤害类标签------------------------------------------
	GameplayTags.Damage = GameplayTagsManager->AddNativeGameplayTag(
		FName("Damage"), FString("Damage"));
	//------------------------------------------添加伤害类标签------------------------------------------

	//------------------------------------------添加击中效果标签------------------------------------------
	GameplayTags.Effects_HitReact = GameplayTagsManager->AddNativeGameplayTag(
		FName("Effects.HitReact"), FString("Tag granted when Hit Reacting"));
	//------------------------------------------添加击中效果标签------------------------------------------
}
