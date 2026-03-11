// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NipcatIndicatorManagerComponentOwnerInterface.generated.h"

class UNipcatIndicatorManagerComponent;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UNipcatIndicatorManagerComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATBASICGAMEPLAY_API INipcatIndicatorManagerComponentOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UNipcatIndicatorManagerComponent* GetNipcatIndicatorManagerComponent() const;
};
