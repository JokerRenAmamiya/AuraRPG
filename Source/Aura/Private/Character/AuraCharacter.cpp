// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "UI/HUD/AuraHUD.h"
#include "Player/AuraPlayerState.h"
#include "Player/AuraPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

AAuraCharacter::AAuraCharacter()
{
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->bOrientRotationToMovement = true;
		Movement->RotationRate = FRotator(0.f, 400.f, 0.f);
		Movement->bConstrainToPlane = true;
		Movement->bSnapToPlaneAtStart = true;
	}

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	// init ability actor info for the server
	InitAbilityActorInfo();
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	// init ability actor info for the client
	InitAbilityActorInfo();
}

int32 AAuraCharacter::GetPlayerLevel()
{
	const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayerLevel();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	UAuraAbilitySystemComponent* Asc = AuraPlayerState->GetAbilitySystemComponent();
	check(Asc);
	Asc->InitAbilityActorInfo(AuraPlayerState, this);
	// 初始化属性集
	Asc->AbilityActorInfoSet();
	AbilitySystemComponent = Asc;
	AttributeSet = AuraPlayerState->GetAttributeSet();

	// 检测多人控制权
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController()))
	{
		AHUD* HUD = AuraPlayerController->GetHUD();
		if (HUD == nullptr)
			return;
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(HUD))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}

	// 初始化默认属性
	InitializeDefaultAttributes();
}
