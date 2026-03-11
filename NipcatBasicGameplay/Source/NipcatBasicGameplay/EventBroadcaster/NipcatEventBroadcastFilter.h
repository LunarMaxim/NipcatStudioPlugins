// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatEventBroadcastFilter.generated.h"

/**
 * 
 */
UCLASS(meta=(ShowWorldContextPin), Abstract, Blueprintable, BlueprintType, CollapseCategories)
class NIPCATBASICGAMEPLAY_API UNipcatEventBroadcastFilter : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, BlueprintNativeEvent)
	bool ShouldBroadcastEvent(AActor* SourceActor, AActor* TargetActor);
	
	virtual UWorld* GetWorld() const override;
};
