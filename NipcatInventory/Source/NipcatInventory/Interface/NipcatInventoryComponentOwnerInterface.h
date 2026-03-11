// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatInventoryComponentOwnerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable, BlueprintType)
class UNipcatInventoryComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATINVENTORY_API INipcatInventoryComponentOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="NipcatInventory")
	AActor* GetAvatarActor() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="NipcatInventory")
	UNipcatInventoryComponent* GetInventoryComponent() const;
};
