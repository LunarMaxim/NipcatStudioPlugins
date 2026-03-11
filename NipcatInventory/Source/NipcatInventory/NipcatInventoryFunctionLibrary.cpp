// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInventoryFunctionLibrary.h"

#include "NipcatInventoryItemInstance.h"

const UNipcatInventoryItemFragment* UNipcatInventoryFunctionLibrary::FindItemDefinitionFragment(UNipcatInventoryItemDefinition* ItemDef, TSubclassOf<UNipcatInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return ItemDef->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}

const UNipcatInventoryItemFragment* UNipcatInventoryFunctionLibrary::FindItemInstanceDynamicFragment(
	UNipcatInventoryItemInstance* ItemInstance, TSubclassOf<UNipcatInventoryItemFragment> FragmentClass)
{
	if (ItemInstance && FragmentClass)
	{
		return ItemInstance->FindDynamicFragmentByClass(FragmentClass);
	}
	return nullptr;
}

