// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetUserData.h"
#include "NipcatAssetUserData.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, Abstract, CollapseCategories)
class NIPCATBASICGAMEPLAY_API UNipcatAssetUserData : public UAssetUserData
{
	GENERATED_BODY()
};

USTRUCT(BlueprintType)
struct NIPCATBASICGAMEPLAY_API FNipcatAssetUserDataArray
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	TArray<UNipcatAssetUserData*> Array;
};
