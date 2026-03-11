// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition_DisabledStateTags.h"

#include "NipcatBasicGameplay/Interaction/NipcatInteractableComponent.h"

bool UNipcatInteractCondition_DisabledStateTags::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
                                                                            UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	if (TagRequirements.IsEmpty())
	{
		return true;
	}
	if (InteractComponent && Interactable)
	{
		if (const bool Succeed = TagRequirements.RequirementsMet(Interactable->DisabledStateTags))
		{
			return true;
		}
		OutFailedReason = GetFailedReason(Interactable);
	}
	return false;
}
