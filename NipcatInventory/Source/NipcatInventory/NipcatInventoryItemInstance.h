// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NipcatInventoryItemDefinition.h"
#include "NipcatInventoryItemInstance.generated.h"


class UNipcatInventoryComponent;
class UNipcatInventoryItemDefinition;
class UNipcatInventoryItemFragment;
struct FGameplayTag;

/**
 * UNipcatInventoryItemInstance
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew)
class NIPCATINVENTORY_API UNipcatInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UNipcatInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, SaveGame)
	FGuid GUID;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, SaveGame)
	int32 Count;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, SaveGame)
	TSoftObjectPtr<UNipcatInventoryItemDefinition> ItemDef;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced)
	TArray<TObjectPtr<UNipcatInventoryItemFragment>> DynamicFragments;
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	virtual int32 AddItemCount(int32 StackCount);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Inventory")
	virtual int32 RemoveItemCount(int32 StackCount);

	UFUNCTION()
	virtual void NotifyItemCountChanged(int32 DeltaCount);
	
	UFUNCTION()
	virtual void NotifySaveGameLoaded();
	
	UFUNCTION(BlueprintCallable)
	UNipcatInventoryItemDefinition* GetItemDef() const
	{
		return ItemDef.LoadSynchronous();
	}

	UFUNCTION(BlueprintPure, Category="ItemInstance")
	int32 GetCount() const;

	UFUNCTION(BlueprintCallable, BlueprintPure)
	UNipcatInventoryComponent* GetInventoryComponent();
	
	UFUNCTION(BlueprintCallable, BlueprintCallable)
	TArray<UNipcatInventoryItemFragment*> GetAllFragments();
	
	UFUNCTION(BlueprintCallable, BlueprintCallable)
	TArray<UNipcatInventoryItemFragment*> GetAllDynamicFragments();
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const UNipcatInventoryItemFragment* FindFragmentByClass(TSubclassOf<UNipcatInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	const ResultClass* FindFragmentByClass() const
	{
		return (ResultClass*)FindFragmentByClass(ResultClass::StaticClass());
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const UNipcatInventoryItemFragment* FindDynamicFragmentByClass(TSubclassOf<UNipcatInventoryItemFragment> FragmentClass) const;

	template <typename ResultClass>
	ResultClass* FindDynamicFragmentByClass() const
	{
		return (ResultClass*)FindDynamicFragmentByClass(ResultClass::StaticClass());
	}

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void Serialize(FArchive& Ar) override;
	
private:
	void Init(const UNipcatInventoryItemDefinition* InDef);
	void InitDynamicFragments();

	friend struct FNipcatInventoryList;
	friend class UNipcatInventoryComponent;

};


USTRUCT(BlueprintType)
struct NIPCATINVENTORY_API FInstancedNipcatInventoryItemInstance
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UNipcatInventoryItemInstance* ItemInstance;

	bool operator==(const FInstancedNipcatInventoryItemInstance& Other) const
	{
		return ItemInstance == Other.ItemInstance;
	}

	friend FORCEINLINE uint32 GetTypeHash(const FInstancedNipcatInventoryItemInstance& Value)
	{
		return GetTypeHash(Value.ItemInstance);
	}
};

USTRUCT(BlueprintType)
struct NIPCATINVENTORY_API FInstancedNipcatInventoryItemInstanceWithProbability
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UNipcatInventoryItemInstance* ItemInstance;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(UIMin = 0, UIMax = 100, ClampMin = 0, ClampMax = 100, Units="Percent"))
	double Probability = 100.f;
};