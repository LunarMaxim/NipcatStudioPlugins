// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractTypes.h"
#include "Engine/DataAsset.h"
#include "NipcatInteractData.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class NIPCATBASICGAMEPLAY_API UNipcatInteractData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ShowOnlyInnerProperties))
	FNipcatInteractConfig InteractConfig;
};
