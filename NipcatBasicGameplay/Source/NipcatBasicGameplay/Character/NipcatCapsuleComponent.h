// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"

#include "NipcatCapsuleComponent.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatCapsuleComponent : public UCapsuleComponent
{
	GENERATED_BODY()

	UNipcatCapsuleComponent();
	
	virtual TArray<Chaos::FPhysicsObject*> GetAllPhysicsObjects() const override;
};
