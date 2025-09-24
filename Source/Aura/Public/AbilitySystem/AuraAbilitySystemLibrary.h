// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
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
	 * @param WorldContextObject 世界上下文
	 * @param Asc 能力系统组件
	 */
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaluts")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* Asc);

	/**
	 * 
	 * @param WorldContextObject 世界上下文
	 * @return 角色属性信息
	 */
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaluts")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);


	/**
	 * 获取阻挡命中状态
	 * @param EffectContextHandle 效果上下文句柄
	 * @return 是否命中
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	/**
	 * 获取关键命中状态
	 * @param EffectContextHandle 效果上下文句柄
	 * @return 是否命中
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	/**
	 * 设置格挡命中
	 * @param EffectContextHandle 效果上下文句柄 输入参数引用
	 * @param bInIsBlockedHit 是否格挡
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref)
	                            FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);


	/**
	 * 设置关键命中
	 * @param EffectContextHandle 效果上下文句柄 输入参数引用
	 * @param bInCriticalHit 是否命中
	 */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref)
	                             FGameplayEffectContextHandle& EffectContextHandle, bool bInCriticalHit);
};
