// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "CombatInterface.generated.h"

/**
 * 攻击标签蒙太奇结构体
 */
USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MontageTag;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 战斗接口
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
	 * 获取战斗Socket位置 [默认蓝图实现，如果C++和蓝图都实现优先蓝图]
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
	 * 获取受击动画蒙太奇 [默认蓝图实现，如果C++和蓝图都实现优先蓝图]
	 * @return 受击动画
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage();

	/**
	 * 死亡纯虚(抽象)函数
	 */
	virtual void Die() = 0;

	/**
	 * 是否死亡 [默认蓝图实现，如果C++和蓝图都实现优先蓝图]
	 * @return 死亡标志位
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;

	/**
	 * 获取演员头像 [默认蓝图实现，如果C++和蓝图都实现优先蓝图]
	 * @return 演员头像
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();

	/**
	 * 攻击蒙太奇数组 [默认蓝图实现，如果C++和蓝图都实现优先蓝图]
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages();
};
