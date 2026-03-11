// Copyright Epic Games, Inc. All Rights Reserved.

#include "IndicatorLibrary.h"

#include "NipcatIndicatorManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IndicatorLibrary)

class AController;

UIndicatorLibrary::UIndicatorLibrary()
{
}

UNipcatIndicatorManagerComponent* UIndicatorLibrary::GetIndicatorManagerComponent(AController* Controller)
{
	return UNipcatIndicatorManagerComponent::GetComponent(Controller);
}

