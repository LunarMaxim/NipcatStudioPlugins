// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAbilitySystem/EffectContext/NipcatGameplayEffectContext.h"
#include "UObject/Interface.h"
#include "NipcatDamageNotifyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType, MinimalAPI)
class UNipcatDamageNotifyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATABILITYSYSTEM_API INipcatDamageNotifyInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FDamageDefinitionHandle GetDamageDefinitionFromNotify();
};
