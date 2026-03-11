// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAssetUserData.h"
#include "Engine/DataAsset.h"
#include "NipcatAssetUserDataMap.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class NIPCATBASICGAMEPLAY_API UNipcatAssetUserDataMap : public UPrimaryDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow))
	TMap<TSoftObjectPtr<UObject>, FNipcatAssetUserDataArray> Map;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FNipcatAssetUserDataArray Fallback;
};
