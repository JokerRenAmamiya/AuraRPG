// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "AttributeInfo.generated.h"

/*属性信息结构体*/
USTRUCT(BlueprintType)
struct FAuraAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly);
	FGameplayTag AttributeTag = FGameplayTag();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly);
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly);
	FText AttributeDescription = FText();

	UPROPERTY(BlueprintReadOnly);
	float AttributeValue = 0.f;
};

/**
 * 
 */
UCLASS()
class AURA_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	/*根据属性标签查找属性*/
	FAuraAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;

	/*属性信息数组*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly);
	TArray<FAuraAttributeInfo> AttributeInformation;
};
