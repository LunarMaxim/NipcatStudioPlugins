// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatFlowComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AIController.h"
#include "NipcatFlowSubsystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "GameFramework/PlayerState.h"
#include "NipcatBasicGameplay/Team/NipcatTeamComponentOwnerInterface.h"
#include "NipcatBasicGameplay/Team/NipcatTeamComponent.h"
#include "Perception/AIPerceptionComponent.h"

void UNipcatFlowComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (UFlowSubsystem* FlowSubsystem = GetFlowSubsystem())
	{
		if (const auto NipcatFlowSubsystem = Cast<UNipcatFlowSubsystem>(FlowSubsystem))
		{
			NipcatFlowSubsystem->RegisterComponentByGuid(this);
		}
	}
	
	RefreshFlowIdentityInfo();
}

void UNipcatFlowComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UFlowSubsystem* FlowSubsystem = GetFlowSubsystem())
	{
		if (const auto NipcatFlowSubsystem = Cast<UNipcatFlowSubsystem>(FlowSubsystem))
		{
			NipcatFlowSubsystem->UnregisterComponentByGuid(this);
		}
	}
	
	Super::EndPlay(EndPlayReason);
}

void UNipcatFlowComponent::ApplyFlowIdentityInfo(FNipcatFlowIdentityInfo& NewInfo)
{
	AActor* AvatarActor = GetOwner();
	
	if (const auto Controller = Cast<AController>(AvatarActor))
	{
		AvatarActor = Controller->GetPawn();
	}
	else if (const auto PlayerState = Cast<APlayerState>(AvatarActor))
	{
		AvatarActor = PlayerState->GetPawn();
	}

	if (AvatarActor)
	{
		if (const auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(AvatarActor))
		{
			TSet<FGameplayTag> OwningTagsSet;
			OwningTagsSet.Append(ASC->GetOwnedGameplayTags().GetGameplayTagArray());
			TSet<FGameplayTag> RemovingTagsSet;
			RemovingTagsSet.Append(NewInfo.RemovedTags.GetGameplayTagArray());
			
			NewInfo.RemovedTags.RemoveTags(FGameplayTagContainer::CreateFromArray(RemovingTagsSet.Difference(OwningTagsSet).Array()));
			
			ASC->RemoveLooseGameplayTags(CurrentInfo.AddedTags);
			ASC->AddLooseGameplayTags(CurrentInfo.RemovedTags);
			ASC->AddLooseGameplayTags(NewInfo.AddedTags);
			ASC->RemoveLooseGameplayTags(NewInfo.RemovedTags);
		}

		if (const auto TeamComponent = INipcatTeamComponentOwnerInterface::Execute_GetNipcatTeamComponent(AvatarActor))
		{
			TeamComponent->OverrideCurrentTeamTag(NewInfo.TeamTag);
			if (const auto AIController = UAIBlueprintHelperLibrary::GetAIController(AvatarActor))
			{
				if (AIController->PerceptionComponent)
				{
					AIController->PerceptionComponent->RequestStimuliListenerUpdate();
				}
			}
		}
	}
	
	CurrentInfo = NewInfo;
}

void UNipcatFlowComponent::RefreshFlowIdentityInfo()
{
	if (const auto NipcatFlowSubsystem = Cast<UNipcatFlowSubsystem>(GetFlowSubsystem()))
	{
		FNipcatFlowIdentityInfo NewInfo = NipcatFlowSubsystem->GetCombinedIdentityInfo(this);
		ApplyFlowIdentityInfo(NewInfo);
	}
}

void UNipcatFlowComponent::PostLoad()
{
	Super::PostLoad();
	if (!HasAllFlags(RF_ClassDefaultObject))
	{
		Guid = FGuid::NewGuid();
	}
}
