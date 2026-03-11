// Copyright Nipcat Studio All Rights Reserved.

#include "NipcatTeamUtils.h"

#include "NipcatTeamComponent.h"
#include "NipcatBasicGameplay/Settings/NipcatTeamDeveloperSettings.h"

ETeamAttitude::Type UNipcatTeamUtils::GetTeamAttitudeTowards(AActor* Target, AActor* Other)
{
	if (const IGenericTeamAgentInterface* AsTeamAgentInterface = Cast<IGenericTeamAgentInterface>(Target))
	{
		return AsTeamAgentInterface->GetTeamAttitudeTowards(*Other);
	}
	return ETeamAttitude::Neutral;
}

const UNipcatTeamConfig* UNipcatTeamUtils::GetNipcatTeamConfig()
{
	return Cast<UNipcatTeamConfig>(GetDefault<UNipcatTeamDeveloperSettings>()->TeamConfigPath.TryLoad());
}

FNipcatTeamDefinition UNipcatTeamUtils::FindTeamDefinitionByGenericTeamId(FGenericTeamId Id)
{
	if (const UNipcatTeamConfig* TeamConfig = GetNipcatTeamConfig())
	{
		return TeamConfig->FindTeamDefinitionByGenericTeamId(Id);
	}
	return FNipcatTeamDefinition();
}


FNipcatTeamDefinition UNipcatTeamUtils::FindTeamDefinitionByTag(FGameplayTag TeamTag)
{
	if (const UNipcatTeamConfig* TeamConfig = GetNipcatTeamConfig())
	{
		return TeamConfig->FindTeamDefinitionByTag(TeamTag);
	}
	return FNipcatTeamDefinition();
}
