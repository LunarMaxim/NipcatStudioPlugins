// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition_Distance.h"

#include "NipcatBasicGameplay/Interaction/NipcatInteractableComponent.h"

bool UNipcatInteractCondition_Distance::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
                                                                   UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	if (IsValid(InteractComponent))
	{
		if (const bool bSucceed = FVector::DistSquared(InteractComponent->GetOwner()->GetActorLocation(), Interactable->GetInteractPointLocation()) < Distance * Distance)
		{
			return true;
		}
		OutFailedReason = GetFailedReason(Interactable);
	}
	return false;
}
