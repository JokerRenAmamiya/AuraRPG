// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "OverlayWidgetController.generated.h"

class UAuraUserWidget;
struct FOnAttributeChangeData;

/*
 * UI接口详情结构体
 */
USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText MessageText = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

/*
 * 动态多播委托
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetDataTableSignature, FUIWidgetRow, Row);


/**
 * 主UI控制器
 * 可在蓝图调用
 */
UCLASS(BlueprintType, Blueprintable)
class AURA_API UOverlayWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	/**
	 * 广播初始化值
	 */
	virtual void BroadcastInitialValues() override;
	/**
	 * 属性回调绑定
	 */
	virtual void BindCallbacksToDependencies() override;

	// BlueprintAssignable只能绑定多播委托 单播委托会编译报错
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Health")
	FOnAttributeChangedSignature OnHealChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|MaxHealth")
	FOnAttributeChangedSignature OnMaxHealChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Mana")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|MaxMana")
	FOnAttributeChangedSignature OnMaxManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes|Message")
	FMessageWidgetDataTableSignature OnMessageWidgetDataTableSignature;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	/**
	 * 获取表数据模板函数
	 * 根据标签获取一行表数据
	 * @tparam T 泛型名称
	 * @param DataTable 表数据
	 * @param Tag 标签
	 * @return 表数据
	 */
	template <typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	T* Row = DataTable->FindRow<T>(Tag.GetTagName(), TEXT(""));
	return Row;
}
