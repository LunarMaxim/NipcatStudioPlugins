// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatEquipmentComponent.h"

#include "NipcatInventoryItemFragment_Equipment.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"
#include "NipcatInventory/NipcatInventoryComponent.h"
#include "NipcatInventory/NipcatInventoryItemInstance.h"
#include "NipcatInventory/Interface/NipcatInventoryComponentOwnerInterface.h"


UNipcatEquipmentSlotGroup* UNipcatEquipmentComponent::FindSlotGroup(FGameplayTag SlotGroupTag)
{
	if (const auto Result = SlotGroups.Find(SlotGroupTag))
	{
		return *Result;
	}
	return nullptr;
}

void UNipcatEquipmentComponent::AddItemToSlot(const FGameplayTag& SlotTag, UNipcatInventoryItemInstance* Item, int32 SlotIndex)
{
	if (const auto SlotGroup = FindSlotGroup(SlotTag))
	{
		if (SlotGroup->IsValidSlotIndex(SlotIndex))
		{
			const int32 ExistingIndex = SlotGroup->FindItem(Item);
			if (ExistingIndex != INDEX_NONE)
			{
				SlotGroup->Slots[ExistingIndex].Item = nullptr;
				OnSlotUpdated(SlotTag, SlotGroup, ExistingIndex, nullptr);
			}
			if (UNipcatInventoryItemInstance* ItemToSwap = SlotGroup->Slots[SlotIndex].Item)
			{
				if (ExistingIndex != INDEX_NONE)
				{
					SlotGroup->Slots[ExistingIndex].Item = ItemToSwap;
					OnSlotUpdated(SlotTag, SlotGroup, ExistingIndex, ItemToSwap);
				}
				SlotGroup->Slots[SlotIndex].Item = nullptr;
				OnSlotUpdated(SlotTag, SlotGroup, SlotIndex, nullptr);

				if (ExistingIndex == INDEX_NONE)
				{
					if (const auto Fragment = ItemToSwap->FindDynamicFragmentByClass<UNipcatInventoryItemFragment_Equipment>())
					{
						Fragment->OnEquipmentRemovedFromSlot();
					}
				}
			}
			
			SlotGroup->Slots[SlotIndex].Item = Item;
			OnSlotUpdated(SlotTag, SlotGroup, SlotIndex, Item);
			
			if (ExistingIndex == INDEX_NONE)
			{
				if (const auto Fragment = Item->FindDynamicFragmentByClass<UNipcatInventoryItemFragment_Equipment>())
				{
					Fragment->OnEquipmentAddedToSlot();
				}
			}
		}
	}
}

void UNipcatEquipmentComponent::RemoveItemAtSlot(const FGameplayTag& SlotTag, int32 SlotIndex)
{
	if (const auto SlotGroup = FindSlotGroup(SlotTag))
	{
		if (SlotGroup->IsValidSlotIndex(SlotIndex))
		{
			if (SlotGroup->Slots.IsValidIndex(SlotIndex))
			{
				if (const auto Item = SlotGroup->Slots[SlotIndex].Item; IsValid(Item.Get()))
				{
					SlotGroup->Slots[SlotIndex].Item = nullptr;
					OnSlotUpdated(SlotTag, SlotGroup, SlotIndex, nullptr);
					if (const auto Fragment = Item->FindDynamicFragmentByClass<UNipcatInventoryItemFragment_Equipment>())
					{
						Fragment->OnEquipmentRemovedFromSlot();
					}
				}
			}
		}
	}
}

void UNipcatEquipmentComponent::RemoveItem(const FGameplayTag& SlotTag, UNipcatInventoryItemInstance* Item)
{
	int32 SlotIndex;
	if (FindSlotByItem(SlotTag, Item, SlotIndex))
	{
		RemoveItemAtSlot(SlotTag, SlotIndex);
	}
}

bool UNipcatEquipmentComponent::FindSlotByItem(const FGameplayTag& SlotTag, UNipcatInventoryItemInstance* Item, int32& OutIndex)
{
	if (const auto SlotGroup = FindSlotGroup(SlotTag))
	{
		OutIndex = -1;
		for (int32 i = 0; i < SlotGroup->MaxSlotCount; i++)
		{
			if (!SlotGroup->Slots.IsValidIndex(i))
			{
				break;
			}
			if (SlotGroup->Slots[i].Item == Item)
			{
				OutIndex = i;
				return true;
			}
		}
	}
	return false;
}

bool UNipcatEquipmentComponent::FindNextEmptySlot(const FGameplayTag& SlotTag, int32& OutIndex)
{
	if (const auto SlotGroup = FindSlotGroup(SlotTag))
	{
		OutIndex = -1;
		for (int32 i = 0; i < SlotGroup->MaxSlotCount; i++)
		{
			if (!SlotGroup->Slots.IsValidIndex(i))
			{
				OutIndex = i;
				return true;
			}
			if (SlotGroup->Slots[i].Item == nullptr)
			{
				OutIndex = i;
				return true;
			}
		}
	}
	return false;
}

bool UNipcatEquipmentComponent::AddItemToNextEmptySlot(const FGameplayTag& SlotTag, UNipcatInventoryItemInstance* Item, int32& OutSlotIndex)
{
	if (const auto SlotGroup = FindSlotGroup(SlotTag))
	{
		if (FindNextEmptySlot(SlotTag, OutSlotIndex))
		{
			if (!SlotGroup->Slots.IsValidIndex(OutSlotIndex))
			{
				SlotGroup->Slots.SetNum(OutSlotIndex + 1);
			}
			AddItemToSlot(SlotTag, Item, OutSlotIndex);
			return true;
		}
	}
	return false;
}

