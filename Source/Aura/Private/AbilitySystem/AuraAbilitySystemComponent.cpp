// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
	//const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>> StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		if (UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupInputTag);
			GiveAbility(AbilitySpec);
		}
		//GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	// 判断有效的标签
	if (!InputTag.IsValid())
		return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 过滤不匹配的标签
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputPressed(AbilitySpec);
			// 过滤激活的能力
			if (!AbilitySpec.IsActive())
			{
				// 尝试激活能力
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
		return;
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

/**
 * 
 * @param AbilitySystemComponent 能力组件
 * @param EffectSpec 效果
 * @param ActiveHandle 效果句柄
 */
void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec,
                                                                     FActiveGameplayEffectHandle ActiveHandle) const
{
	FGameplayTagContainer TagContainer;

	// 尝试多种方式获取标签
	if (EffectSpec.Def)
	{
		// 方法1：GetAllAssetTags (原方法)
		EffectSpec.GetAllAssetTags(TagContainer);

		// 方法2：如果AssetTags为空，尝试从GameplayEffect定义中获取
		if (TagContainer.IsEmpty())
		{
			// 从InheritableOwnedTagsContainer.CombinedTags获取tags
			TagContainer.AppendTags(EffectSpec.Def->GetGrantedTags());
		}

		// 方法3：如果还是为空，尝试从GrantedTags获取
		if (TagContainer.IsEmpty())
		{
			FGameplayTagContainer GrantedTags;
			EffectSpec.GetAllGrantedTags(GrantedTags);
			TagContainer.AppendTags(GrantedTags);
		}

		// 调试信息
		//UE_LOG(LogTemp, Warning, TEXT("Effect Name: %s"), *EffectSpec.Def->GetName());
		//UE_LOG(LogTemp, Warning, TEXT("Final TagContainer Count: %d"), TagContainer.Num());

		// 打印所有找到的标签
		// for (const FGameplayTag& Tag : TagContainer)
		// {
		// 	UE_LOG(LogTemp, Warning, TEXT("Found Tag: %s"), *Tag.ToString());
		// }
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("EffectSpec.Def is NULL!"));
	}

	// 只有当TagContainer不为空时才广播
	if (!TagContainer.IsEmpty())
	{
		OnEffectAssetTags.Broadcast(TagContainer);
	}
}
