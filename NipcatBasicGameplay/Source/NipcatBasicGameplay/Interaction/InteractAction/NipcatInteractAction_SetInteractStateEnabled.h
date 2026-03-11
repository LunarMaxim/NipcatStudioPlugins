// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NipcatBasicGameplay/Interaction/NipcatInteractAction.h"
#include "NipcatInteractAction_SetInteractStateEnabled.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractAction_SetInteractStateEnabled : public UNipcatInteractAction
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories="InteractState"))
	TMap<FGameplayTag, bool> InteractStates;
	
	virtual void ExecuteAction(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, const FText& FailedReason, UNipcatInteractCondition* FailedCondition) override;
};