bool UNipcatEquipmentComponent::CyclePrevItem(const FGameplayTag& SlotGroupTag, UNipcatInventoryItemInstance*& OutNewItem,
	int32& OutNewIndex, bool Wrap)
{
	if (const auto SlotGroup = FindSlotGroup(SlotGroupTag))
	{
		return SlotGroup->CyclePrevItem(OutNewItem, OutNewIndex, Wrap);
	}
	return false;
}

bool UNipcatEquipmentComponent::CycleNextItem(const FGameplayTag& SlotGroupTag, UNipcatInventoryItemInstance*& OutNewItem,
	int32& OutNewIndex, bool Wrap)
{
	if (const auto SlotGroup = FindSlotGroup(SlotGroupTag))
	{
		return SlotGroup->CycleNextItem(OutNewItem, OutNewIndex, Wrap);
	}
	return false;
}

TArray<UNipcatInventoryItemInstance*> UNipcatEquipmentComponent::GetAllItemsInSlotGroup(
	const FGameplayTag& SlotGroupTag)
{
	TArray<UNipcatInventoryItemInstance*> Result;
	if (const auto SlotGroup = FindSlotGroup(SlotGroupTag))
	{
		Result = SlotGroup->GetAllItems();
	}
	return Result;
}

TArray<UNipcatInventoryItemInstance*> UNipcatEquipmentComponent::GetAllItemsInInventoryWithSlotTag(
	const FGameplayTag& SlotGroupTag, TArray<UNipcatInventoryItemInstance*> ExcludeItems)
{
	TArray<UNipcatInventoryItemInstance*> Result;
	if (const UNipcatInventoryComponent* InventoryComponent = GetInventoryComponent())
	{
		for (const auto& Item : InventoryComponent->GetAllItems())
		{
			if (ExcludeItems.Contains(Item))
			{
				continue;
			}
			if (const auto& Fragment = Item->FindDynamicFragmentByClass<UNipcatInventoryItemFragment_Equipment>())
			{
				if (Fragment->SlotTypeTag == SlotGroupTag)
				{
					Result.Add(Item);
				}
			}
		}
	}
	return Result;
}

void UNipcatEquipmentComponent::OnSlotUpdated(const FGameplayTag& SlotTypeTag, UNipcatEquipmentSlotGroup* SlotGroup,
                                              int32 Index, UNipcatInventoryItemInstance* ItemInstance)
{
	if (SlotGroup->GetSelectedSlotIndex() == Index && SlotGroup->GetSelectedItem() != ItemInstance)
	{
		SlotGroup->UnSelectItem();
		if (ItemInstance)
		{
			SlotGroup->SelectItem(Index);
		}
	}
	if (SlotGroup->IsEmpty()) // 没东西自动取消选择
	{
		SlotGroup->UnSelectItem();
	}
	else if (SlotGroup->GetSelectedSlotIndex() < 0) // 有东西但没选，自动选择
	{
		SlotGroup->SelectItem(Index);
	}
	K2_OnSlotUpdated(SlotTypeTag, SlotGroup, Index, ItemInstance);
}

void UNipcatEquipmentComponent::OnSlotItemSelected(const UNipcatEquipmentSlotGroup* InSlotGroup, int32 Index)
{
	for (auto& [Tag, SlotGroup] : SlotGroups)
	{
		if (InSlotGroup == SlotGroup)
		{
			UNipcatInventoryItemInstance* Item = SlotGroup->Slots.IsValidIndex(Index) ? SlotGroup->Slots[Index].Item : nullptr;
			BP_OnSlotItemSelected(Tag, Index, Item);
		}
	}
}

UNipcatInventoryComponent* UNipcatEquipmentComponent::GetInventoryComponent() const
{
	if (GetOwner()->GetClass()->ImplementsInterface(UNipcatInventoryComponentOwnerInterface::StaticClass()))
	{
		if (UNipcatInventoryComponent* InventoryComponent = INipcatInventoryComponentOwnerInterface::Execute_GetInventoryComponent(GetOwner()))
		{
			return InventoryComponent;
		}
	}
	return nullptr;
}

void UNipcatEquipmentComponent::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	
	if (Ar.IsSaveGame())
	{
		if (Ar.IsLoading())
		{
			for (const auto& [Tag, SlotGroup] : SlotGroups)
			{
				if (SlotGroup)
				{
					SlotGroup->UnSelectItem();
				}
			}
		}
		UNipcatBasicGameplayFunctionLibrary::SerializeObjectMap<FGameplayTag, UNipcatEquipmentSlotGroup>(Ar, SlotGroups, this);
		if (Ar.IsLoading())
		{
			for (const auto& [Tag, SlotGroup] : SlotGroups)
			{
				if (SlotGroup)
				{
					for (int i = 0; i < SlotGroup->Slots.Num(); ++i)
					{
						OnSlotUpdated(Tag, SlotGroup, i, SlotGroup->Slots[i].Item.Get());
						if (SlotGroup->Slots[i].Item.Get())
						{
							if (const auto Fragment = SlotGroup->Slots[i].Item.Get()->FindDynamicFragmentByClass<UNipcatInventoryItemFragment_Equipment>())
							{
								Fragment->OnEquipmentAddedToSlot();
							}
						}
					}
					const int32 Index = SlotGroup->GetSelectedSlotIndex();
					SlotGroup->UnSelectItem();
					SlotGroup->SelectItem(Index);
				}
			}
		}
	}
}

