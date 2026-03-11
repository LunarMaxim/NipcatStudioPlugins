// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatInteractComponentOwnerInterface.generated.h"

class UNipcatInteractComponent;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UNipcatInteractComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATBASICGAMEPLAY_API INipcatInteractComponentOwnerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="NipcatInteract")
	UNipcatInteractComponent* GetNipcatInteractComponent() const;
};
