// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatActorConfigInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UNipcatActorConfigInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATBASICGAMEPLAY_API INipcatActorConfigInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Nipcat Basic Gameplay|ActorConfig")
	FText GetCharacterName() const;
};
