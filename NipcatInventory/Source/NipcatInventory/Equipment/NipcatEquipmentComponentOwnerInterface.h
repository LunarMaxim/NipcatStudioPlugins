// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatEquipmentComponentOwnerInterface.generated.h"

class UNipcatEquipmentComponent;
// This class does not need to be modified.
UINTERFACE(BlueprintType, MinimalAPI)
class UNipcatEquipmentComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATINVENTORY_API INipcatEquipmentComponentOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Nipcat Inventory|Equipment")
	UNipcatEquipmentComponent* GetNipcatEquipmentComponent();
};
