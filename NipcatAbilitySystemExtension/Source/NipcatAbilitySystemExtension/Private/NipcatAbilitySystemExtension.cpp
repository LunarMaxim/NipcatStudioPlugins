// Copyright Epic Games, Inc. All Rights Reserved.

#include "NipcatAbilitySystemExtension.h"

#include "GameplayTagsManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FNipcatAbilitySystemExtensionModule"

void FNipcatAbilitySystemExtensionModule::StartupModule()
{
	if (const auto ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("NipcatAbilitySystemExtension")))
	{
		UGameplayTagsManager::Get().AddTagIniSearchPath(ThisPlugin->GetBaseDir() / TEXT("Config/Tags"));
	}
}

void FNipcatAbilitySystemExtensionModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FNipcatAbilitySystemExtensionModule, NipcatAbilitySystemExtension)