// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NipcatInventory/NipcatInventoryItemInstance.h"

#include "NipcatEquipmentSlotGroup.generated.h"

/**
 * 
 */

class UNipcatEquipmentComponent;
class UNipcatInventoryItemInstance;

USTRUCT(BlueprintType)
struct NIPCATINVENTORY_API FNipcatEquipmentSlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UNipcatInventoryItemInstance> Item;

};

UCLASS(BlueprintType, Blueprintable, EditInlineNew)
class NIPCATINVENTORY_API UNipcatEquipmentSlotGroup : public UObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	int32 MaxSlotCount = 1;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere, meta=(ShowOnlyInnerProperties), SaveGame)
	TArray<FNipcatEquipmentSlot> Slots;

	UNipcatEquipmentSlotGroup();
	
	UFUNCTION(BlueprintCallable)
	void SelectItem(int32 Index);

	UFUNCTION(BlueprintCallable)
	void UnSelectItem();

	int32 GetSelectedSlotIndex() const { return SelectedSlotIndex; }
	
	UNipcatInventoryItemInstance* GetSelectedItem() const { return SelectedItem; }
	
	UFUNCTION(BlueprintPure)
	bool IsEmpty() const;
	
	UFUNCTION(BlueprintPure)
	bool IsValidSlotIndex(int32 Index);
	
	UFUNCTION(BlueprintCallable)
	bool CyclePrevItem(UNipcatInventoryItemInstance*& OutNewItem, int32& OutNewIndex, bool Wrap = true);
	
	UFUNCTION(BlueprintCallable)
	bool CycleNextItem(UNipcatInventoryItemInstance*& OutNewItem, int32& OutNewIndex, bool Wrap = true);
	
	UFUNCTION(BlueprintCallable)
	TArray<UNipcatInventoryItemInstance*> GetAllItems();

	UFUNCTION(BlueprintCallable)
	int32 FindItem(UNipcatInventoryItemInstance* ItemInstance);

	virtual void Serialize(FArchive& Ar) override;

protected:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	int32 SelectedSlotIndex = -1;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	UNipcatInventoryItemInstance* SelectedItem = nullptr;
	
};
