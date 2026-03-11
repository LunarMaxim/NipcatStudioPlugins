// Fill out your copyright notice in the Description page of Project Settings.


#include "NipcatInventoryComponent.h"

#include "NipcatInventoryItemDefinition.h"
#include "NipcatInventoryItemInstance.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"


UNipcatInventoryComponent::UNipcatInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


int32 UNipcatInventoryComponent::AddItemByDefinition(const UNipcatInventoryItemDefinition* ItemDef, UNipcatInventoryItemInstance*& OutItemInstance, int32 StackCount, bool bIsPickUp)
{
	if (ItemDef)
	{
		if (ItemDef->BaseData.bUnique && StackCount > 0)
		{
			for (int i = 0; i < StackCount; ++i)
			{
				UNipcatInventoryItemInstance* NewItemInstance = NewObject<UNipcatInventoryItemInstance>(this, ItemDef->GetInstanceClass());
				OutItemInstance = NewItemInstance;
				NewItemInstance->Init(ItemDef);
				ItemInstances.Add(NewItemInstance);
				NewItemInstance->AddItemCount(1);
				
				if (bIsPickUp)
				{
					OnPickUpItem(NewItemInstance, NewItemInstance->Count, true);
				}
			}
			return StackCount;
		}
		else if (UNipcatInventoryItemInstance* ItemInstance = FindItemByDefinition(ItemDef))
		{
			OutItemInstance = ItemInstance;
			return AddItem(ItemInstance, StackCount, bIsPickUp);
		}
		else if (StackCount > 0)
		{
			UNipcatInventoryItemInstance* NewItemInstance = NewObject<UNipcatInventoryItemInstance>(this, ItemDef->GetInstanceClass());
			OutItemInstance = NewItemInstance;
			NewItemInstance->Init(ItemDef);
			ItemInstances.Add(NewItemInstance);
			
			if (bIsPickUp)
			{
				OnPickUpItem(NewItemInstance, NewItemInstance->Count, false);
			}
			return NewItemInstance->AddItemCount(StackCount);
		}
	}
	return 0;
}

int32 UNipcatInventoryComponent::AddItem(UNipcatInventoryItemInstance* ItemInstance, int32 StackCount, bool bIsPickUp)
{
	if (ItemInstance)
	{
		if (UNipcatInventoryComponent* OwningInventory = ItemInstance->GetInventoryComponent())
		{
			if (OwningInventory != this)
			{
				if (OwningInventory->TransferItem(ItemInstance, this))
				{
					return ItemInstance->Count;
				}
			}
			else
			{
				if (bIsPickUp && StackCount > 0)
				{
					OnPickUpItem(ItemInstance, StackCount, false);
				}
				const int32 RemovedCount = ItemInstance->AddItemCount(StackCount);
				if (ItemInstance->GetCount() <= 0)
				{
					OnItemCountEmpty(ItemInstance);
				}
				return RemovedCount;
			}
		}
		else if (StackCount > 0) // No Owning Inventory
		{
			if (!ItemInstance->ItemDef.LoadSynchronous()->BaseData.bUnique)
			{
				if (const auto ExistItem = FindItemByDefinition(ItemInstance->ItemDef.LoadSynchronous()))
				{
					if (bIsPickUp && StackCount > 0)
					{
						OnPickUpItem(ExistItem, StackCount, false);
					}
					const int32 RemovedCount = ExistItem->AddItemCount(StackCount);
					if (ExistItem->GetCount() <= 0)
					{
						OnItemCountEmpty(ExistItem);
					}
					return RemovedCount;
				}
			}
			else
			{
				ItemInstance->Rename(nullptr, this);
				ItemInstance->InitDynamicFragments();
				ItemInstances.Add(ItemInstance);
				if (bIsPickUp)
				{
					OnPickUpItem(ItemInstance, ItemInstance->Count, true);
				}
				return ItemInstance->Count;
			}
		}
	}
	return 0;
}

bool UNipcatInventoryComponent::TransferItem(UNipcatInventoryItemInstance* ItemInstance,
	UNipcatInventoryComponent* OtherInventory)
{
	if (ItemInstance && OtherInventory)
	{
		if (ItemInstance->GetInventoryComponent() == this)
		{
			for (const auto& Fragment : ItemInstance->DynamicFragments)
			{
				Fragment->PreInstanceRemoved();
			}
			ItemInstances.Remove(ItemInstance);
			ItemInstance->Rename(nullptr, OtherInventory);
			OtherInventory->ItemInstances.Add(ItemInstance);
			return true;
		}
	}
	return false;
}


