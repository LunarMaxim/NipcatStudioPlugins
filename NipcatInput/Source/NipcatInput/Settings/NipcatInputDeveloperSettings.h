// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NipcatInput/Types/NipcatInputTypes.h"
#include "NipcatInputDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config = NipcatInput, DisplayName = "NipcatInput", DefaultConfig, BlueprintType)
class NIPCATINPUT_API UNipcatInputDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<TSoftObjectPtr<UInputMappingContext>, FNipcatInputMappingContextSetting> InputMappingContexts;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<TSoftObjectPtr<UInputMappingContext>, FNipcatInputMappingContextSetting> InputMappingContexts_Keyboard;
	
	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, meta=(ForceInlineRow))
	TMap<TSoftObjectPtr<UInputMappingContext>, FNipcatInputMappingContextSetting> InputMappingContexts_Gamepad;

	/** Gets the settings container name for the settings, either Project or Editor */
	virtual FName GetContainerName() const override { return FName("Project"); }
	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override { return FName("NipcatStudio"); }
	/** The unique name for your section of settings, uses the class's FName. */
	virtual FName GetSectionName() const override { return FName("Input"); }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="NipcatInput|DeveloperSettings", DisplayName="GetNipcatInputGlobalConfig")
	static const UNipcatInputDeveloperSettings* Get()
	{
		return GetDefault<UNipcatInputDeveloperSettings>();
	}
};
