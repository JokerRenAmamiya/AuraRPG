// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class AURA_API ICombatInterface
{
	GENERATED_BODY()

public:
	/**
	 * 获取玩家等级 [接口实现]
	 * @return 玩家等级
	 */
	virtual int32 GetPlayerLevel();

	/**
	 * 获取战斗Socket位置 [蓝图实现]
	 * @return 战斗Socket位置
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FVector GetCombatSocketLocation();

	/**
	 * 转向目标方向 [蓝图实现]
	 * @param Target 目标
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void UpdateFacingTarget(const FVector& Target);

	/**
	 * 获取受击动画蒙太奇 [继承接口的类实现]
	 * @return 受击动画
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	/**
	 * 死亡纯虚(抽象)函数
	 */
	virtual void Die() = 0;

	/**
	 * 是否死亡
	 * @return 死亡标志位
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	/**
	 * 获取演员头像
	 * @return 演员头像
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();
};
