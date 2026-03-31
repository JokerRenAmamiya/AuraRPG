// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BTService_FindNearestPlayer.h"

#include "AIController.h"
#include "BehaviorTree/BTFunctionLibrary.h"
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

	// ===================== 寻找最近目标逻辑 =====================
	// 初始化最近距离为浮点数最大值（确保任何有效距离都会更小）
	float ClosetDistance = TNumericLimits<float>::Max();

	// 初始化最近目标Actor为空
	AActor* ClosetActor = nullptr;

	// 遍历所有找到的目标Actor
	for (AActor* Actor : ActorsWithTag)
	{
		// 安全校验：确保当前遍历的Actor和AI自身Pawn都有效（防止空指针崩溃）
		if (IsValid(Actor) && IsValid(OriginPawn))
		{
			// 计算AI自身与当前目标Actor之间的距离
			const float Distance = OriginPawn->GetDistanceTo(Actor);

			// 如果当前距离比记录的最近距离更小，说明找到了更近的目标
			if (Distance < ClosetDistance)
			{
				// 更新最近距离
				ClosetDistance = Distance;
				// 更新最近目标Actor
				ClosetActor = Actor;
			}
		}
	}

	// ===================== 更新行为树黑板 =====================
	// 将找到的最近目标Actor写入黑板（供行为树其他节点使用，如追击、攻击）
	UBTFunctionLibrary::SetBlackboardValueAsObject(this, TargetToFollowSelector, ClosetActor);

	// 将最近目标的距离写入黑板（供行为树根据距离做决策，如远程/近战切换）
	UBTFunctionLibrary::SetBlackboardValueAsFloat(this, DistanceToFollowSelector, ClosetDistance);
}
