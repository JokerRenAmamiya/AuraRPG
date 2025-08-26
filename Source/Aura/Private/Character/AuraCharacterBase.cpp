#include "Character/AuraCharacterBase.h"

#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = true;

	// 胶囊体获取
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	// 网格获取
	USkeletalMeshComponent* MeshComp = GetMesh();
	// 胶囊体碰撞通道设置
	CapsuleComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	// 胶囊体不生成重叠事件
	CapsuleComp->SetGenerateOverlapEvents(false);
	// 网格碰撞通道设置
	MeshComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	MeshComp->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	// 网格生成重叠事件
	MeshComp->SetGenerateOverlapEvents(true);

	// 创建武器
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetupAttachment(MeshComp, FName("WeaponHandSocket"));
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die()
{
	// 武器分离逻辑
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	// 多播句柄处理
	MulticastHandleDeath();
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	// 开启物理效果
	Weapon->SetSimulatePhysics(true);
	// 开启重力效果让武器掉落
	Weapon->SetEnableGravity(true);
	// 设置武器碰撞
	Weapon->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);

	// 胶囊体获取
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	// 网格获取
	USkeletalMeshComponent* MeshComp = GetMesh();
	MeshComp->SetSimulatePhysics(true);
	MeshComp->SetEnableGravity(true);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::Type::PhysicsOnly);
	MeshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

	// 溶解
	Dissolve();
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(const TSubclassOf<UGameplayEffect>& GameplayEffectClass,
                                           const float Level) const
{
	UAbilitySystemComponent* Asc = GetAbilitySystemComponent();
	check(Asc);
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = Asc->MakeEffectContext();
	// 给自己添加效果
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = Asc->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	// 应用初始化属性效果到对象
	Asc->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), Asc);
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	UAbilitySystemComponent* Asc = GetAbilitySystemComponent();
	// 判断服务器权限
	if (!HasAuthority())
		return;
	if (UAuraAbilitySystemComponent* AuraAsc = Cast<UAuraAbilitySystemComponent>(Asc))
	{
		AuraAsc->AddCharacterAbilities(StartupAbilities);
	}
}

void AAuraCharacterBase::Dissolve()
{
	// 判断有效的网格溶解材质
	if (IsValid(DissolveMaterialInstance))
	{
		USkeletalMeshComponent* MeshComp = GetMesh();
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		// 设置材质和下标
		MeshComp->SetMaterial(0, DynamicMatInst);
		StartDissolveTimeLine(DynamicMatInst);
	}

	// 判断有效的武器溶解材质
	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(
			WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);
		StartWeaponDissolveTimeLine(DynamicMatInst);
	}
}
