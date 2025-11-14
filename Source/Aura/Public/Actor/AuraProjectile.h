// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class UNiagaraSystem;
class USphereComponent;
class UProjectileMovementComponent;

/**
 * 投射物类 可衍生出火球 冰球 飞镖等投射物
 */
UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()

public:
	AAuraProjectile();

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/**
	 * 伤害效果
	 */
	UPROPERTY(BlueprintReadWrite, meta=(ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	/**
	 * 当物体重叠到其他物体
	 * @param OverlappedComp 重叠到的组件
	 * @param OtherActor 重叠到的物体
	 * @param OtherComp 其他组件
	 * @param OtherBodyIndex 其他下标
	 * @param bFromSweep 是否扫描
	 * @param SweepResult 扫描结果
	 */
	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * 被击中后触发
	 */
	void OnHit();

	/**
	 * 是否有效堆叠
	 * @param OtherActor 对象
	 * @return 是否有效
	 */
	bool IsValidOverlap(AActor* OtherActor);

private:
	/**
	 * 存在时间
	 */
	UPROPERTY(EditDefaultsOnly)
	float LeftSpan = 15.f;

	bool bHit = false;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;
};
