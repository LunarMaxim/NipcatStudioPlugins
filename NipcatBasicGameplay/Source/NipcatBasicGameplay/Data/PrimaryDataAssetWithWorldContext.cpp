// Copyright Nipcat Studio All Rights Reserved.


#include "PrimaryDataAssetWithWorldContext.h"

UWorld* UPrimaryDataAssetWithWorldContext::GetWorld() const
{
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& Context : WorldContexts)
	{
		if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game)) && (Context.World() != NULL))
		{
			return Context.World();
		}
	}
	return Super::GetWorld();
}