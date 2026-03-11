// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "NipcatBasicGameplayEditorPerUserSettings.generated.h"

/**
 * 
 */
UCLASS(Config=EditorPerProjectUserSettings, DisplayName="NipcatBasicGameplayEditor", BlueprintType)
class NIPCATBASICGAMEPLAYEDITOR_API UNipcatBasicGameplayEditorPerUserSettings : public UDeveloperSettings
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "SaveGame", DisplayName="ClearSaveGame")
	bool bClearSaveOnPIE = true;

	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "DefaultGameplayTags", meta=(InlineEditConditionToggle))
	bool bEnableDefaultGameplayTags = true;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category = "DefaultGameplayTags", meta=(EditCondition="bEnableDefaultGameplayTags", ForceInlineRow), DisplayName="Tags")
	FGameplayTagContainer DefaultGameplayTags;
	
	
	UFUNCTION(BlueprintPure, Category="Nipcat Basic Gameplay|DeveloperSettings", DisplayName="GetNipcatBasicGameplayEditorPerUserSettings")
	static UNipcatBasicGameplayEditorPerUserSettings* Get()
	{
		return GetMutableDefault<UNipcatBasicGameplayEditorPerUserSettings>();
	}
};
