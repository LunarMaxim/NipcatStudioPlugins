// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractCondition.h"
#include "NipcatInteractCondition_LineTrace.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractCondition_LineTrace : public UNipcatInteractCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TEnumAsByte<ETraceTypeQuery> TraceChannel = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UseCameraLocation = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IgnoreSelf = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool DrawDebug;
	
	virtual bool CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
											UNipcatInteractableComponent* Interactable, FText& OutFailedReason) override;
};
