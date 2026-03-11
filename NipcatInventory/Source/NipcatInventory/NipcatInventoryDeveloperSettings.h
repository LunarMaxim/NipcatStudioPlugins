// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NipcatInventoryDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=NipcatInventory, DisplayName="NipcatInventory", DefaultConfig, BlueprintType)
class NIPCATINVENTORY_API UNipcatInventorySettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	/** Gets the settings container name for the settings, either Project or Editor */
	virtual FName GetContainerName() const override { return FName("Project"); }
	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override { return FName("NipcatStudio"); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="NipcatInventory|DeveloperSettings", DisplayName="GetNipcatInventorySettings")
	static const UNipcatInventorySettings* Get()
	{
		return GetDefault<UNipcatInventorySettings>();
	}
};
