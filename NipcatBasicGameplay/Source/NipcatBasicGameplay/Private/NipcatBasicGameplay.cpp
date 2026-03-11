// Copyright Epic Games, Inc. All Rights Reserved.

#include "NipcatBasicGameplay.h"

#include "GameplayTagsManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FNipcatBasicGameplayModule"

void FNipcatBasicGameplayModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// Add "Projects" and "GameplayTags" to PrivateDependencyModuleNames in your plugin's Build.cs
	if (const auto ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("NipcatBasicGameplay")))
	{
		UGameplayTagsManager::Get().AddTagIniSearchPath(ThisPlugin->GetBaseDir() / TEXT("Config/Tags"));
	}
}

void FNipcatBasicGameplayModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNipcatBasicGameplayModule, NipcatBasicGameplay)