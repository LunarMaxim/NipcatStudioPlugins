// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractCondition.h"
#include "NipcatInteractCondition_Distance.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractCondition_Distance : public UNipcatInteractCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double Distance = 200.f;

	virtual bool CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
											UNipcatInteractableComponent* Interactable, FText& OutFailedReason) override;
};
