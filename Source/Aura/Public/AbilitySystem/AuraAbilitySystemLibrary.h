// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

struct FGameplayEffectContextHandle;
class UAbilitySystemComponent;
class UAttributeMenuWidgetController;
class UOverlayWidgetController;


/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 获取主UI控制器
	 * @param WorldContextObject 对象实例
	 * @return 主UI控制器
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);

	/**
	 * 获取菜单UI控制器
	 * @param WorldContextObject 对象实例
	 * @return 菜单UI控制器
	 */
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|WidgetController")
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	/**
	 * 初始化属性信息
	 * @param WorldContextObject 世界上下文
	 * @param CharacterClass 角色枚举
	 * @param Level 等级
	 * @param Asc 能力系统组件
	 */
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaluts")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass,
	                                        float Level, UAbilitySystemComponent* Asc);

	/**
	 * 给与启动不同的能力
	 * @param CharacterClass 角色枚举
	 * @param WorldContextObject 世界上下文
	 * @param Asc 能力系统组件
	 */
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaluts")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* Asc,
	                                 ECharacterClass CharacterClass);

	/**
	 * 
	 * @param WorldContextObject 世界上下文
	 * @return 角色属性信息
	 */
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaluts")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);


	/**
	 * 蓝图函数
	 * 判断是否格挡
	 * @param EffectContextHandle 游戏效果上下文
	 * @return 是否格挡
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	/**
	 * 蓝图函数
	 * 判断是否暴击
	 * @param EffectContextHandle 游戏效果上下文
	 * @return 是否暴击
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	/**
	 * 蓝图函数
	 * 设置是否格挡
	 * @param EffectContextHandle 游戏效果上下文
	 * @param bInIsBlockedHit 是否格挡
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);

	/**
	 * 蓝图函数
	 * 设置是否暴击
	 * @param EffectContextHandle 游戏效果上下文
	 * @param bInIsCritical 是否暴击
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCritical(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCritical);
};
