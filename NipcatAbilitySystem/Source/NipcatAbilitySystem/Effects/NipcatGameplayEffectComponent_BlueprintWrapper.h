// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatGameplayEffectComponent.h"
#include "NipcatGameplayEffectComponent_BlueprintWrapper.generated.h"

struct FGameplayEffectRemovalInfo;
class UNipcatGameplayEffectComponent_BlueprintBase;
/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UNipcatGameplayEffectComponent_BlueprintWrapper : public UNipcatGameplayEffectComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UNipcatGameplayEffectComponent_BlueprintBase* BlueprintEffectComponent;
	
	virtual void PreGameplayEffectApply(FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;
	
	virtual bool CanGameplayEffectApply(const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const override;
	
	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const override;

	virtual void OnGameplayEffectExecuted(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;

	virtual void OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;

	virtual void OnGameplayEffectStackChange(FActiveGameplayEffectHandle Handle, int32 NewCount, int32 OldCount) const override;

	virtual void OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const override;

};
