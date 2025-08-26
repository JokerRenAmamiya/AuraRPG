// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AuraGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "UI/Widget/DamageTextComponent.h"


AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true;
	ThisActor = nullptr;
	LastActor = nullptr;

	CachedDestination = FVector::ZeroVector;
	FlowTime = 0.f;
	ShortPressThreshold = 0.5f;
	bAutoRunning = false;
	bTargeting = false;
	AutoRunAcceptanceRadius = 50.f;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(const float DamageAmount, ACharacter* TargetCharacter)
{
	// 检查生命周期
	if (IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		// 创建伤害组件
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		// 注册生命周期
		DamageText->RegisterComponent();
		// 附着到目标角色节点上
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(),
		                              FAttachmentTransformRules::KeepRelativeTransform);
		// 子节点分离
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		// 设置数值
		DamageText->SetDamageText(DamageAmount);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)
		return;
	if (APawn* ControllerPawn = GetPawn())
	{
		const FVector ActorLocation = ControllerPawn->GetActorLocation();
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(
			ActorLocation, ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(
			LocationOnSpline, ESplineCoordinateSpace::World);
		ControllerPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit)
		return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	// 指针悬停情况判断
	if (LastActor != ThisActor)
	{
		if (LastActor)
		{
			LastActor->UnHighlightActor();
		}
		if (ThisActor)
		{
			ThisActor->HighlightActor();
		}
	}
}

void AAuraPlayerController::AbilityInputPressed(const FGameplayTag InputTag)
{
	// 左键输入检测
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;
	}
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Red, *InputTag.ToString());
}

void AAuraPlayerController::AbilityInputReleased(const FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraAsc())
		{
			GetAuraAsc()->AbilityInputTagReleased(InputTag);
		}
		return;
	}
	if (GetAuraAsc())
	{
		GetAuraAsc()->AbilityInputTagReleased(InputTag);
	}
	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControllerPawn = GetPawn();
		if (FlowTime <= ShortPressThreshold && ControllerPawn)
		{
			// 寻路路径
			const UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(
				this, ControllerPawn->GetActorLocation(),
				CachedDestination);
			if (NavPath)
			{
				Spline->ClearSplinePoints();
				TArray<FVector> PathPoints = NavPath->PathPoints;
				// 如果没有元素，则不继续执行
				if (PathPoints.IsEmpty())
					return;
				// 遍历找到的路径点
				for (const FVector& PointLoc : PathPoints)
				{
					// 曲线添加路径点
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					// 绘制路径点
					DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8.f, FColor::Green, false, 5.f);
				}
				CachedDestination = PathPoints[PathPoints.Num() - 1];
				bAutoRunning = true;
			}
		}
		FlowTime = 0.f;
		bTargeting = false;
	}
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Blue, *InputTag.ToString());
}

void AAuraPlayerController::AbilityInputHeld(const FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetAuraAsc())
		{
			GetAuraAsc()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	// 选中目标或按下Shift键
	if (bTargeting || bShiftKeyDown)
	{
		if (GetAuraAsc())
		{
			GetAuraAsc()->AbilityInputTagHeld(InputTag);
		}
	}
	else
	{
		FlowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit)
		{
			// 缓存点击位置
			CachedDestination = CursorHit.ImpactPoint;
		}

		if (APawn* ControllerPawn = GetPawn())
		{
			// 获取距离向量
			FVector WorldDirection = CachedDestination - ControllerPawn->GetActorLocation();
			// 标准化
			WorldDirection = WorldDirection.GetSafeNormal();
			// 添加移动输入
			ControllerPawn->AddMovementInput(WorldDirection);
		}
	}
	//GEngine->AddOnScreenDebugMessage(1, 3.f, FColor::Green, *InputTag.ToString());
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetAuraAsc()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(
			UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}


void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	// 断言检查
	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(
		GetLocalPlayer());
	// 检测多人控制权
	if (Subsystem)
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 增强输入绑定
	//UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	//EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	// 自定义输入绑定
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftRelease);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputPressed,
	                                       &ThisClass::AbilityInputReleased, &ThisClass::AbilityInputHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	// 控制玩家移动
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	// 获取输入轴
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// 添加移动
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}
