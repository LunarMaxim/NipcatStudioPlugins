// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "NipcatTeamDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config=NipcatTeam, DisplayName="NipcatTeam", DefaultConfig, BlueprintType)
class NIPCATBASICGAMEPLAY_API UNipcatTeamDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UNipcatTeamDeveloperSettings();

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Team", meta=(AllowedClasses="/Script/NipcatBasicGameplay.NipcatTeamConfig"))
	FSoftObjectPath TeamConfigPath;

	/** Gets the settings container name for the settings, either Project or Editor */
	virtual FName GetContainerName() const override { return FName("Project"); }
	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override { return FName("NipcatStudio"); }
	/** The unique name for your section of settings, uses the class's FName. */
	virtual FName GetSectionName() const override { return FName("Team"); }
	
};
