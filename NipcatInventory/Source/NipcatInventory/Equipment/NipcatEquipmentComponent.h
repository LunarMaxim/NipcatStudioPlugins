// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatEquipmentSlotGroup.h"

#include "NipcatEquipmentComponent.generated.h"


UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class NIPCATINVENTORY_API UNipcatEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(Categories="Slot.Type", ForceInlineRow))
	TMap<FGameplayTag, TObjectPtr<UNipcatEquipmentSlotGroup>> SlotGroups;

	UFUNCTION(BlueprintPure, meta=(GameplayTagFilter="Slot.Type"))
	UNipcatEquipmentSlotGroup* FindSlotGroup(FGameplayTag SlotGroupTag);

	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotTag"))
	void AddItemToSlot(const FGameplayTag& SlotTag, UNipcatInventoryItemInstance* Item, int32 SlotIndex);
	
	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotTag"))
	void RemoveItemAtSlot(const FGameplayTag& SlotTag, int32 SlotIndex);
	
	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotTag"))
	void RemoveItem(const FGameplayTag& SlotTag, UNipcatInventoryItemInstance* Item);
	
	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotTag"))
	bool FindSlotByItem(const FGameplayTag& SlotTag, UNipcatInventoryItemInstance* Item, int32& OutIndex);
	
	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotTag"))
	bool FindNextEmptySlot(const FGameplayTag& SlotTag, int32& OutIndex);
	
	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotTag"))
	bool AddItemToNextEmptySlot(const FGameplayTag& SlotTag, UNipcatInventoryItemInstance* Item, int32& OutSlotIndex);

	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotGroupTag"))
	bool CyclePrevItem(const FGameplayTag& SlotGroupTag, UNipcatInventoryItemInstance*& OutNewItem, int32& OutNewIndex, bool Wrap = true);

	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotGroupTag"))
	bool CycleNextItem(const FGameplayTag& SlotGroupTag, UNipcatInventoryItemInstance*& OutNewItem, int32& OutNewIndex, bool Wrap = true);

	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotGroupTag"))
	TArray<UNipcatInventoryItemInstance*> GetAllItemsInSlotGroup(const FGameplayTag& SlotGroupTag);

	UFUNCTION(BlueprintCallable, meta=(Categories="Slot.Type", AutoCreateRefTerm="SlotGroupTag,ExcludeItems"))
	TArray<UNipcatInventoryItemInstance*> GetAllItemsInInventoryWithSlotTag(const FGameplayTag& SlotGroupTag, TArray<UNipcatInventoryItemInstance*> ExcludeItems);

	UFUNCTION()
	void OnSlotUpdated(const FGameplayTag& SlotTypeTag, UNipcatEquipmentSlotGroup* SlotGroup, int32 Index, UNipcatInventoryItemInstance* ItemInstance);
	
	UFUNCTION()
	void OnSlotItemSelected(const UNipcatEquipmentSlotGroup* InSlotGroup, int32 Index);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnSlotUpdated")
	void K2_OnSlotUpdated(const FGameplayTag& SlotTypeTag, UNipcatEquipmentSlotGroup* SlotGroup, int32 Index, UNipcatInventoryItemInstance* ItemInstance);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnSlotItemSelected")
	void BP_OnSlotItemSelected(const FGameplayTag& SlotTypeTag, int32 Index, UNipcatInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable)
	UNipcatInventoryComponent* GetInventoryComponent() const;
	
	virtual void Serialize(FArchive& Ar) override;
};
