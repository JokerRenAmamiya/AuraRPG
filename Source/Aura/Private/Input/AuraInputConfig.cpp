// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,
                                                                   bool bLogNotFound) const
{
	// for (const FAuraInputAction& Action : AbilityInputActions)
	// {
	// 	if (Action.InputAction && InputTag.MatchesTagExact(Action.GameplayTag))
	// 	{
	// 		return Action.InputAction;
	// 	}
	// }
	// C++17新语法 结构化绑定(Structured Binding) 将容器中的元素拆解为多个变量。
	for (const auto& [InputAction, GameplayTag] : AbilityInputActions)
	{
		// 输入Action存在，且Tag对比一致
		if (InputAction && InputTag.MatchesTagExact(GameplayTag))
		{
			return InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]"),
		       *InputTag.ToString(), *GetNameSafe(this));
	}
	return nullptr;
}
