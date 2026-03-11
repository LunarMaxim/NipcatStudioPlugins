// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatActivateAbilityEffectComponent.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"

void UNipcatActivateAbilityEffectComponent::OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	Super::OnGameplayEffectApplied(ActiveGEContainer, GESpec, PredictionKey);
	if (ActiveGEContainer.Owner)
	{
		if (bGamePlayEvent)
		{
			if (EventTag.IsValid())
			{
				FGameplayEventData Payload;
				Payload.ContextHandle = GESpec.GetContext();
				ActiveGEContainer.Owner->HandleGameplayEvent(EventTag, &Payload);
			}
		}
		else
		{
			if (!AbilityTags.IsEmpty())
			{
				ActiveGEContainer.Owner->TryActivateAbilitiesByTag(AbilityTags);
			}
		}
	}
}
