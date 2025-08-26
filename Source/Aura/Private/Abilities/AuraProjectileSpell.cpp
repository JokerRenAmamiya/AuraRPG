// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AuraProjectileSpell.h"

#include "AuraGameplayTags.h"
#include "Actor/AuraProjectile.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "Interaction/CombatInterface.h"
//#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo,
                                           const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	// const bool bIsServer = HasAuthority(&ActivationInfo);
	// if (!bIsServer)
	// 	return;
	//UKismetSystemLibrary::PrintString(this, FString("ActivateAbility C++"), true, true, FColor::Yellow, 3);
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	// 获取代表(玩家)演员信息
	AActor* Avatar = GetAvatarActorFromActorInfo();
	/**服务器权限判断*/
	if (const bool bIsServer = Avatar->HasAuthority(); !bIsServer)
		return;

	// 接口转换
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Avatar))
	{
		// 武器槽位置
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		FRotator Rotator = (ProjectileTargetLocation - SocketLocation).Rotation();
		Rotator.Pitch = 0.f;
		FTransform SpawnTransform;
		// 设置位置
		SpawnTransform.SetLocation(SocketLocation);
		// 设置旋转
		SpawnTransform.SetRotation(Rotator.Quaternion());
		// 打印火球生成位置
		UE_LOG(LogTemp, Warning, TEXT("UAuraProjectileSpell SpawnTransform位置: X=%.2f, Y=%.2f, Z=%.2f"),
		       SpawnTransform.GetLocation().X, SpawnTransform.GetLocation().Y, SpawnTransform.GetLocation().Z);
		// 异步延迟生成 Actor
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass, // 参数1：要生成的投射物类
			SpawnTransform, // 参数2：生成的位置和旋转
			GetOwningActorFromActorInfo(), // 参数3：所有者Actor 设置投射物的 “所有者”，通常是生成投射物的源头（如玩家控制器、技能持有者）
			Cast<APawn>(GetOwningActorFromActorInfo()), // 参数4：发起者Pawn 触发投射物生成的 Pawn（如玩家角色），通常与所有者相同（若所有者是 Pawn）
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn); // 参数5：碰撞处理方式

		// 火球伤害效果
		const UAbilitySystemComponent* SourceAsc =
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Avatar);
		const FGameplayEffectSpecHandle SpecHandle = SourceAsc->MakeOutgoingSpec(
			DamageEffectClass, GetAbilityLevel(), SourceAsc->MakeEffectContext());
		const FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		// 根据等级获取对应伤害曲线数值
		const float ScaleDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaleDamage);
		Projectile->DamageEffectSpecHandle = SpecHandle;
		// 完成生成（触发BeginPlay）
		Projectile->FinishSpawning(SpawnTransform);
	}
}
