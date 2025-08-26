// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	// 设置自己的碰撞类型
	Sphere->SetCollisionObjectType(ECC_Projectile);
	// 碰撞设置
	Sphere->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	// 碰撞体向下偏移 50 单位 临时操作，测试会不会打到哥布林 [测试结果可以打到，就是之前火球位置太高了]
	Sphere->SetRelativeLocation(FVector(0.0f, 0.0f, -50.0f));
	// 创建移动组件
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	// 设置初始速度和最大速度
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	// 重力缩放值
	ProjectileMovement->ProjectileGravityScale = 0.f;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	// 设置存在时间
	SetLifeSpan(LeftSpan);
	SetReplicateMovement(true);
	// 动态绑定重叠函数
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	// 生成声音循环组件
	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
}

void AAuraProjectile::Destroyed()
{
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	if (!bHit && !HasAuthority())
	{
		// 获取演员位置(不能用const常量，否则原地爆炸)
		FVector ActorLocation = GetActorLocation();
		//UE_LOG(LogTemp, Error, TEXT("ActorLocation: %f"), ActorLocation.X);
		// 在演员位置播放音效
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ActorLocation, FRotator::ZeroRotator);
		// 在演员位置生成粒子特效
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, ActorLocation);
	}
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                      const FHitResult& SweepResult)
{
	// 获取演员位置(不能用const常量，否则原地爆炸)
	FVector ActorLocation = GetActorLocation();
	//UE_LOG(LogTemp, Error, TEXT("ActorLocation: %f"), ActorLocation.X);
	// 在演员位置播放音效
	UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ActorLocation, FRotator::ZeroRotator);
	// 在演员位置生成粒子特效
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, ActorLocation);

	// 停止播放声音
	LoopingSoundComponent->Stop();
	if (HasAuthority())
	{
		// 应用效果
		if (UAbilitySystemComponent* TargetAsc = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			TargetAsc->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
		}
		Destroy();
	}
	else
	{
		bHit = true;
	}
}
