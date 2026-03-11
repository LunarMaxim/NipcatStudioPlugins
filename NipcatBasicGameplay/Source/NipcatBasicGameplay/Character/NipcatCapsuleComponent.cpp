// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatCapsuleComponent.h"

#include "NipcatBasicGameplay/Interface/NipcatBasicGameplayCharacterInterface.h"

UNipcatCapsuleComponent::UNipcatCapsuleComponent()
{
	InitCapsuleSize(50.0f, 90.0f);

	CanCharacterStepUpOn = ECB_No;
	SetShouldUpdatePhysicsVolume(true);
	SetCanEverAffectNavigation(false);
	bDynamicObstacle = true;
	bUseSystemDefaultObstacleAreaClass = false;
}

TArray<Chaos::FPhysicsObject*> UNipcatCapsuleComponent::GetAllPhysicsObjects() const
{
	TArray<Chaos::FPhysicsObject*> Bodies = Super::GetAllPhysicsObjects();
	if (GetOwner()->Implements<UNipcatBasicGameplayCharacterInterface>())
	{
		auto AdditionalPhysicsObjects = INipcatBasicGameplayCharacterInterface::Execute_GetAdditionalColliders(GetOwner());
		for (const auto PrimitiveComponent : AdditionalPhysicsObjects)
		{
			if (PrimitiveComponent && !PrimitiveComponent->IsA<UNipcatCapsuleComponent>())
			{
				Bodies.Append(PrimitiveComponent->GetAllPhysicsObjects());
			}
		}
	}
	
	return Bodies;
}
