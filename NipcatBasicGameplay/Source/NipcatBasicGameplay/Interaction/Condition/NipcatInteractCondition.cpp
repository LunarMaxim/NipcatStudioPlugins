// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition.h"

#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"
#include "NipcatBasicGameplay/Interaction/NipcatInteractableComponent.h"

FText UNipcatInteractCondition::GetFailedReason(const UNipcatInteractableComponent* Interactable) const
{
	FText Result = DefaultFailedReason;
	FProperty* FoundProperty = nullptr;
	void* FoundPropertyAddress = nullptr;
	if (UNipcatBasicGameplayFunctionLibrary::Generic_GetPropertyByReflection(Interactable->GetOwner(), FName(TEXT("FailedReason")), FoundPropertyAddress, FoundProperty))
	{
		if (const auto FoundText = FoundProperty->ContainerPtrToValuePtr<FText>(Interactable->GetOwner()))
		{
			if (!FoundText->IsEmpty())
			{
				Result = *FoundText;
			}
		}
	}
	return Result;
}

bool UNipcatInteractCondition::EvaluateCondition(UNipcatInteractComponent* InteractComponent,
                                                 UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	InstigatorContext = InteractComponent;
	InteractableContext = Interactable;

	const bool Result = CanInteract(InteractComponent, Interactable, OutFailedReason);
	
	InstigatorContext = nullptr;
	InteractableContext = nullptr;
	
	return bInverseCondition ? !Result : Result;
}

bool UNipcatInteractCondition::EvaluateInteractConditions(TArray<UNipcatInteractCondition*> Conditions,
	UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, FText& OutFailedReason, int32& FailedConditionIndex)
{
	for (int32 i = 0; i < Conditions.Num(); ++i)
	{
		if (IsValid(Conditions[i]))
		{
			if (!Conditions[i]->EvaluateCondition(InteractComponent, Interactable, OutFailedReason))
			{
				FailedConditionIndex = i;
				return false;
			}
		}
	}
	return true;
}

bool UNipcatInteractCondition::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
                                                          UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	return true;
}

UWorld* UNipcatInteractCondition::GetWorld() const
{
	if (InstigatorContext)
	{
		return InstigatorContext->GetWorld();
	}
	if (InteractableContext)
	{
		return InteractableContext->GetWorld();
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
