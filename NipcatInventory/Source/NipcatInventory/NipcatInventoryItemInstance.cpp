// Copyright Epic Games, Inc. All Rights Reserved.

#include "NipcatInventoryItemInstance.h"


#include "NipcatInventoryComponent.h"
#include "NipcatInventoryItemDefinition.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"


UNipcatInventoryItemInstance::UNipcatInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GUID = FGuid::NewGuid();
}

int32 UNipcatInventoryItemInstance::AddItemCount(int32 StackCount)
{
	if (!ItemDef.IsNull())
	{
		int32 OldCount = Count;
		if (!ItemDef.LoadSynchronous()->BaseData.bUnique)
		{
			Count += StackCount;
			NotifyItemCountChanged(StackCount);
			return StackCount;
		}
		else
		{
			if (Count <= 0 && StackCount > 0)
			{
				Count = 1;
				NotifyItemCountChanged(1);
				return 1;
			}
			else if (Count > 0 && StackCount < 0)
			{
				Count = 0;
				NotifyItemCountChanged(-1);
				return -1;
			}
		}
		if (UNipcatInventoryComponent* OwningInventory = GetInventoryComponent())
		{
			if (Count <= 0)
			{
				OwningInventory->OnItemCountEmpty(this);
			}
		}
	}
	return 0;
}

int32 UNipcatInventoryItemInstance::RemoveItemCount(int32 StackCount)
{
	if (StackCount < 0)
	{
		return -AddItemCount(-GetCount());
	}
	return -AddItemCount(-StackCount);
}

void UNipcatInventoryItemInstance::NotifyItemCountChanged(int32 DeltaCount)
{
	if (DeltaCount != 0)
	{
		for (const auto& Fragment : DynamicFragments)
		{
			Fragment->OnItemCountChanged(Count - DeltaCount, Count, DeltaCount);
		}
	}
}

void UNipcatInventoryItemInstance::NotifySaveGameLoaded()
{
	for (const auto& Fragment : DynamicFragments)
	{
		Fragment->OnSaveGameLoaded();
	}
}

int32 UNipcatInventoryItemInstance::GetCount() const
{
	return Count;
}

UNipcatInventoryComponent* UNipcatInventoryItemInstance::GetInventoryComponent()
{
	if (UNipcatInventoryComponent* InventoryComponent = Cast<UNipcatInventoryComponent>(GetOuter()))
	{
		return InventoryComponent;
	}
	
	return nullptr;
}

TArray<UNipcatInventoryItemFragment*> UNipcatInventoryItemInstance::GetAllFragments()
{
	if (!ItemDef.IsNull())
	{
		return ItemDef.LoadSynchronous()->Fragments;
	}
	return TArray<UNipcatInventoryItemFragment*>();
}

TArray<UNipcatInventoryItemFragment*> UNipcatInventoryItemInstance::GetAllDynamicFragments()
{
	return DynamicFragments;
}

#if WITH_EDITOR
void UNipcatInventoryItemInstance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	if (PropertyChangedEvent.Property->GetName() == GET_MEMBER_NAME_CHECKED(ThisClass, ItemDef))
	{
		if (!ItemDef.IsNull())
		{
			DynamicFragments.Empty();
			for (auto Fragment : ItemDef.LoadSynchronous()->Fragments)
			{
				if (Fragment)
				{
					auto DynamicFragment = DuplicateObject(Fragment, this);
					DynamicFragments.Emplace(DynamicFragment);
				}
			}
		}
	}
}
#endif

void UNipcatInventoryItemInstance::Serialize(FArchive& Ar)
{
	const int32 OldCount = Count;
	
	Super::Serialize(Ar);

	UNipcatBasicGameplayFunctionLibrary::SerializeObjectArray<UNipcatInventoryItemFragment>(Ar, DynamicFragments, this);
	if (Ar.IsSaveGame())
	{
		if (Ar.IsLoading())
		{
			InitDynamicFragments();
			NotifyItemCountChanged(Count - OldCount);
			NotifySaveGameLoaded();
		}
	}
}

void UNipcatInventoryItemInstance::Init(const UNipcatInventoryItemDefinition* InDef)
{
	ItemDef = const_cast<UNipcatInventoryItemDefinition*>(InDef);
	
	DynamicFragments.Empty();
	for (auto Fragment : ItemDef.LoadSynchronous()->Fragments)
	{
		if (Fragment)
		{
			auto DynamicFragment = DuplicateObject(Fragment, this);
			DynamicFragments.Emplace(DynamicFragment);
			DynamicFragment->Init();
			DynamicFragment->OnInstanceCreated();
		}
	}
}

void UNipcatInventoryItemInstance::InitDynamicFragments()
{
	for (const auto& DynamicFragment : DynamicFragments)
	{
		if (DynamicFragment)
		{
			DynamicFragment->Init();
			// DynamicFragment->OnInstanceCreated(); Item只在外部初始化一次
		}
	}
}

const UNipcatInventoryItemFragment* UNipcatInventoryItemInstance::FindFragmentByClass(TSubclassOf<UNipcatInventoryItemFragment> FragmentClass) const
{
	if ((!ItemDef.IsNull()) && (FragmentClass != nullptr))
	{
		return ItemDef.LoadSynchronous()->FindFragmentByClass(FragmentClass);
	}

	return nullptr;
}

const UNipcatInventoryItemFragment* UNipcatInventoryItemInstance::FindDynamicFragmentByClass(
	TSubclassOf<UNipcatInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (const UNipcatInventoryItemFragment* Fragment : DynamicFragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}
