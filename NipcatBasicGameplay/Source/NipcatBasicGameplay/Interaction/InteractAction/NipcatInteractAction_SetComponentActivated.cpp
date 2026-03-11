// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractAction_SetComponentActivated.h"

#include "NipcatBasicGameplay/Interaction/NipcatInteractableComponent.h"

void UNipcatInteractAction_SetComponentActivated::ExecuteAction(UNipcatInteractComponent* InteractComponent,
                                                                UNipcatInteractableComponent* Interactable, const FText& FailedReason, UNipcatInteractCondition* FailedCondition)
{
	if (Interactable)
	{
		Interactable->SetActive(Activated);
		Interactable->Activated = Activated;
	}
}
