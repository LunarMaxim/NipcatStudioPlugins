// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition_InteractState.h"

bool UNipcatInteractCondition_InteractState::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
	UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	if (InteractStateTagRequirement.IsEmpty())
	{
		return true;
	}
	if (InteractComponent && Interactable)
	{
		if (const bool Succeed = InteractStateTagRequirement.RequirementsMet(FGameplayTagContainer(InteractComponent->GetInteractState(Interactable).StateTag)))
		{
			return true;
		}
		
		OutFailedReason = GetFailedReason(Interactable);
	}
	return false;
}
