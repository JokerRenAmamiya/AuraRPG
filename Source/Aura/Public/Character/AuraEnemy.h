// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/EnemyInterface.h"
#include "Character/AuraCharacterBase.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AuraEnemy.generated.h"

class UWidgetComponent;
class UBehaviorTree;
class AAuraAIController;

/**
 * 敌人类，可拓展不同种类的敌人
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	virtual void PossessedBy(AController* NewController) override;

	/* Enemy Interface */
	/**
	 * 高亮演员
	 */
	virtual void HighlightActor() override;
	/**
	 * 取消高亮演员
	 */
	virtual void UnHighlightActor() override;
	/* End Enemy Interface */

	/* Combat Interface */
	/**
	 * 获取演员等级
	 */
	virtual int32 GetPlayerLevel() override;
	/**
	 * 演员死亡
	 */
	virtual void Die() override;
	/* End Combat Interface */

	/**
	 * 设置战斗目标 重写实现
	 * @param InCombatTarget 战斗目标
	 */
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;

	/**
	 * 获取战斗目标 重写实现
	 */
	virtual AActor* GetCombatTarget_Implementation() const override;

	/**
	 * 生命值变化广播
	 */
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChange;

	/**
	 * 最大生命值变化广播
	 */
	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChange;

	/**
	 * 击中标签变化 
	 * @param CallbackTag 回调标签
	 * @param NewCount 击中次数
	 */
	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;

	/**
	 * 基础行走速度
	 */
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 260.f;

	/**
	 * 存在时间
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;

	/**
	 * 战斗目标
	 */
	UPROPERTY(BlueprintReadWrite, Category="Combat")
	TObjectPtr<AActor> CombatTarget;

protected:
	virtual void BeginPlay() override;
	/**
	 * 初始化演员信息
	 */
	virtual void InitAbilityActorInfo() override;
	virtual void InitializeDefaultAttributes() const override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults ")
	int32 Level = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults ")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<AAuraAIController> AuraAIController;
};
