// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "NipcatGameplayEffectComponent.generated.h"

struct FGameplayEffectRemovalInfo;
struct FActiveGameplayEffectHandle;
/**
 * 
 */
UCLASS(Abstract)
class NIPCATABILITYSYSTEM_API UNipcatGameplayEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()
	
public:
	virtual void PreGameplayEffectApply(FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const;

	virtual bool OnActiveGameplayEffectAdded(FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const override;

	virtual void OnGameplayEffectStackChange(FActiveGameplayEffectHandle Handle, int32 NewCount, int32 OldCount) const;

	virtual void OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const;

};
