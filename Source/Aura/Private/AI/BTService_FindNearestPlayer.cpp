// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

// 行为树服务：每帧查找最近的玩家/敌人目标
void UBTService_FindNearestPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	// 调用父类的TickNode方法，执行基础的行为树节点更新逻辑
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	// 获取当前AI控制器拥有的Pawn（AI自身角色）
	APawn* OriginPawn = AIOwner->GetPawn();

	// 根据当前AI角色的标签，动态确定需要查找的目标标签
	// 如果自身是Player标签 → 目标改为Enemy
	// 如果自身不是Player标签 → 目标改为Player
	const FName TargetTag = OriginPawn->ActorHasTag(FName("Player")) ? FName("Enemy") : FName("Player");

	// 定义数组，用于存储查找到的所有带目标标签的Actor
	TArray<AActor*> ActorsWithTag;

	// 从世界中获取所有带有【目标标签】的Actor，并存储到数组中
	UGameplayStatics::GetAllActorsWithTag(OriginPawn, TargetTag, ActorsWithTag);
}
