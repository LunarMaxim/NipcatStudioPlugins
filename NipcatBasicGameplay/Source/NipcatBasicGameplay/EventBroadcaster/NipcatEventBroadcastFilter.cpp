// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatEventBroadcastFilter.h"


bool UNipcatEventBroadcastFilter::ShouldBroadcastEvent_Implementation(AActor* SourceActor, AActor* TargetActor)
{
	return true;
}

UWorld* UNipcatEventBroadcastFilter::GetWorld() const
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
