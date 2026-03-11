// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NipcatBasicGameplayCharacterInterface.generated.h"

class INipcatActorConfigInterface;
// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable, MinimalAPI)
class UNipcatBasicGameplayCharacterInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATBASICGAMEPLAY_API INipcatBasicGameplayCharacterInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Nipcat Basic Gameplay")
	AActor* NipcatBasicGameplay_GetTargetActor() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Nipcat Basic Gameplay")
	FVector NipcatBasicGameplay_GetTargetLocation() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Nipcat Basic Gameplay")
	USkeletalMeshComponent* NipcatBasicGameplay_GetMainMesh() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Nipcat Basic Gameplay")
	UMotionWarpingComponent* NipcatBasicGameplay_GetMotionWarpingComponent() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Nipcat Basic Gameplay")
	TArray<UPrimitiveComponent*> GetAdditionalColliders();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Nipcat Basic Gameplay")
	TScriptInterface<INipcatActorConfigInterface> GetActorConfig() const;
};
