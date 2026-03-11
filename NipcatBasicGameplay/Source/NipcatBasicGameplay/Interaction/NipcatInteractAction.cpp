// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractAction.h"

#include "NipcatInteractComponent.h"

void UNipcatInteractAction::ExecuteAction(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, const FText& FailedReason, UNipcatInteractCondition* FailedCondition)
{
	BP_ExecuteAction(InteractComponent, Interactable, FailedReason, FailedCondition);
}

UWorld* UNipcatInteractAction::GetWorld() const
{
	if (InstigatorContext)
	{
		return InstigatorContext->GetWorld();
	}
	if (InteractableContext.GetObject())
	{
		return InteractableContext.GetObject()->GetWorld();
	}
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& Context : WorldContexts)
	{
		if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game)) && (Context.World() != NULL))
		{
			return Context.World();
		}
	}
	return nullptr;
}
