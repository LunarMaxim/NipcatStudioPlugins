// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DeveloperSettings.h"
#include "NipcatBasicGameplay/UI/IndicatorSystem/IndicatorDescriptor.h"
#include "NipcatBasicGameplayDeveloperSettings.generated.h"

class UNipcatAssetUserDataMap;
/**
 * 
 */
UCLASS(Config=NipcatBasicGameplay, DisplayName="NipcatBasicGameplay", DefaultConfig, BlueprintType)
class NIPCATBASICGAMEPLAY_API UNipcatBasicGameplayDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UNipcatBasicGameplayDeveloperSettings();

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Channels")
	TEnumAsByte<ETraceTypeQuery> HitBoxTraceChannel;
	
	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="Channels")
	TEnumAsByte<ETraceTypeQuery> FootIKTraceChannel;

	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="UI", meta=(Categories="Indicator", ForceInlineRow))
	TMap<FGameplayTag, FNipcatIndicatorConfig> IndicatorConfig;
	
	UPROPERTY(Config, BlueprintReadOnly, EditDefaultsOnly, Category="AsserUserData", meta=(AllowedClasses="/Script/NipcatBasicGameplay.NipcatAssetUserDataMap"))
	FSoftObjectPath AssetUserDataMap;
	
	UPROPERTY(Config, EditAnywhere, BlueprintReadWrite, Category="CustomMovement", meta=(ForceInlineRow, Categories="MovementMode.Custom"))
	TMap<uint8, FGameplayTag> CustomMovementModeDefinitions;
	
	/** Gets the settings container name for the settings, either Project or Editor */
	virtual FName GetContainerName() const override { return FName("Project"); }
	/** Gets the category for the settings, some high level grouping like, Editor, Engine, Game...etc. */
	virtual FName GetCategoryName() const override { return FName("NipcatStudio"); }
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Nipcat Basic Gameplay|DeveloperSettings", DisplayName="GetNipcatBasicGameplayGlobalConfig")
	static const UNipcatBasicGameplayDeveloperSettings* Get()
	{
		return GetDefault<UNipcatBasicGameplayDeveloperSettings>();
	}
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Nipcat Basic Gameplay|DeveloperSettings")
	static UNipcatAssetUserDataMap* GetNipcatAssetUserDataMap();
};
