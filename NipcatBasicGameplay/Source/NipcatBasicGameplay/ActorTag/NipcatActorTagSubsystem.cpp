// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatActorTagSubsystem.h"

void UNipcatActorTagSubsystem::RegisterActor(AActor* Actor, const FGameplayTagContainer& Tags)
{
	if (IsValid(Actor))
	{
		for (auto Tag : Tags.GetGameplayTagArray())
		{
			auto& Array = TaggedActors.FindOrAdd(Tag);
			Array.Set.Add(TWeakObjectPtr(Actor));
		}
	}
}

void UNipcatActorTagSubsystem::UnregisterActor(AActor* Actor, const FGameplayTagContainer& Tags)
{
	if (IsValid(Actor))
	{
		for (auto Tag : Tags.GetGameplayTagArray())
		{
			auto& Array = TaggedActors.FindOrAdd(Tag);
			Array.Set.Remove(Actor);
		}
	}
}

TArray<AActor*> UNipcatActorTagSubsystem::GetAllActorsOfTag(const FGameplayTag& Tag)
{
	TSet<AActor*> Result;
	if (const auto Actors = TaggedActors.Find(Tag))
	{
		for (auto It = Actors->Set.CreateIterator(); It; ++It)
		{
			if (It->IsValid())
			{
				Result.Add(It->Get());
			}
			else
			{
				It.RemoveCurrent();
			}
		}
	}
	return Result.Array();
}

FNipcatWeakActorSet* UNipcatActorTagSubsystem::GetWeakActorSetOfTag(const FGameplayTag& Tag)
{
	return TaggedActors.Find(Tag);
}

TArray<AActor*> UNipcatActorTagSubsystem::GetAllActorsOfTags(const FGameplayTagContainer& Tags)
{
	TSet<AActor*> Result;
	for (const auto Tag : Tags.GetGameplayTagArray())
	{
		Result.Append(GetAllActorsOfTag(Tag));
	}
	return Result.Array();
}

TArray<FNipcatWeakActorSet*> UNipcatActorTagSubsystem::GetWeakActorSetOfTags(const FGameplayTagContainer& Tags)
{
	TArray<FNipcatWeakActorSet*> Result;
	for (const auto Tag : Tags.GetGameplayTagArray())
	{
		if (FNipcatWeakActorSet* Set = GetWeakActorSetOfTag(Tag))
		{
			Result.Add(Set);
		}
	}
	return Result;
}

TArray<AActor*> UNipcatActorTagSubsystem::GetAllActorsByTagQuery(const FGameplayTagQuery& TagQuery)
{
	TArray<AActor*> Result;
	if (!TagQuery.IsEmpty())
	{
		FGameplayTagContainer Tags;
		for (const auto& [Tag, Actors] : TaggedActors)
		{
			if (TagQuery.Matches(FGameplayTagContainer(Tag)))
			{
				Tags.AddTag(Tag);
			}
		}
		Result = GetAllActorsOfTags(Tags);
	}
	return Result;
}

TArray<FNipcatWeakActorSet*> UNipcatActorTagSubsystem::GetWeakActorSetOfTagsByTagQuery(const FGameplayTagQuery& TagQuery)
{
	TArray<FNipcatWeakActorSet*> Result;
	if (!TagQuery.IsEmpty())
	{
		for (auto& [Tag, Actors] : TaggedActors)
		{
			if (TagQuery.Matches(FGameplayTagContainer(Tag)) && !Actors.Set.IsEmpty())
			{
				Result.Add(&Actors);
			}
		}
	}
	return Result;
}

TArray<AActor*> UNipcatActorTagSubsystem::GetActorsOfTagInRange(const FGameplayTag& Tag, FVector Center, double Radius)
{
	TArray<AActor*> Result;
	if (Tag.IsValid())
	{
		if (FNipcatWeakActorSet* Set = GetWeakActorSetOfTag(Tag))
		{
			for (auto It = Set->Set.CreateIterator(); It; ++It)
			{
				if (It->IsValid())
				{
					if (FVector::DistSquared(It->Get()->GetActorLocation(), Center) < Radius * Radius)
					{
						Result.Add(It->Get());
					}
				}
				else
				{
					It.RemoveCurrent();
				}
			}
		}
	}
	return Result;
}

TArray<TWeakObjectPtr<AActor>> UNipcatActorTagSubsystem::GetWeakActorsOfTagInRange(const FGameplayTag& Tag,
	FVector Center, double Radius)
{
	TArray<TWeakObjectPtr<AActor>> Result;
	if (Tag.IsValid())
	{
		if (FNipcatWeakActorSet* Set = GetWeakActorSetOfTag(Tag))
		{
			for (auto It = Set->Set.CreateIterator(); It; ++It)
			{
				if (It->IsValid())
				{
					if (FVector::DistSquared(It->Get()->GetActorLocation(), Center) < Radius * Radius)
					{
						Result.Add(It->Get());
					}
				}
				else
				{
					It.RemoveCurrent();
				}
			}
		}
	}
	return Result;
}
