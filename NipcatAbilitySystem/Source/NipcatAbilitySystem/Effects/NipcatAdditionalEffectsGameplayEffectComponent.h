// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "NipcatGameplayEffectComponent.h"
#include "NipcatAdditionalEffectsGameplayEffectComponent.generated.h"

/**
 * 
 */
UCLASS(CollapseCategories)
class NIPCATABILITYSYSTEM_API UNipcatAdditionalEffectsGameplayEffectComponent : public UNipcatGameplayEffectComponent
{
	GENERATED_BODY()
	
public:

	virtual void PreGameplayEffectApply(FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;
	
	/**
	 * Called when a Gameplay Effect is applied.  This executes the OnApplication Gameplay Effects.
	 */
	virtual void OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;

	
#if WITH_EDITOR
	/**
	 * There are some fields that are incompatible with one another, so let's catch them during configuration phase. 
	 */
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif // WITH_EDITOR


protected:
	/**
	 * Whenever the ActiveGE gets removed, we want to apply the configured OnComplete GameplayEffects
	 */
	void OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const override;

	void ApplyExtraEffectToTarget(TArray<FConditionalGameplayEffect> Effects, FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec,
							  FPredictionKey& PredictionKey, bool bApplyToSelf = false) const;

public:
	/**
	 * This will copy all of the data (e.g. SetByCallerMagnitudes) from the GESpec that Applied this GameplayEffect to the new OnApplicationGameplayEffect Specs.
	 * One would think this is normally desirable (and how OnComplete works by default), but we default to false here for backwards compatability.
	 */
	UPROPERTY(EditDefaultsOnly, Category = OnApplication)
	bool bOnApplicationCopyDataFromOriginalSpec = false;
	
	UPROPERTY(EditDefaultsOnly, Category = OnApplication)
	TArray<FConditionalGameplayEffect> PreApplicationGameplayEffectsToSelf;

	UPROPERTY(EditDefaultsOnly, Category = OnApplication)
	TArray<FConditionalGameplayEffect> PreApplicationGameplayEffects;

	/** Other gameplay effects that will be applied to self if the owning effect applies */
	UPROPERTY(EditDefaultsOnly, Category = OnApplication)
	TArray<FConditionalGameplayEffect> OnApplicationGameplayEffectsToSelf;

	/** Other gameplay effects that will be applied to the target of this effect if the owning effect applies */
	UPROPERTY(EditDefaultsOnly, Category = OnApplication)
	TArray<FConditionalGameplayEffect> OnApplicationGameplayEffects;

	/** Effects to apply when this effect completes, regardless of how it ends */
	UPROPERTY(EditDefaultsOnly, Category = OnComplete)
	TArray<TSubclassOf<UGameplayEffect>> OnCompleteAlways;

	/** Effects to apply when this effect expires naturally via its duration */
	UPROPERTY(EditDefaultsOnly, Category = OnComplete)
	TArray<TSubclassOf<UGameplayEffect>> OnCompleteNormal;

	/** Effects to apply when this effect is made to expire prematurely (e.g. via a forced removal, clear tags, etc.) */
	UPROPERTY(EditDefaultsOnly, Category = OnComplete)
	TArray<TSubclassOf<UGameplayEffect>> OnCompletePrematurely;
};