int32 UNipcatInventoryComponent::RemoveItemByDefinition(const UNipcatInventoryItemDefinition* ItemDef, int32 StackCount)
{
	if (ItemDef)
	{
		if (UNipcatInventoryItemInstance* ItemInstance = FindItemByDefinition(ItemDef))
		{
			RemoveItem(ItemInstance, StackCount);
		}
	}
	return 0;
}

int32 UNipcatInventoryComponent::RemoveItem(UNipcatInventoryItemInstance* ItemInstance, int32 StackCount)
{
	if (ItemInstance)
	{
		if (StackCount < 0)
		{
			return AddItem(ItemInstance, -ItemInstance->GetCount());
		}
		else if (StackCount > 0)
		{
			return AddItem(ItemInstance, -StackCount);
		}
	}
	return 0;
}

int32 UNipcatInventoryComponent::RemoveAllItem(UNipcatInventoryItemInstance* ItemInstance)
{
	if (ItemInstance)
	{
		return AddItem(ItemInstance, -ItemInstance->Count);
	}
	return 0;
}


void UNipcatInventoryComponent::OnPickUpItem(UNipcatInventoryItemInstance* ItemInstance, int32 Count, bool IsNewItem)
{
	for (const auto& Fragment : ItemInstance->DynamicFragments)
	{
		Fragment->OnPickUpItem(Count, IsNewItem);
	}
	
	BP_OnPickUpItem(ItemInstance, Count, IsNewItem);
}

void UNipcatInventoryComponent::OnItemCountEmpty(UNipcatInventoryItemInstance* ItemInstance)
{
	if (ItemInstance->ItemDef->BaseData.bRemoveItemOnEmpty)
	{
		for (const auto& Fragment : ItemInstance->DynamicFragments)
		{
			Fragment->PreInstanceRemoved();
		}
		
		ItemInstances.Remove(ItemInstance);
	}
}

TArray<UNipcatInventoryItemInstance*> UNipcatInventoryComponent::GetAllItems() const
{
	return ItemInstances;
}

UNipcatInventoryItemInstance* UNipcatInventoryComponent::FindItemByDefinition(const UNipcatInventoryItemDefinition* ItemDef) const
{
	if (ItemDef)
	{
		for (const auto ItemInstance : ItemInstances)
		{
			if (ItemInstance && ItemInstance->ItemDef)
			{
				if (ItemInstance->ItemDef == ItemDef)
				{
					return ItemInstance;
				}
			}
		}
	}

	return nullptr;
}

UNipcatInventoryItemInstance* UNipcatInventoryComponent::FindItemByGuid(const FGuid Guid) const
{
	for (const auto& ItemInstance : ItemInstances)
	{
		if (ItemInstance)
		{
			if (ItemInstance->GUID == Guid)
			{
				return ItemInstance;
			}
		}
	}
	return nullptr;
}

TArray<UNipcatInventoryItemInstance*> UNipcatInventoryComponent::FindItemsByTag(const FGameplayTag Tag, bool bExact) const
{
	TArray<UNipcatInventoryItemInstance*> Result;
	if (Tag.IsValid())
	{
		Result.Reserve(ItemInstances.Num());
		for (auto ItemInstance : ItemInstances)
		{
			if (ItemInstance && ItemInstance->ItemDef)
			{
				if ((bExact && ItemInstance->ItemDef->BaseData.Type == Tag) || (!bExact && Tag.MatchesTag(ItemInstance->ItemDef->BaseData.Type)))
				{
					Result.Add(ItemInstance);
				}
			}
		}
	}

	return Result;
}

TArray<UNipcatInventoryItemInstance*> UNipcatInventoryComponent::FindItemsByTags(const FGameplayTagContainer Tags, bool bExact) const
{
	TArray<UNipcatInventoryItemInstance*> Result;
	if (Tags.IsValid() && !Tags.IsEmpty())
	{
		Result.Reserve(ItemInstances.Num());
		for (auto ItemInstance : ItemInstances)
		{
			if (ItemInstance && ItemInstance->ItemDef)
			{
				if ((bExact && Tags.HasTagExact(ItemInstance->ItemDef->BaseData.Type)) || (!bExact && Tags.HasTag(ItemInstance->ItemDef->BaseData.Type)))
				{
					Result.Add(ItemInstance);
				}
			}
		}
	}

	return Result;
}

void UNipcatInventoryComponent::Serialize(FArchive& Ar)
{
	
	if (Ar.IsSaveGame() && Ar.IsLoading())
	{
		for (auto It(ItemInstances.CreateIterator()); It; ++It)
		{
			UNipcatInventoryItemInstance* Item = It->Get();
			RemoveAllItem(Item);
		}
	}
	
	Super::Serialize(Ar);
	
	if (Ar.IsSaveGame())
	{
		UNipcatBasicGameplayFunctionLibrary::SerializeObjectArray<UNipcatInventoryItemInstance>(Ar, ItemInstances, this);
	}
}


