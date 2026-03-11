// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "NipcatTeamActorConfigInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UNipcatTeamActorConfigInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATBASICGAMEPLAY_API INipcatTeamActorConfigInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Nipcat Basic Gameplay|Team")
	FGameplayTag GetDefaultTeamTag();
};
