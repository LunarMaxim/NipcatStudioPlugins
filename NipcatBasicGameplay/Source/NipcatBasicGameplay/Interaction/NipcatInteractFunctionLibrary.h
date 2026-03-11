// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatInteractFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatInteractFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="NipcatInteract", meta=(DefaultToSelf="Actor", ScriptMethod))
	static void DeactivateInteraction(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category="NipcatInteract", meta=(DefaultToSelf="Actor", ScriptMethod))
	static void ActivateInteraction(AActor* Actor);
	
};
