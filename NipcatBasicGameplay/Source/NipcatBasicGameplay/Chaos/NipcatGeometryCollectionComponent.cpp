// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatGeometryCollectionComponent.h"



void UNipcatGeometryCollectionComponent::SetCollisionResponseToChannel(ECollisionChannel Channel,
	ECollisionResponse NewResponse)
{
	Super::SetCollisionResponseToChannel(Channel, NewResponse);
	OnActorEnableCollisionChanged();
}

void UNipcatGeometryCollectionComponent::SetCollisionProfileName(FName InCollisionProfileName, bool bUpdateOverlaps)
{
	Super::SetCollisionProfileName(InCollisionProfileName, bUpdateOverlaps);
	OnActorEnableCollisionChanged();
}

void UNipcatGeometryCollectionComponent::SetCollisionResponseToChannels(const FCollisionResponseContainer& NewReponses)
{
	Super::SetCollisionResponseToChannels(NewReponses);
	OnActorEnableCollisionChanged();
}

void UNipcatGeometryCollectionComponent::SetCollisionResponseToAllChannels(ECollisionResponse NewResponse)
{
	Super::SetCollisionResponseToAllChannels(NewResponse);
	OnActorEnableCollisionChanged();
}

void UNipcatGeometryCollectionComponent::SetCollisionEnabled(ECollisionEnabled::Type NewType)
{
	Super::SetCollisionEnabled(NewType);
	OnActorEnableCollisionChanged();
}
