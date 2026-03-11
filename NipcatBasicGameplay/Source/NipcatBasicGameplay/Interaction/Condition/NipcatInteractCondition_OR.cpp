// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition_OR.h"

bool UNipcatInteractCondition_OR::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
	UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	for (const auto Condition : Conditions)
	{
		if (Condition->EvaluateCondition(InteractComponent, Interactable,OutFailedReason))
		{
			return true;
		}
	}
	return false;
}
