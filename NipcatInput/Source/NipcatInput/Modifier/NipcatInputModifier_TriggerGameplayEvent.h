// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputModifiers.h"
#include "NipcatInput/Types/NipcatInputTypes.h"
#include "NipcatInputModifier_TriggerGameplayEvent.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, CollapseCategories, DisplayName="Trigger Gameplay Event")
class NIPCATINPUT_API UNipcatInputModifier_TriggerGameplayEvent : public UInputModifier
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FNipcatInputActionEvent TriggerEvents;
};
