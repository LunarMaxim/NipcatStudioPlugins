// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractFunctionLibrary.h"

#include "NipcatInteractableInterface.h"
#include "NipcatInteractComponent.h"
#include "NipcatBasicGameplay/ActorTag/NipcatActorTagSubsystem.h"

void UNipcatInteractFunctionLibrary::DeactivateInteraction(AActor* Actor)
{
	if (IsValid(Actor) && Actor->GetClass()->ImplementsInterface(UNipcatInteractableInterface::StaticClass()))
	{
		Actor->GetWorld()->GetSubsystem<UNipcatActorTagSubsystem>()->UnregisterActor(Actor, FGameplayTagContainer(NipcatInteractTags::Actor_Interactable));
	}
}

void UNipcatInteractFunctionLibrary::ActivateInteraction(AActor* Actor)
{
	if (IsValid(Actor) && Actor->GetClass()->ImplementsInterface(UNipcatInteractableInterface::StaticClass()))
	{
		Actor->GetWorld()->GetSubsystem<UNipcatActorTagSubsystem>()->RegisterActor(Actor, FGameplayTagContainer(NipcatInteractTags::Actor_Interactable));
	}
}
