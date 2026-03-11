// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAbilitySystemActorConfigInterface.h"
#include "UObject/Interface.h"
#include "NipcatAbilitySystemComponentOwnerInterface.generated.h"

UINTERFACE(MinimalAPI)
class UNipcatAbilitySystemComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATABILITYSYSTEM_API INipcatAbilitySystemComponentOwnerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Nipcat Ability System", meta=(CallableWithoutWorldContext))
	TScriptInterface<INipcatAbilitySystemActorConfigInterface> GetAbilitySystemActorConfig();
};
