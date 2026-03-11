// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatBasicGameplayDeveloperSettings.h"

#include "NipcatBasicGameplay/AssetUserData/NipcatAssetUserDataMap.h"

UNipcatBasicGameplayDeveloperSettings::UNipcatBasicGameplayDeveloperSettings()
{
	HitBoxTraceChannel = TraceTypeQuery2;
	FootIKTraceChannel = TraceTypeQuery1;
}

UNipcatAssetUserDataMap* UNipcatBasicGameplayDeveloperSettings::GetNipcatAssetUserDataMap()
{
	if (!Get()->AssetUserDataMap.IsNull())
	{
		if (const auto Result = Get()->AssetUserDataMap.TryLoad())
		{
			return Cast<UNipcatAssetUserDataMap>(Result);
		}
	}
	return nullptr;
}
