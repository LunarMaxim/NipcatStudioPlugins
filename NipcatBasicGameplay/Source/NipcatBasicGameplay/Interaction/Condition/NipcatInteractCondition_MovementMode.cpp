// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition_MovementMode.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

bool UNipcatInteractCondition_MovementMode::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
                                                                       UNipcatInteractableComponent*  Interactable, FText& OutFailedReason)
{
	if (IsValid(InteractComponent))
	{
		if (const auto Character = Cast<ACharacter>(InteractComponent->GetOwner()))
		{
			if (const bool Succeed = Character->GetCharacterMovement()->MovementMode == RequiredMovementMode)
			{
				return true;
			}
			OutFailedReason = GetFailedReason(Interactable);
		}
	}
	return false;
}
