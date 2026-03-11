// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatFlowSubsystem.h"

#include "NipcatFlowComponent.h"
#include "NipcatFlowTypes.h"

void UNipcatFlowSubsystem::RegisterComponentByGuid(UNipcatFlowComponent* Component)
{
	if (Component && Component->Guid.IsValid())
	{
		FlowComponentGuidRegistry.Add(Component->Guid, Component);
	}
}

void UNipcatFlowSubsystem::UnregisterComponentByGuid(UNipcatFlowComponent* Component)
{
	if (Component && Component->Guid.IsValid())
	{
		FlowComponentGuidRegistry.Remove(Component->Guid);
	}
}

void UNipcatFlowSubsystem::AddIdentityInfoModifier(const FNipcatFlowIdentityInfoModifier& Modifier)
{
	if (Modifier.bUseGuid)
	{
		IdentityInfoModifiersFromGuid.FindOrAdd(Modifier.ComponentGuid).Array.Emplace(Modifier);
		RefreshIdentityInfoByGuidImmediately(Modifier.ComponentGuid);
	}
	else
	{
		IdentityInfoModifiersFromTags.FindOrAdd(Modifier.IdentityTag).Array.Emplace(Modifier);
		RefreshIdentityInfoByTagsImmediately(Modifier.IdentityTag, Modifier.bExactMatch);
	}
}



void UNipcatFlowSubsystem::RemoveAllIdentityModifiers(UObject* SourceObject)
{
	bool Removed = false;
	TArray<FNipcatFlowIdentityInfoModifier> RemovedModifiers;
	for (auto& [Tag, ModifierArray] : IdentityInfoModifiersFromTags)
	{
		for (auto It = ModifierArray.Array.CreateIterator(); It; ++It)
		{
			if (It->SourceObject == SourceObject)
			{
				RemovedModifiers.Add(*It);
				Removed = true;
				It.RemoveCurrent();
			}
		}
	}

	for (auto& [Guid, ModifierArray] : IdentityInfoModifiersFromGuid)
	{
		for (auto It = ModifierArray.Array.CreateIterator(); It; ++It)
		{
			if (It->SourceObject == SourceObject)
			{
				RemovedModifiers.Add(*It);
				Removed = true;
				It.RemoveCurrent();
			}
		}
	}
	
	if (Removed)
	{
		for (auto& RemovedModifier : RemovedModifiers)
		{
			if (RemovedModifier.bUseGuid)
			{
				RefreshIdentityInfoByGuidImmediately(RemovedModifier.ComponentGuid);
			}
			else
			{
				RefreshIdentityInfoByTagsImmediately(RemovedModifier.IdentityTag, RemovedModifier.bExactMatch);
			}
		}
	}
}

void UNipcatFlowSubsystem::GetCombinedIdentityInfoByTags(const FGameplayTagContainer& IdentityTags, FNipcatFlowIdentityInfo& IdentityInfo)
{
	for (const auto IdentityTag : IdentityTags.GetGameplayTagParents())
	{
		if (const auto ModifierArray = IdentityInfoModifiersFromTags.Find(IdentityTag))
		{
			for (const auto& Modifier : ModifierArray->Array)
			{
				bool bIsMatch;

				if (Modifier.bExactMatch)
				{
					bIsMatch = IdentityTags.HasTagExact(Modifier.IdentityTag);
				}
				else
				{
					bIsMatch = IdentityTags.HasTag(Modifier.IdentityTag);
				}

				if (bIsMatch)
				{
					IdentityInfo += Modifier.IdentityInfo;
				}
			}
		}
	}
}

void UNipcatFlowSubsystem::GetCombinedIdentityInfoByGuid(FGuid FlowComponentGuid, FNipcatFlowIdentityInfo& IdentityInfo)
{
	if (const auto ModifierArray = IdentityInfoModifiersFromGuid.Find(FlowComponentGuid))
	{
		for (const auto& Modifier : ModifierArray->Array)
		{
			IdentityInfo += Modifier.IdentityInfo;
		}
	}
}

FNipcatFlowIdentityInfo UNipcatFlowSubsystem::GetCombinedIdentityInfo(UNipcatFlowComponent* FlowComponent)
{
	FNipcatFlowIdentityInfo ResultInfo;
	
	if (FlowComponent)
	{
		if (FlowComponent->IdentityTags.IsValid())
		{
			GetCombinedIdentityInfoByTags(FlowComponent->IdentityTags, ResultInfo);
		}
		GetCombinedIdentityInfoByGuid(FlowComponent->Guid, ResultInfo);
	}
	
	return ResultInfo;
}


void UNipcatFlowSubsystem::RefreshIdentityInfoByTagsImmediately(FGameplayTag ChangedIdentity, bool bExactMatch)
{
	for (auto [Tag, Component] : FlowComponentRegistry)
	{
		if (auto Comp = Cast<UNipcatFlowComponent>(Component))
		{
			if (bExactMatch)
			{
				if (Comp->IdentityTags.HasTagExact(ChangedIdentity))
				{
					Comp->RefreshFlowIdentityInfo();
				}
			}
			else
			{
				if (Comp->IdentityTags.HasTag(ChangedIdentity))
				{
					Comp->RefreshFlowIdentityInfo();
				}
			}
		}
	}
}


void UNipcatFlowSubsystem::RefreshIdentityInfoByGuidImmediately(FGuid ChangedGuid)
{
	if (const auto Comp = FlowComponentGuidRegistry.Find(ChangedGuid))
	{
		Comp->Get()->RefreshFlowIdentityInfo();
	}
}