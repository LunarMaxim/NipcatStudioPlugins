// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "NipcatInventoryItemDefinition.generated.h"

class UNipcatInventoryItemDefinition;
class UNipcatInventoryComponent;
class UNipcatInventoryItemInstance;
template <typename T>
class TSubclassOf;


//////////////////////////////////////////////////////////////////////

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract, Blueprintable, BlueprintType, CollapseCategories, meta=(ShowWorldContextPin))
class NIPCATINVENTORY_API UNipcatInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	AActor* AvatarActor;

	UPROPERTY(BlueprintReadOnly)
	UNipcatInventoryComponent* InventoryComponent;
	
	UPROPERTY(BlueprintReadOnly)
	const UNipcatInventoryItemDefinition* ItemDef;
	
	UPROPERTY(BlueprintReadOnly)
	UNipcatInventoryItemInstance* ItemInstance;

	UFUNCTION()
	virtual void Init();
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnInitialize();
	
	UFUNCTION(BlueprintNativeEvent)
	void OnInstanceCreated();

	UFUNCTION()
	virtual void OnPickUpItem(int32 Count, bool IsNewItem);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnPickUpItem")
	void BP_OnPickUpItem(int32 Count, bool IsNewItem);
	
	UFUNCTION(BlueprintNativeEvent)
	void PreInstanceRemoved();
	
	UFUNCTION(BlueprintCallable)
	void NotifyAvatarActorChanged();
	
	UFUNCTION()
	virtual void PreAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor);
	
	UFUNCTION()
	virtual void PostAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor);
	
	UFUNCTION()
	virtual void OnItemCountChanged(int32 OldCount, int32 NewCount, int32 DeltaCount);
	
	UFUNCTION()
	virtual void OnSaveGameLoaded();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="PreAvatarActorChanged")
	void K2_PreAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="PostAvatarActorChanged")
	void K2_PostAvatarActorChanged(AActor* OldAvatarActor, AActor* NewAvatarActor);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnItemCountChanged")
	void K2_OnItemCountChanged(int32 OldCount, int32 NewCount, int32 DeltaCount);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnSaveGameLoaded")
	void K2_OnSaveGameLoaded();
	
	virtual UWorld* GetWorld() const override;
};

UCLASS(meta=(DisplayName="Grid"))
class NIPCATINVENTORY_API UNipcatInventoryItemFragment_Grid : public UNipcatInventoryItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, DisplayName="数量限制", Category="ItemFragment", meta=(ClampMin=2, UIMin=2))
	int32 MaxCount{2};
};

//////////////////////////////////////////////////////////////////////

USTRUCT(BlueprintType)
struct NIPCATINVENTORY_API FNipcatInventoryItemBaseData : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NipcatInventory|ItemDefinition")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NipcatInventory|ItemDefinition", meta=(MultiLine))
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NipcatInventory|ItemDefinition", meta=(MultiLine))
	FText Details;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NipcatInventory|ItemDefinition")
	TSoftObjectPtr<UTexture2D> Icon = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NipcatInventory|ItemDefinition")
	TSoftObjectPtr<UTexture2D> Thumbnail = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NipcatInventory|ItemDefinition", meta=(Categories = Item))
	FGameplayTag Type;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NipcatInventory|ItemDefinition", meta=(Categories = "Item.Rare"))
	FGameplayTag Rare;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NipcatInventory|ItemDefinition")
	bool bUnique = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="NipcatInventory|ItemDefinition")
	bool bRemoveItemOnEmpty = true;
};

/**
 * UNipcatInventoryItemDefinition
 */
UCLASS(BlueprintType, Blueprintable, meta=(ShowWorldContextPin))
class NIPCATINVENTORY_API UNipcatInventoryItemDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="NipcatInventory|ItemDefinition", meta=(ShowOnlyInnerProperties))
	FNipcatInventoryItemBaseData BaseData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="NipcatInventory|ItemDefinition")
	TSubclassOf<UNipcatInventoryItemInstance> InstanceClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="NipcatInventory|ItemDefinition", Instanced)
	TArray<TObjectPtr<UNipcatInventoryItemFragment>> Fragments;

	virtual void GetAssetRegistryTags(FAssetRegistryTagsContext Context) const override;
	
	UFUNCTION(BlueprintImplementableEvent)
	TMap<FName, FString> BP_GetAssetRegistryTags() const;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
	
	const UNipcatInventoryItemFragment* FindFragmentByClass(const TSubclassOf<UNipcatInventoryItemFragment> FragmentClass) const;

	TSubclassOf<UNipcatInventoryItemInstance> GetInstanceClass() const;

	UTexture2D* GetIcon() const;
};


USTRUCT()
struct FNipcatInventoryItemTableRow : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, DisplayName="定义", Category="Inventory", meta=(MetaClass="/Script/NipcatInventory.NipcatInventoryItemDefinition"))
	FSoftObjectPath ItemDefinitionPath;
};
