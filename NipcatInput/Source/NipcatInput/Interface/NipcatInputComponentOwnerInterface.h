// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatInputComponentOwnerInterface.generated.h"

class UNipcatInputComponent;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, NotBlueprintable)
class UNipcatInputComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATINPUT_API INipcatInputComponentOwnerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="NipcatInput", meta=(CallableWithoutWorldContext))
	virtual UNipcatInputComponent* GetNipcatInputComponent() const = 0;
};
