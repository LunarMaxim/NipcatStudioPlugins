// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatGameplayEffectComponent_BlueprintBase.h"

bool UNipcatGameplayEffectComponent_BlueprintBase::CanGameplayEffectApply_Implementation(
	UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpecHandle& GESpec) const
{
	return true;
}

bool UNipcatGameplayEffectComponent_BlueprintBase::OnActiveGameplayEffectAdded_Implementation(
	UAbilitySystemComponent* AbilitySystemComponent, const FActiveGameplayEffect& ActiveGE) const
{
	return true;
}

UWorld* UNipcatGameplayEffectComponent_BlueprintBase::GetWorld() const
{
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& Context : WorldContexts)
	{
		if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game)) && (Context.World() != NULL))
		{
			return Context.World();
		}
	}
	return nullptr;
}
