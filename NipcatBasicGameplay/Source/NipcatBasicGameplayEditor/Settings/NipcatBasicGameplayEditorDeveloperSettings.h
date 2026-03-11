// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "NipcatBasicGameplayEditorDeveloperSettings.generated.h"

class UEditorUtilityWidgetBlueprint;
class UNipcatAssetCreateData;
/**
 * 
 */
UCLASS(Config=NipcatBasicGameplayEditor, DisplayName="NipcatBasicGameplayEditor", DefaultConfig, BlueprintType)
class NIPCATBASICGAMEPLAYEDITOR_API UNipcatBasicGameplayEditorDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	
	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category = "EditorDebug")
	TSoftObjectPtr<UEditorUtilityWidgetBlueprint> EditorDebugWidget;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="AssetCreation", meta=(Categories="Editor.AssetCreateData", ForceInlineRow))
    TMap<FGameplayTag, TSoftClassPtr<UNipcatAssetCreateData>> AssetCreateData;

	/** Gets the settings container name for the settings, either Project or Editor */
	virtual FName GetContainerName() const override { return FName("Project"); }
	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override { return FName("NipcatStudio"); }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Nipcat Basic Gameplay|DeveloperSettings", DisplayName="GetNipcatBasicGameplayEditorGlobalConfig")
	static const UNipcatBasicGameplayEditorDeveloperSettings* Get()
	{
		return GetDefault<UNipcatBasicGameplayEditorDeveloperSettings>();
	}
};
