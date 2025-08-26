// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer&/*AssetTags*/);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	/*
	 * 属性集合设置
	 */
	void AbilityActorInfoSet();

	/**
	 * 标签资产委托
	 */
	FEffectAssetTags OnEffectAssetTags;

	/**
	 * 添加角色能力
	 * @param StartupAbilities 能力列表
	 */
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> StartupAbilities);

	/**
	 * 能力输入保持
	 * @param InputTag 标签
	 */
	void AbilityInputTagHeld(const FGameplayTag& InputTag);

	/**
	 * 能力输入释放
	 * @param InputTag 标签
	 */
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

protected:
	/**
	 * 效果应用后回调 客户端RPC 服务器主动调用客户端上的函数 调用权限：只能由服务器调用（客户端调用会被 UE 自动忽略，避免作弊）。
	 * Client：指定该函数是客户端 RPC，即函数的实现逻辑在客户端，但调用的触发来自服务器。
		（原理：服务器会将函数调用的参数打包发送给目标客户端，客户端接收后执行函数体。）
	 * Reliable：指定该 RPC 是可靠的，即底层网络层会保证：
		1.函数调用的数据包会被重传直到客户端接收成功（不会因网络波动丢失）。
		2.数据包会按发送顺序被客户端处理（不会乱序）。
	 * @param AbilitySystemComponent 能力组件
	 * @param EffectSpec 效果
	 * @param ActiveHandle 效果激活句柄
	 */
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec,
	                         FActiveGameplayEffectHandle ActiveHandle) const;
};
