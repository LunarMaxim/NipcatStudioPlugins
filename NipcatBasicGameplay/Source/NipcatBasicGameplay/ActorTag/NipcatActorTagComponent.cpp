// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatActorTagComponent.h"

#include "NipcatActorTagSubsystem.h"


UNipcatActorTagComponent::UNipcatActorTagComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void UNipcatActorTagComponent::Activate(bool bReset)
{
	Super::Activate(bReset);
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		GetWorld()->GetSubsystem<UNipcatActorTagSubsystem>()->RegisterActor(GetOwner(), ActorTags);
	}
}

void UNipcatActorTagComponent::Deactivate()
{
	if (GetWorld() && GetWorld()->IsGameWorld())
	{
		GetWorld()->GetSubsystem<UNipcatActorTagSubsystem>()->UnregisterActor(GetOwner(), ActorTags);
	}
	Super::Deactivate();
}


