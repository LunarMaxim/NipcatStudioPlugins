// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractCondition.h"
#include "NipcatInteractCondition_AND.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractCondition_AND : public UNipcatInteractCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	TArray<UNipcatInteractCondition*> Conditions;

	virtual bool CanInteract_Implementation(UNipcatInteractComponent* Instigator,
	                                        UNipcatInteractableComponent*
	                                        Interactable, FText& OutFailedReason) override;
};
