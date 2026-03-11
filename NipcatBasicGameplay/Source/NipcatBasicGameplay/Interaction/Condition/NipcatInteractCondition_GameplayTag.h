// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractCondition.h"
#include "GameplayEffectTypes.h"
#include "NipcatInteractCondition_GameplayTag.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractCondition_GameplayTag : public UNipcatInteractCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FGameplayTagRequirements TagRequirements;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CheckInstigatorTag = true;

	virtual bool CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
											UNipcatInteractableComponent* Interactable, FText& OutFailedReason) override;
};
