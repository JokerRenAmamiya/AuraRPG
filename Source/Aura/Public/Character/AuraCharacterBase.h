#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GameFramework/Character.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"
#include "AuraCharacterBase.generated.h"

class UAuraAttributeSet;

UCLASS()
class AURA_API AAuraCharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AAuraCharacterBase();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	// -----------------------------------Combat Interface-----------------------------------
	/**
	 * 获取受击动画蒙太奇 [继承接口的类实现]
	 * @return 受击动画
	 */
	virtual UAnimMontage* GetHitReactMontage_Implementation() override;

	/**
	 * 死亡函数
	 */
	virtual void Die() override;

	/**
	 * 获取武器套索位置 [继承接口的类实现]
	 * @return 套索位置
	 */
	virtual FVector GetCombatSocketLocation_Implementation() override;

	/**
	 * 是否死亡 [继承接口的类实现]
	 * @return 死亡标志位
	 */
	virtual bool IsDead_Implementation() const override;

	/**
	 * 获取角色头像 [继承接口的类实现]
	 * @return 角色头像
	 */
	virtual AActor* GetAvatar_Implementation() override;
	// -----------------------------------Combat Interface-----------------------------------

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Combat")
	FName WeaponTipSocketName;

	/**
	 * 死亡标志位
	 */
	bool bDead = false;

	/**
	 * 能力组件
	 */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	/**
	 * 能力数据
	 */
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual void InitAbilityActorInfo();

	/**
	 * 默认主要属性
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	/**
	 * 默认次要属性
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	/**
	 * 默认重要属性
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;

	/**
	 * 默认抵抗属性
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultResistanceAttributes;

	/*
	 * 对自己应用效果
	 */
	void ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass, float Level) const;

	/*
	 * 初始化属性集合
	 */
	virtual void InitializeDefaultAttributes() const;

	/*
	 * 添加角色能力
	 */
	void AddCharacterAbilities() const;

	/**
	 * 溶解函数
	 */
	void Dissolve();

	/**
	 * 开始溶解时间轴
	 * @param DynamicMaterialInstance 动态实例化材质
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

	UFUNCTION(BlueprintImplementableEvent)
	void StartWeaponDissolveTimeLine(UMaterialInstanceDynamic* DynamicMaterialInstance);

	/**
	 * 网格溶解材质实例
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;

	/**
	 * 武器溶解材质实例
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

private:
	UPROPERTY(EditAnywhere, Category="Attributes")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;

	UPROPERTY(EditAnywhere, Category="Combat")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
