// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatInventoryItemDefinition.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NipcatInventoryFunctionLibrary.generated.h"

/**
 * 
 */

//@TODO: Make into a subsystem instead?
UCLASS()
class NIPCATINVENTORY_API UNipcatInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory", meta=(DeterminesOutputType=FragmentClass))
	static const UNipcatInventoryItemFragment* FindItemDefinitionFragment(UNipcatInventoryItemDefinition* ItemDef, TSubclassOf<UNipcatInventoryItemFragment> FragmentClass);


	template <typename ResultClass>
	static const ResultClass* FindFragmentByClass(TSubclassOf<UNipcatInventoryItemDefinition> ItemDef)
	{
		if (ItemDef)
		{
			return (ResultClass*)FindItemDefinitionFragment(ItemDef.GetDefaultObject(), ResultClass::StaticClass());
		}
		return nullptr;
	}
	
	template <typename ResultClass>
	static const ResultClass* FindFragmentByClass(UNipcatInventoryItemDefinition* ItemDef)
	{
		if (ItemDef)
		{
			return (ResultClass*)FindItemDefinitionFragment(ItemDef, ResultClass::StaticClass());
		}
		return nullptr;
	}
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory", meta=(DeterminesOutputType=FragmentClass))
	static const UNipcatInventoryItemFragment* FindItemInstanceDynamicFragment(UNipcatInventoryItemInstance* ItemInstance, TSubclassOf<UNipcatInventoryItemFragment> FragmentClass);


	template <typename ResultClass>
	static const ResultClass* FindDynamicFragmentByClass(UNipcatInventoryItemInstance* ItemInstance)
	{
		if (ItemInstance)
		{
			return (ResultClass*)FindItemInstanceDynamicFragment(ItemInstance, ResultClass::StaticClass());
		}
		return nullptr;
	}
	
};
