// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractCondition.h"
#include "NipcatInteractCondition_OR.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractCondition_OR : public UNipcatInteractCondition
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	TArray<UNipcatInteractCondition*> Conditions;

	virtual bool CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
											UNipcatInteractableComponent* Interactable, FText& OutFailedReason) override;
};
