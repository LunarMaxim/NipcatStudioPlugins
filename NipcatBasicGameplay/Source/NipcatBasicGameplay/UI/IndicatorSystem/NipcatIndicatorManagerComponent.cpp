// Copyright Epic Games, Inc. All Rights Reserved.

#include "NipcatIndicatorManagerComponent.h"

#include "IndicatorDescriptor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NipcatIndicatorManagerComponent)

UNipcatIndicatorManagerComponent::UNipcatIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bAutoRegister = true;
	bAutoActivate = true;
}

/*static*/ UNipcatIndicatorManagerComponent* UNipcatIndicatorManagerComponent::GetComponent(AController* Controller)
{
	if (Controller)
	{
		return Controller->FindComponentByClass<UNipcatIndicatorManagerComponent>();
	}

	return nullptr;
}

void UNipcatIndicatorManagerComponent::AddIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	IndicatorDescriptor->SetIndicatorManagerComponent(this);
	OnIndicatorAdded.Broadcast(IndicatorDescriptor);
	Indicators.Add(IndicatorDescriptor);
}

void UNipcatIndicatorManagerComponent::RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor)
{
	if (IndicatorDescriptor)
	{
		ensure(IndicatorDescriptor->GetIndicatorManagerComponent() == this);
	
		OnIndicatorRemoved.Broadcast(IndicatorDescriptor);
		Indicators.Remove(IndicatorDescriptor);
	}
}
