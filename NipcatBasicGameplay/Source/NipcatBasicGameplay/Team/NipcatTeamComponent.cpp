// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatTeamComponent.h"

#include "NipcatTeamComponentOwnerInterface.h"
#include "NipcatTeamUtils.h"

FNipcatTeamDefinition UNipcatTeamConfig::FindTeamDefinitionByGenericTeamId(FGenericTeamId Id) const
{
	for (const FNipcatTeamDefinition& TeamDefinition : TeamDefinitions)
	{
		if (TeamDefinition.TeamId == Id)
		{
			return TeamDefinition;
		}
	}
	return FNipcatTeamDefinition();
}

FNipcatTeamDefinition UNipcatTeamConfig::FindTeamDefinitionByTag(FGameplayTag TeamTag) const
{
	for (const FNipcatTeamDefinition& TeamDefinition : TeamDefinitions)
	{
		if (TeamDefinition.TeamTag == TeamTag)
		{
			return TeamDefinition;
		}
	}
	return FNipcatTeamDefinition();
}

ETeamAttitude::Type UNipcatTeamConfig::GetTeamAttitudeTowards(FGameplayTag SourceTeamTag, FGameplayTag TargetTeamTag) const
{
	const FNipcatTeamDefinition SourceTeamDefinition = FindTeamDefinitionByTag(SourceTeamTag);
	const FNipcatTeamDefinition TargetTeamDefinition = FindTeamDefinitionByTag(TargetTeamTag);

	if (!SourceTeamDefinition.TeamTag.IsValid() || !TargetTeamDefinition.TeamTag.IsValid()) { return ETeamAttitude::Type::Neutral; }

	if (const TEnumAsByte<ETeamAttitude::Type>* Attitude = SourceTeamDefinition.AttitudeTowards.Find(TargetTeamDefinition.TeamTag))
	{
		return *Attitude;
	}
	UE_LOG(LogTemp, Error, TEXT("Could Not Get Team Attitude From %s Towards %s in NipcatTeamConfig"), *SourceTeamTag.ToString(), *TargetTeamTag.ToString());
	return ETeamAttitude::Type::Neutral;
}

void UNipcatTeamComponent::BeginPlay()
{
	if (GetOwner()->GetClass()->ImplementsInterface(UNipcatTeamComponentOwnerInterface::StaticClass()))
	{
		if (UObject* TeamActorConfig = INipcatTeamComponentOwnerInterface::Execute_GetNipcatTeamActorConfig(GetOwner()).GetObject())
		{
			if (TeamActorConfig->GetClass()->ImplementsInterface(UNipcatTeamActorConfigInterface::StaticClass()))
			{
				DefaultTeamTag = INipcatTeamActorConfigInterface::Execute_GetDefaultTeamTag(TeamActorConfig);
			}
		}
	}
	Super::BeginPlay();
}


void UNipcatTeamComponent::SetActorAttitudeOverride(AActor* Actor, ETeamAttitude::Type NewAttitude, bool Bidirectional)
{
	AActor* ActualTargetActor = Actor;
	if (const AController* Controller = Cast<AController>(Actor))
	{
		ActualTargetActor = Controller->GetPawn();
	}
	
	ActorAttitudeOverride.Add(ActualTargetActor, NewAttitude);
	if (Bidirectional)
	{
		if (ActualTargetActor->GetClass()->ImplementsInterface(UNipcatTeamComponentOwnerInterface::StaticClass()))
		{
			if (UNipcatTeamComponent* TargetTeamComponent = INipcatTeamComponentOwnerInterface::Execute_GetNipcatTeamComponent(ActualTargetActor))
			{
				TargetTeamComponent->SetActorAttitudeOverride(GetOwner(), NewAttitude, false);
			}
		}
	}
}


