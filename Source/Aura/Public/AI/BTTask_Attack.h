// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlueprintBase.h"
#include "BTTask_Attack.generated.h"

/**
* 自定义行为树节点
* 攻击节点
 */
UCLASS()
class AURA_API UBTTask_Attack : public UBTTask_BlueprintBase
{
	GENERATED_BODY()

	/**
	 * 重写执行攻击函数
	 * @param OwnerComp 拥有者组件
	 * @param NodeMemory 节点内存
	 * @return 执行类型
	 */
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};
