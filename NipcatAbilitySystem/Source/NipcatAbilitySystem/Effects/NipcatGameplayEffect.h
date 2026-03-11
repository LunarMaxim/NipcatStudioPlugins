// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "UObject/Object.h"
#include "NipcatGameplayEffect.generated.h"

class UGameplayEffectAdjustment;

USTRUCT(BlueprintType)
struct FGameplayEffectLevelMap
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow))
	TMap<TSubclassOf<UGameplayEffect>, float> Map;
};

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UNipcatGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category=EffectAdjustment)
	TArray<UGameplayEffectAdjustment*> SourceGrantEffectAdjustments;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Instanced, Category=EffectAdjustment)
	TArray<UGameplayEffectAdjustment*> TargetGrantEffectAdjustments;
	
	void PreGameplayEffectApply(FActiveGameplayEffectsContainer& ActiveGameplayEffects, const FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const;
};
