// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatGameplayEffectComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

void UNipcatGameplayEffectComponent::PreGameplayEffectApply(FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec,
                                                            FPredictionKey& PredictionKey) const
{
}

bool UNipcatGameplayEffectComponent::OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer,
	FActiveGameplayEffect& ActiveGE) const
{
	// We don't allow prediction of expiration (on removed) effects
	if (ActiveGEContainer.IsNetAuthority())
	{
		// When this ActiveGE gets removed, so will our events so no need to unbind this.
		ActiveGE.EventSet.OnEffectRemoved.AddUObject(this, &UNipcatGameplayEffectComponent::OnActiveGameplayEffectRemoved, &ActiveGEContainer);
		ActiveGE.EventSet.OnStackChanged.AddUObject(this, &UNipcatGameplayEffectComponent::OnGameplayEffectStackChange);
	}
	
	return Super::OnActiveGameplayEffectAdded(ActiveGEContainer, ActiveGE);
}

void UNipcatGameplayEffectComponent::OnGameplayEffectStackChange(FActiveGameplayEffectHandle Handle,
	int32 NewCount, int32 OldCount) const
{
}

void UNipcatGameplayEffectComponent::OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo,
	FActiveGameplayEffectsContainer* ActiveGEContainer) const
{
}
