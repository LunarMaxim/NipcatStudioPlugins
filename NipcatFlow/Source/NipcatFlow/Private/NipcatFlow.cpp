// Copyright Epic Games, Inc. All Rights Reserved.

#include "NipcatFlow.h"

#include "GameplayTagsManager.h"
#include "GameFramework/CheatManager.h"
#include "Interfaces/IPluginManager.h"
#include "NipcatFlow/Quest/NipcatQuestCheatManagerExtension.h"

#define LOCTEXT_NAMESPACE "FNipcatFlowModule"

void FNipcatFlowModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	// Add "Projects" and "GameplayTags" to PrivateDependencyModuleNames in your plugin's Build.cs
	if (const auto ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("NipcatFlow")))
	{
		UGameplayTagsManager::Get().AddTagIniSearchPath(ThisPlugin->GetBaseDir() / TEXT("Config/Tags"));
	}
}

void FNipcatFlowModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNipcatFlowModule, NipcatFlow)