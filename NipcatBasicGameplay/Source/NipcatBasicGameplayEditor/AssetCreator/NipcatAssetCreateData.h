// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorUtilityObject.h"
#include "NipcatAssetCreateData.generated.h"


class UFactory;
/**
 * 
 */
UCLASS(EditInlineNew, BlueprintType, Blueprintable, Abstract)
class NIPCATBASICGAMEPLAYEDITOR_API UNipcatAssetCreateData : public UEditorUtilityObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSubclassOf<UObject> AssetClass;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Prefix;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString Suffix;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString RelativePath;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsBlueprint;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bIsBlueprint", EditConditionHides))
	TSubclassOf<UObject> BlueprintClass = UBlueprint::StaticClass();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bIsBlueprint", EditConditionHides))
	UFactory* Factory;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, Export)
	TArray<UNipcatAssetCreateData*> SubData;

	UFUNCTION(BlueprintCallable)
	UObject* CreateAsset(UObject* RootAsset, TArray<UObject*>& CreatedAssets);

	UFUNCTION()
	virtual UObject* ExecuteCreateAsset(UObject* RootAsset, UPARAM(ref) TArray<UObject*>& CreatedAssets);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="ExecuteCreateAsset")
	UObject* BP_ExecuteCreateAsset(UObject* RootAsset, const TArray<UObject*>& CreatedAssets);
	
	UFUNCTION(BlueprintCallable)
	virtual void PostCreateAsset(UObject* RootAsset, UObject* CreatedAsset, const TArray<UObject*>& CreatedAssets);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="PostCreateAsset")
	void BP_PostCreateAsset(UObject* RootAsset, UObject* CreatedAsset, const TArray<UObject*>& CreatedAssets);
	
	UFUNCTION(BlueprintPure, meta=(DeterminesOutputType=Class, AutoCreateRefTerm="CreatedAssets"))
	UObject* FindCreatedAssetByClass(const TArray<UObject*>& CreatedAssets, TSubclassOf<UObject> Class);
	
	template <typename ResultClass>
	const ResultClass* FindCreatedAssetByClass(TArray<UObject*> CreatedAssets) const
	{
		return (ResultClass*)FindCreatedAssetByClass(CreatedAssets, ResultClass::StaticClass());
	}
};