ETeamAttitude::Type UNipcatTeamComponent::GetTeamAttitudeTowards(const AActor* Actor)
{
	DebugLog(FString::Printf(TEXT("%s Try Get Team Attitude Towards %s"), *AActor::GetDebugName(GetOwner()), *AActor::GetDebugName(Actor)), bShowDebug);
	const AActor* ActualTargetActor = Actor;
	if (const AController* Controller = Cast<AController>(Actor))
	{
		if (const APawn* Pawn = Controller->GetPawn())
		{
			ActualTargetActor = Pawn;
		}
	}

	if (const TEnumAsByte<ETeamAttitude::Type>* Attitude = ActorAttitudeOverride.Find(ActualTargetActor))
	{
		DebugLog(FString::Printf(TEXT("Found Attitude in ActorAttitudeOverride as %s"), *StaticEnum<ETeamAttitude::Type>()->GetDisplayNameTextByValue(*Attitude).ToString()), bShowDebug);
		return *Attitude;
	}
	
	if (const UNipcatTeamConfig* TeamConfig = UNipcatTeamUtils::GetNipcatTeamConfig())
	{
		if (!ActualTargetActor)
		{
			DebugLog(FString::Printf(TEXT("Actor Invalid, Returning")), bShowDebug);
			return ETeamAttitude::Type::Neutral;
		}
		const UClass* TargetActorClass = ActualTargetActor->GetClass();
		if (TargetActorClass && TargetActorClass->ImplementsInterface(UNipcatTeamComponentOwnerInterface::StaticClass()))
		{
			if (const UNipcatTeamComponent* TargetTeamComponent = INipcatTeamComponentOwnerInterface::Execute_GetNipcatTeamComponent(ActualTargetActor))
			{
				const FGameplayTag TargetTeamTag = TargetTeamComponent->GetCurrentTeamTag();
				DebugLog(FString::Printf(TEXT("Source Team Tag is %s, Target Team Tag is %s"), *GetCurrentTeamTag().ToString(), *TargetTeamTag.ToString()), bShowDebug);

				const ETeamAttitude::Type Result = TeamConfig->GetTeamAttitudeTowards(GetCurrentTeamTag(), TargetTeamTag);
				DebugLog(FString::Printf(TEXT("Found Attitude as %s"), *StaticEnum<ETeamAttitude::Type>()->GetDisplayNameTextByValue(Result).ToString()), bShowDebug);

				return Result;
			}
			else
			{
				DebugLog(FString::Printf(TEXT("Target Actor %s Missing UNipcatTeamComponent"), *AActor::GetDebugName(ActualTargetActor)), bShowDebug);
			}
		}
		else
		{
			DebugLog(FString::Printf(TEXT("Target Actor %s Missing INipcatTeamComponentHolderInterface"), *AActor::GetDebugName(ActualTargetActor)), bShowDebug);
		}
	}
	else
	{
		DebugLog(FString::Printf(TEXT("Could Not Found NipcatTeamConfig")), bShowDebug);
	}
	
	return ETeamAttitude::Type::Neutral;
}

void UNipcatTeamComponent::DebugLog(const FString& DebugMessage, bool EnableDebug,  float TimeToDisplay, FColor Color)
{
	if (EnableDebug)
	{
		const FString& Message = (TEXT("%s: " ), __FUNCTION__) + DebugMessage;
		GEngine->AddOnScreenDebugMessage(-1, TimeToDisplay, Color, Message);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Message);
	}
}

FGameplayTag UNipcatTeamComponent::GetCurrentTeamTag() const
{
	return OverrideTeamTag.IsValid() ? OverrideTeamTag : DefaultTeamTag;
}

void UNipcatTeamComponent::OverrideCurrentTeamTag(FGameplayTag InTag)
{
	OverrideTeamTag = InTag;
}

void UNipcatTeamComponent::OverrideCurrentTeamTagByGenericTeamId(FGenericTeamId Id)
{
	if (const UNipcatTeamConfig* TeamConfig = UNipcatTeamUtils::GetNipcatTeamConfig())
	{
		const FNipcatTeamDefinition TeamDefinition = TeamConfig->FindTeamDefinitionByGenericTeamId(Id);
		DefaultTeamTag = TeamDefinition.TeamTag;
	}
}

FGenericTeamId UNipcatTeamComponent::GetCurrentGenericTeamId() const
{
	if (GetCurrentTeamTag().IsValid())
	{
		if (const UNipcatTeamConfig* TeamConfig = UNipcatTeamUtils::GetNipcatTeamConfig())
		{
			const FNipcatTeamDefinition TeamDefinition = TeamConfig->FindTeamDefinitionByTag(GetCurrentTeamTag());
			return TeamDefinition.TeamId;
		}
	}
	return FGenericTeamId();
}
