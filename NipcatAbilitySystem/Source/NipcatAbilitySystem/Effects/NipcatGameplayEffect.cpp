// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatGameplayEffect.h"
#include "GameplayEffectComponent.h"
#include "NipcatGameplayEffectComponent.h"

void UNipcatGameplayEffect::PreGameplayEffectApply(FActiveGameplayEffectsContainer& ActiveGameplayEffects, const FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	for (const UGameplayEffectComponent* GEComponent : GEComponents)
	{
		if (IsValid(GEComponent))
		{
			if (const UNipcatGameplayEffectComponent* NipcatGEComponent = Cast<UNipcatGameplayEffectComponent>(GEComponent))
			{
				NipcatGEComponent->PreGameplayEffectApply(ActiveGameplayEffects, GESpec, PredictionKey);
			}
		}
	}
}
