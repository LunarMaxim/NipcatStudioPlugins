// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInventoryItemFragment_Equipment.h"

#include "NipcatEquipmentComponentOwnerInterface.h"


void UNipcatInventoryItemFragment_Equipment::AddToSlot(int32 SlotIndex)
{
	EquipmentComponent->AddItemToSlot(SlotTypeTag, ItemInstance, SlotIndex);
	CurrentSlotIndex = SlotIndex;
}

bool UNipcatInventoryItemFragment_Equipment::AddToNextEmptySlot()
{
	return EquipmentComponent->AddItemToNextEmptySlot(SlotTypeTag, ItemInstance, CurrentSlotIndex);
}

void UNipcatInventoryItemFragment_Equipment::RemoveFromSlot()
{
	EquipmentComponent->RemoveItem(SlotTypeTag, ItemInstance);
}

void UNipcatInventoryItemFragment_Equipment::OnEquipmentAddedToSlot()
{
	BP_OnEquipmentAddedToSlot();
}

void UNipcatInventoryItemFragment_Equipment::OnEquipmentRemovedFromSlot()
{
	BP_OnEquipmentRemovedFromSlot();
}

void UNipcatInventoryItemFragment_Equipment::OnEquipmentSelected_Implementation()
{
}

void UNipcatInventoryItemFragment_Equipment::OnEquipmentUnSelected_Implementation()
{
}

void UNipcatInventoryItemFragment_Equipment::Init()
{
	Super::Init();
	if (AvatarActor->GetClass()->ImplementsInterface(UNipcatEquipmentComponentOwnerInterface::StaticClass()))
	{
		EquipmentComponent = INipcatEquipmentComponentOwnerInterface::Execute_GetNipcatEquipmentComponent(AvatarActor);
	}
}

void UNipcatInventoryItemFragment_Equipment::OnPickUpItem(int32 Count, bool IsNewItem)
{
	if (AutoAddToEmptySlotOnItemAdded)
	{
		AddToNextEmptySlot();
	}
	Super::OnPickUpItem(Count, IsNewItem);
}

void UNipcatInventoryItemFragment_Equipment::PreInstanceRemoved_Implementation()
{
	RemoveFromSlot();
}

void UNipcatInventoryItemFragment_Equipment::PreAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor)
{
	if (OldAvatarActor && EquipmentComponent)
	{
		RemoveFromSlot();
	}
	Super::PreAvatarActorChanged(OldAvatarActor, NewAvatarActor);
}

void UNipcatInventoryItemFragment_Equipment::PostAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor)
{
	if (NewAvatarActor)
	{
		if (NewAvatarActor->GetClass()->ImplementsInterface(UNipcatEquipmentComponentOwnerInterface::StaticClass()))
		{
			EquipmentComponent = INipcatEquipmentComponentOwnerInterface::Execute_GetNipcatEquipmentComponent(NewAvatarActor);
		}
		
		if (EquipmentComponent)
		{/*
			if (AutoAddToEmptySlotOnItemAdded)
			{
				AddToNextEmptySlot();
			}
			if (EquipmentComponent->FindSlotGroup(SlotTypeTag)->GetSelectedSlotIndex() == CurrentSlotIndex)
			{
				OnEquipmentSelected();
			}*/
		}
	}
	Super::PostAvatarActorChanged(OldAvatarActor, NewAvatarActor);
}
