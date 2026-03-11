// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatTargetingSelectionTask_GameplayTag.h"

#include "NipcatActorTagSubsystem.h"

void UNipcatTargetingSelectionTask_GameplayTag::Execute(const FTargetingRequestHandle& TargetingHandle) const
{
	Super::Execute(TargetingHandle);
	
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Executing);

	if (FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle))
	{
		if (Tag.IsValid() || !TagQuery.IsEmpty())
		{
			if (const auto World = GetWorld())
			{
				if (const auto Subsystem = World->GetSubsystem<UNipcatActorTagSubsystem>())
				{
					if (Tag.IsValid())
					{
						for (const auto Actor : Subsystem->GetAllActorsOfTag(Tag))
						{
							AddTargetActor(TargetingHandle, Actor);
						}
					}
					if (!TagQuery.IsEmpty())
					{
						for (const auto Actor : Subsystem->GetAllActorsByTagQuery(TagQuery))
						{
							AddTargetActor(TargetingHandle, Actor);
						}
					}
				}
			}
		}
	}
	SetTaskAsyncState(TargetingHandle, ETargetingTaskAsyncState::Completed);
}

bool UNipcatTargetingSelectionTask_GameplayTag::AddTargetActor(const FTargetingRequestHandle& TargetingHandle,
	AActor* Actor)
{
	if (Actor)
	{
		FTargetingDefaultResultsSet& ResultsSet = FTargetingDefaultResultsSet::FindOrAdd(TargetingHandle);
		if (!ResultsSet.TargetResults.FindByPredicate([Actor](const FTargetingDefaultResultData& ResultData){ return ResultData.HitResult.GetActor() == Actor; }))
		{
			FTargetingDefaultResultData& ResultData = ResultsSet.TargetResults.AddDefaulted_GetRef();
			ResultData.HitResult.HitObjectHandle = FActorInstanceHandle(Actor);
			ResultData.HitResult.Location = Actor->GetActorLocation();
			if (const FTargetingSourceContext* SourceContext = FTargetingSourceContext::Find(TargetingHandle))
			{
				if (SourceContext->SourceLocation != FVector::Zero())
				{
					ResultData.HitResult.Distance = FVector::Distance(SourceContext->SourceLocation, Actor->GetActorLocation());
				}
				else if (SourceContext->SourceActor)
				{
					ResultData.HitResult.Distance = FVector::Distance(SourceContext->SourceActor->GetActorLocation(), Actor->GetActorLocation());
				}
			}
			return true;
		}
	}
	return false;
}

UWorld* UNipcatTargetingSelectionTask_GameplayTag::GetWorld() const
{
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& Context : WorldContexts)
	{
		if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game)) && (Context.World() != NULL))
		{
			return Context.World();
		}
	}
	return nullptr;
}
