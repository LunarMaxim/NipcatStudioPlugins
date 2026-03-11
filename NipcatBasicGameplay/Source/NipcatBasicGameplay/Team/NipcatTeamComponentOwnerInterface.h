// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatTeamActorConfigInterface.h"
#include "NipcatTeamComponent.h"
#include "UObject/Interface.h"
#include "NipcatTeamComponentOwnerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UNipcatTeamComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATBASICGAMEPLAY_API INipcatTeamComponentOwnerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Nipcat Basic Gameplay|Team")
	UNipcatTeamComponent* GetNipcatTeamComponent() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Nipcat Basic Gameplay|Team")
	TScriptInterface<INipcatTeamActorConfigInterface> GetNipcatTeamActorConfig() const;
};
