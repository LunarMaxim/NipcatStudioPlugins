// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractCondition.h"
#include "NipcatInteractCondition_MovementMode.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractCondition_MovementMode : public UNipcatInteractCondition
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<EMovementMode> RequiredMovementMode = MOVE_Walking;

	virtual bool CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
											UNipcatInteractableComponent* Interactable, FText& OutFailedReason) override;
};
