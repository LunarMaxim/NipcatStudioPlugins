// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectComponent.h"
#include "GameplayTagContainer.h"
#include "NipcatActivateAbilityEffectComponent.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UNipcatActivateAbilityEffectComponent : public UGameplayEffectComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, DisplayName="能力标签", Category="ActivateAbility")
	uint8 bGamePlayEvent : 1;
	UPROPERTY(EditAnywhere, DisplayName="能力标签", Category="ActivateAbility", meta=(EditCondition="!bGamePlayEvent"))
	FGameplayTagContainer AbilityTags;
	UPROPERTY(EditAnywhere, DisplayName="事件标签", Category="ActivateAbility", meta=(EditCondition="bGamePlayEvent"))
	FGameplayTag EventTag;

public:
	virtual void OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const override;
};
