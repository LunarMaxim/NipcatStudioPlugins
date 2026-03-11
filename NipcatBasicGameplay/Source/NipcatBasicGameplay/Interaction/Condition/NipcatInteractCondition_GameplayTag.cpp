// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition_GameplayTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NipcatBasicGameplay/Interaction/NipcatInteractableComponent.h"

bool UNipcatInteractCondition_GameplayTag::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
                                                                      UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	if (TagRequirements.IsEmpty())
	{
		return true;
	}
	if (IsValid(InteractComponent))
    {
	    if (const auto AbilitySystemInterface = Cast<IAbilitySystemInterface>(CheckInstigatorTag ? InteractComponent->GetOwner() : Interactable->GetOwner()))
	    {
		    if (const auto ASC = AbilitySystemInterface->GetAbilitySystemComponent())
		    {
			    if (const bool Succeed = TagRequirements.RequirementsMet(ASC->GetOwnedGameplayTags()))
			    {
			    	return true;
			    }
				OutFailedReason = GetFailedReason(Interactable);
		    }
	    }
    }
    return false;
}
