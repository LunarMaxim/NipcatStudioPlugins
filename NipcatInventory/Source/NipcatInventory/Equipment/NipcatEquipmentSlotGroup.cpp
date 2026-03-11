// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatEquipmentSlotGroup.h"

#include "NipcatInventoryItemFragment_Equipment.h"
#include "NipcatInventory/NipcatInventoryComponent.h"
#include "NipcatInventory/NipcatInventoryItemInstance.h"


UNipcatEquipmentSlotGroup::UNipcatEquipmentSlotGroup()
{
	Slots.Reserve(MaxSlotCount);
}

void UNipcatEquipmentSlotGroup::SelectItem(int32 Index)
{
	if (Index != SelectedSlotIndex)
	{
		UnSelectItem();
	
		SelectedSlotIndex = Index;
		if (GetOuter())
		{
			if (const auto& EquipmentComponent = Cast<UNipcatEquipmentComponent>(GetOuter()))
			{
				EquipmentComponent->OnSlotItemSelected(this, Index);
			}
		}
	
		if (IsValidSlotIndex(Index))
		{
			if (const auto NewItem = Slots[SelectedSlotIndex].Item)
			{
				SelectedItem = NewItem;
				if (const auto Fragment = NewItem->FindDynamicFragmentByClass<UNipcatInventoryItemFragment_Equipment>())
				{
					Fragment->OnEquipmentSelected();
				}
			}
		}
	}
}

void UNipcatEquipmentSlotGroup::UnSelectItem()
{
	if (IsValidSlotIndex(SelectedSlotIndex))
	{
		if (SelectedItem)
		{
			if (const auto Fragment = SelectedItem->FindDynamicFragmentByClass<UNipcatInventoryItemFragment_Equipment>())
			{
				Fragment->OnEquipmentUnSelected();
			}
			SelectedItem = nullptr;
		}
	}
	SelectedSlotIndex = -1;
	if (GetOuter())
	{
		if (const auto& EquipmentComponent = Cast<UNipcatEquipmentComponent>(GetOuter()))
		{
			EquipmentComponent->OnSlotItemSelected(this, -1);
		}
	}
}

bool UNipcatEquipmentSlotGroup::IsEmpty() const
{
	for (const auto& Slot : Slots)
	{
		if (Slot.Item)
		{
			return false;
		}
	}
	return true;
}

bool UNipcatEquipmentSlotGroup::IsValidSlotIndex(int32 Index)
{
	const bool IsValid = Index >= 0 && (Index < MaxSlotCount || MaxSlotCount <= 0);
	if (IsValid && !Slots.IsValidIndex(Index))
	{
		Slots.SetNum(Index + 1);
	}
	return IsValid;
}

bool UNipcatEquipmentSlotGroup::CyclePrevItem(UNipcatInventoryItemInstance*& OutNewItem, int32& OutNewIndex, bool Wrap)
{
	if (SelectedSlotIndex >= 0)
	{
		for (int32 i = 0; i < SelectedSlotIndex; i++)
		{
			if (Slots[i].Item)
			{
				SelectItem(i);
				OutNewItem = Slots[i].Item;
				OutNewIndex = i;
				return true;
			}
		}
		if (Wrap)
		{
			for (int32 i = SelectedSlotIndex + 1; i < Slots.Num(); i++)
			{
				if (Slots[i].Item)
				{
					SelectItem(i);
					OutNewItem = Slots[i].Item;
					OutNewIndex = i;
					return true;
				}
			}
		}
	}
	return false;
}

bool UNipcatEquipmentSlotGroup::CycleNextItem(UNipcatInventoryItemInstance*& OutNewItem, int32& OutNewIndex, bool Wrap)
{
	if (SelectedSlotIndex >= 0)
	{
		for (int32 i = SelectedSlotIndex + 1; i < Slots.Num(); i++)
		{
			if (Slots[i].Item)
			{
				SelectItem(i);
				OutNewItem = Slots[i].Item;
				OutNewIndex = i;
				return true;
			}
		}
		if (Wrap)
		{
			for (int32 i = 0; i < SelectedSlotIndex; i++)
			{
				if (Slots[i].Item)
				{
					SelectItem(i);
					OutNewItem = Slots[i].Item;
					OutNewIndex = i;
					return true;
				}
			}
		}
	}
	return false;
}

TArray<UNipcatInventoryItemInstance*> UNipcatEquipmentSlotGroup::GetAllItems()
{
	TArray<UNipcatInventoryItemInstance*> Result;
	for (const auto& Slot : Slots)
	{
		if (Slot.Item)
		{
			Result.Add(Slot.Item);
		}
	}
	return Result;
}

int32 UNipcatEquipmentSlotGroup::FindItem(UNipcatInventoryItemInstance* ItemInstance)
{
	for (int32 i = 0; i < Slots.Num() ; ++i)
	{
		if (Slots[i].Item == ItemInstance)
		{
			return i;
		}
	}
	return INDEX_NONE;
}

void UNipcatEquipmentSlotGroup::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	if (Ar.IsSaveGame())
	{
		if (const UNipcatEquipmentComponent* EquipmentComponent = Cast<UNipcatEquipmentComponent>(GetOuter()))
		{
			if (const auto& InventoryComponent = EquipmentComponent->GetInventoryComponent())
			{
				for (auto& Slot : Slots)
				{
					if (Ar.IsSaving())
					{
						bool ItemExist = IsValid(Slot.Item.Get());
						Ar << ItemExist;
						if (ItemExist)
						{
							Ar << Slot.Item->GUID;
						}
					}
					else
					{
						bool ItemExist;
						Ar << ItemExist;
						if (ItemExist)
						{
							FGuid GUID;
							Ar << GUID;
							if (const auto& Item = InventoryComponent->FindItemByGuid(GUID))
							{
								Slot.Item = Item;
							}
						}
					}
				}
			}
		}
	}
}
