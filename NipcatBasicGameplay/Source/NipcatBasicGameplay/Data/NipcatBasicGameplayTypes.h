// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatBasicGameplayTypes.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FNipcatCollisionChannelResponseMap
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ForeceInlineRow))
	TMap<TEnumAsByte<ECollisionChannel>, TEnumAsByte<ECollisionResponse>> Map;
};
