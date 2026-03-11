// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatChooserTypes.h"

#include "GameplayTagContainer.h"


bool FSingleGameplayTagContextProperty::GetValue(FChooserEvaluationContext& Context,
                                                 const FGameplayTagContainer*& OutResult) const
{
	const FGameplayTag* OutTag;
	if (Binding.GetValuePtr(Context, OutTag))
	{
		FGameplayTagContainer* ContainerPtr = const_cast<FGameplayTagContainer*>(&TagContainer);
		*ContainerPtr = OutTag->GetSingleTagContainer();
		const_cast<FGameplayTagContainer*&>(OutResult) = ContainerPtr;
		return true;
	}
	
	return false;
}

bool FParentGameplayTagContextProperty::GetValue(FChooserEvaluationContext& Context,
	const FGameplayTagContainer*& OutResult) const
{
	const FGameplayTag* OutTag;
	if (Binding.GetValuePtr(Context, OutTag))
	{
		FGameplayTagContainer* ContainerPtr = const_cast<FGameplayTagContainer*>(&TagContainer);
		*ContainerPtr = OutTag->GetGameplayTagParents();
		const_cast<FGameplayTagContainer*&>(OutResult) = ContainerPtr;
		return true;
	}
	
	return false;
}
