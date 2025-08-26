// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 敌人接口
 */
class AURA_API IEnemyInterface
{
	GENERATED_BODY()

public:
	// 高亮演员 纯虚函数
	virtual void HighlightActor() = 0;
	// 取消高亮演员 纯虚函数
	virtual void UnHighlightActor() = 0;
};
