// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatBasicGameplay/Interaction/NipcatInteractAction.h"
#include "NipcatInteractAction_SetComponentActivated.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractAction_SetComponentActivated : public UNipcatInteractAction
{
	GENERATED_BODY()
	
public:
	bool Activated;

	virtual void ExecuteAction(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, const FText& FailedReason, UNipcatInteractCondition* FailedCondition) override;
};
