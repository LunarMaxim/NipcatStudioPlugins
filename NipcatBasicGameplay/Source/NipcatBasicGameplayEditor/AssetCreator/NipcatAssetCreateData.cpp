// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAssetCreateData.h"


UObject* UNipcatAssetCreateData::CreateAsset(UObject* RootAsset, TArray<UObject*>& CreatedAssets)
{
	UObject* CreatedAsset = ExecuteCreateAsset(RootAsset, CreatedAssets);
	TArray<UObject*> CreatedAssetsForSubData;
	int32 SubDataIndex = 0;
	for (const auto& Data : SubData)
	{
		CreatedAssetsForSubData.Add(Data->ExecuteCreateAsset(RootAsset, CreatedAssets));
	}
	for (const auto& Data : SubData)
	{
		Data->PostCreateAsset(RootAsset, CreatedAssetsForSubData[SubDataIndex], CreatedAssets);
		SubDataIndex++;
	}
	PostCreateAsset(RootAsset, CreatedAsset, CreatedAssets);
	return CreatedAsset;
}

UObject* UNipcatAssetCreateData::ExecuteCreateAsset(UObject* RootAsset, TArray<UObject*>& CreatedAssets)
{
	UObject* CreatedAsset = BP_ExecuteCreateAsset(RootAsset, CreatedAssets);
	CreatedAssets.Add(CreatedAsset);
	return CreatedAsset;
}

void UNipcatAssetCreateData::PostCreateAsset(UObject* RootAsset, UObject* CreatedAsset,
	const TArray<UObject*>& CreatedAssets)
{
	BP_PostCreateAsset(RootAsset, CreatedAsset, CreatedAssets);
}

UObject* UNipcatAssetCreateData::FindCreatedAssetByClass(const TArray<UObject*>& CreatedAssets, TSubclassOf<UObject> Class)
{
	for (const auto Asset : CreatedAssets)
	{
		if (Asset && Asset->IsA(Class))
		{
			return Asset;
		}
	}
	return nullptr;
}
