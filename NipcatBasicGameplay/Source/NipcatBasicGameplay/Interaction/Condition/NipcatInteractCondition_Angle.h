// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractCondition.h"
#include "NipcatInteractCondition_Angle.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractCondition_Angle : public UNipcatInteractCondition
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(UIMin = -180, UIMax = 180, ClampMin = -180, ClampMax = 180))
	double Direction = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(UIMin = 0, UIMax = 180, ClampMin = 0, ClampMax = 180))
	double HalfAngle = 90.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool UseCameraRotation;

	virtual bool CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
	                                        UNipcatInteractableComponent* Interactable, FText& OutFailedReason) override;
};
