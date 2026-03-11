// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition_AND.h"

bool UNipcatInteractCondition_AND::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
	UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	for (const auto Condition : Conditions)
	{
		if (!Condition->EvaluateCondition(InteractComponent, Interactable,OutFailedReason))
		{
			return false;
		}
	}
	return true;
}
