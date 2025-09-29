// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Input/AuraInputConfig.h"
#include "Components/SplineComponent.h"
#include "Interaction/EnemyInterface.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "AuraPlayerController.generated.h"

class UDamageTextComponent;
class UAuraAbilitySystemComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	/**
	 * 显示伤害数值 (服务端调用客户端函数)
	 * @param DamageAmount 伤害值
	 * @param TargetCharacter 目标角色
	 * @param bIsBlockedHit 是否格挡
	 * @param bIsCriticalHit 是否命中
	 */
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bIsBlockedHit,
	                      bool bIsCriticalHit);

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;


	void ShiftPressed()
	{
		bShiftKeyDown = true;
	}

	void ShiftRelease()
	{
		bShiftKeyDown = false;
	}

	bool bShiftKeyDown = false;

	/**
	 * 移动函数
	 * @param InputActionValue 输入操作
	 */
	void Move(const FInputActionValue& InputActionValue);

	void CursorTrace();
	// 后续看是否能优化为Ue5的智能指针
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
	/**
	 * 点击命中结果
	 */
	FHitResult CursorHit;


	/**按下绑定回调*/
	void AbilityInputPressed(FGameplayTag InputTag);
	/**释放绑定回调*/
	void AbilityInputReleased(FGameplayTag InputTag);
	/**保持绑定回调*/
	void AbilityInputHeld(FGameplayTag InputTag);

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	/**
	 * 获取能力组件
	 */
	UAuraAbilitySystemComponent* GetAuraAsc();

	/**
	 * 缓存位置
	 */
	FVector CachedDestination;
	float FlowTime;
	/**
	 * 短按持续时间
	 */
	float ShortPressThreshold;
	bool bAutoRunning;
	bool bTargeting;
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius;

	/**
	 * 路径曲线
	 */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;

	void AutoRun();

	/**
	 * 伤害数值浮动组件
	 */
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;
};
