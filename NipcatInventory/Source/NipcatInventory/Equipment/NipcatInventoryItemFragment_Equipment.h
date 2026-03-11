// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatEquipmentComponent.h"
#include "NipcatInventory/NipcatInventoryItemDefinition.h"
#include "NipcatInventoryItemFragment_Equipment.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATINVENTORY_API UNipcatInventoryItemFragment_Equipment : public UNipcatInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
	UNipcatEquipmentComponent* EquipmentComponent;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="Slot.Type"), SaveGame)
	FGameplayTag SlotTypeTag;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	bool AutoAddToEmptySlotOnItemAdded = true;

	UPROPERTY(BlueprintReadOnly, SaveGame)
	int32 CurrentSlotIndex = -1;

	UFUNCTION(BlueprintCallable)
	void AddToSlot(int32 SlotIndex);
	
	UFUNCTION(BlueprintCallable)
	bool AddToNextEmptySlot();
	
	UFUNCTION(BlueprintCallable)
	void RemoveFromSlot();

	UFUNCTION(BlueprintNativeEvent)
	void OnEquipmentSelected();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnEquipmentUnSelected();
	
	UFUNCTION()
	virtual void OnEquipmentAddedToSlot();
	
	UFUNCTION()
	virtual void OnEquipmentRemovedFromSlot();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnEquipmentAddedToSlot")
	void BP_OnEquipmentAddedToSlot();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnEquipmentRemovedFromSlot")
	void BP_OnEquipmentRemovedFromSlot();
	
	virtual void Init() override;
	virtual void OnPickUpItem(int32 Count, bool IsNewItem) override;
	virtual void PreInstanceRemoved_Implementation() override;
	virtual void PreAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor) override;
	virtual void PostAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor) override;
};
