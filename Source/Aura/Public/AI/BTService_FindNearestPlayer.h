// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Services/BTService_BlueprintBase.h"
#include "BTService_FindNearestPlayer.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UBTService_FindNearestPlayer : public UBTService_BlueprintBase
{
	GENERATED_BODY()

	/**
	 * 每帧执行行为树节点
	 * @param OwnerComp 组件拥有者
	 * @param NodeMemory 节点内存
	 * @param DeltaSeconds 秒数变化量
	 */
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
