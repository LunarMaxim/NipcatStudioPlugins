// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "PrimaryDataAssetWithWorldContext.generated.h"

/**
 * 
 */
UCLASS(meta=(ShowWorldContextPin), Abstract, Blueprintable, BlueprintType)
class NIPCATBASICGAMEPLAY_API UPrimaryDataAssetWithWorldContext : public UPrimaryDataAsset
{
	GENERATED_BODY()

	virtual UWorld* GetWorld() const override;
};
