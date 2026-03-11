// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NipcatTeamUtils.generated.h"

class UNipcatTeamConfig;
struct FNipcatTeamDefinition;
/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatTeamUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Basic Gameplay|Team", meta=(DefaultToSelf = "Target"))
	static ETeamAttitude::Type GetTeamAttitudeTowards(AActor* Target, AActor* Other);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Nipcat Basic Gameplay|Team")
	static const UNipcatTeamConfig* GetNipcatTeamConfig();
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Basic Gameplay|Team")
	static FNipcatTeamDefinition FindTeamDefinitionByGenericTeamId(FGenericTeamId Id);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Basic Gameplay|Team", meta=(GameplayTagFilter="Team"))
	static FNipcatTeamDefinition FindTeamDefinitionByTag(FGameplayTag TeamTag);
	
};
