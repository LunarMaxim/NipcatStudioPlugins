// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInteractTypes.h"
#include "UObject/Interface.h"
#include "NipcatInteractableInterface.generated.h"


class UNipcatInteractableComponent;
struct FNipcatInteractableState;
class UNipcatInteractComponent;
class UNipcatIndicatorComponent;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UNipcatInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATBASICGAMEPLAY_API INipcatInteractableInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="NipcatInteract")
	TArray<UNipcatInteractableComponent*> GetNipcatInteractableComponents() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="NipcatInteract")
	UNipcatIndicatorComponent* GetInteractIndicator(UNipcatInteractableComponent* InteractableComponent) const;
	
};
