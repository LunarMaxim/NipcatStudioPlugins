// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractAction_SetInteractStateEnabled.h"

#include "NipcatBasicGameplay/Interaction/NipcatInteractableComponent.h"
#include "NipcatBasicGameplay/Interaction/NipcatInteractComponent.h"


void UNipcatInteractAction_SetInteractStateEnabled::ExecuteAction(UNipcatInteractComponent* InteractComponent,
                                                                UNipcatInteractableComponent* Interactable, const FText& FailedReason, UNipcatInteractCondition* FailedCondition)
{
	if (Interactable)
	{
		for (auto [Tag, Enabled] : InteractStates)
		{
			if (Enabled)
			{
				Interactable->EnableState(FGameplayTagContainer(Tag));
			}
			else
			{
				Interactable->DisableState(FGameplayTagContainer(Tag));
			}
		}
	}
}
