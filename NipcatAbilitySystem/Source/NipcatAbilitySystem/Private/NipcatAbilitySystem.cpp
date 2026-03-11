// Copyright Nipcat Studio All Rights Reserved.

#include "NipcatAbilitySystem.h"

#include "GameplayTagsManager.h"
#include "Interfaces/IPluginManager.h"

#define LOCTEXT_NAMESPACE "FNipcatAbilitySystemModule"

void FNipcatAbilitySystemModule::StartupModule()
{
	// Add "Projects" and "GameplayTags" to PrivateDependencyModuleNames in your plugin's Build.cs
	if (const auto ThisPlugin = IPluginManager::Get().FindPlugin(TEXT("NipcatAbilitySystem")))
	{
		UGameplayTagsManager::Get().AddTagIniSearchPath(ThisPlugin->GetBaseDir() / TEXT("Config/Tags"));
	}
}

void FNipcatAbilitySystemModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

bool FNipcatAbilitySystemModule::IsGameModule() const
{
	return true;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNipcatAbilitySystemModule, NipcatAbilitySystem)
