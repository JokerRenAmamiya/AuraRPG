// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

/**
 * 鼠标目标位置向量数据委托
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignatrue, const FGameplayAbilityTargetDataHandle&,
                                            DataHandle);

/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

public:
	/**
	 * 异步创建目标能力
	 * @param OwingAbility 拥有的能力
	 * @return 实例化的目标
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks",
		meta=(DisplayName = "TargetDataUnderMouse", HidePin="OwingAbility", DefaultToSelf="OwingAbility",
			BlueprintInternalUseOnly = true))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwingAbility);

	/**
	 * 目标数据
	 */
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignatrue ValidData;

private:
	virtual void Activate() override;
	/**
	 * 发送鼠标指针数据到服务器
	 */
	void SendMouseCursorData() const;

	/**
	 * 服务器目标数据复制回调
	 * @param DataHandle 数据句柄
	 * @param ActivationTag 激活的标签
	 */
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	                                    FGameplayTag ActivationTag);
};
