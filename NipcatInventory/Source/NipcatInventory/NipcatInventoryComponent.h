// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "NipcatInventoryComponent.generated.h"


class UNipcatInventoryItemDefinition;
class UNipcatInventoryItemInstance;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent))
class NIPCATINVENTORY_API UNipcatInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNipcatInventoryComponent();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	TArray<TObjectPtr<UNipcatInventoryItemInstance>> ItemInstances;
	
protected:
	UPROPERTY()
	TObjectPtr<UDataTable> ItemTable;

public:
	/// 
	/// @param ItemDef 
	/// @param StackCount Count To Add
	/// @return New Item Count
	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	int32 AddItemByDefinition(const UNipcatInventoryItemDefinition* ItemDef, UNipcatInventoryItemInstance*& OutItemInstance, int32 StackCount = 1, bool bIsPickUp = false);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	int32 AddItem(UNipcatInventoryItemInstance* ItemInstance, int32 StackCount = 1, bool bIsPickUp = false);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	bool TransferItem(UNipcatInventoryItemInstance* ItemInstance, UNipcatInventoryComponent* OtherInventory);

	/// 
	/// @param ItemDef
	/// @param StackCount Count To Remove
	/// @return New Item Count
	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	int32 RemoveItemByDefinition(const UNipcatInventoryItemDefinition* ItemDef, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	int32 RemoveItem(UNipcatInventoryItemInstance* ItemInstance, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	int32 RemoveAllItem(UNipcatInventoryItemInstance* ItemInstance);

	UFUNCTION()
	virtual void OnPickUpItem(UNipcatInventoryItemInstance* ItemInstance, int32 Count, bool IsNewItem);

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnPickUpItem")
	void BP_OnPickUpItem(UNipcatInventoryItemInstance* ItemInstance, int32 Count, bool IsNewItem);
	
	void OnItemCountEmpty(UNipcatInventoryItemInstance* ItemInstance);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	TArray<UNipcatInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	UNipcatInventoryItemInstance* FindItemByDefinition(const UNipcatInventoryItemDefinition* ItemDef) const;
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	UNipcatInventoryItemInstance* FindItemByGuid(const FGuid Guid) const;
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory", meta=(GameplayTagFilter="Item"))
	TArray<UNipcatInventoryItemInstance*> FindItemsByTag(const FGameplayTag Tag, bool bExact = false) const;
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory", meta=(GameplayTagFilter="Item"))
	TArray<UNipcatInventoryItemInstance*> FindItemsByTags(const FGameplayTagContainer Tags, bool bExact = false) const;


	virtual void Serialize(FArchive& Ar) override;
};
