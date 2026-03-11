// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NipcatComboComponentOwnerInterface.generated.h"

class UNipcatComboComponent;
class INipcatComboActorConfigInterface;
// This class does not need to be modified.
UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UNipcatComboComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATABILITYSYSTEM_API INipcatComboComponentOwnerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Nipcat Ability System|Combo", meta=(CallableWithoutWorldContext))
	TScriptInterface<INipcatComboActorConfigInterface> GetNipcatComboActorConfig() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Nipcat Ability System|Combo", meta=(CallableWithoutWorldContext))
	UNipcatComboComponent* GetNipcatComboComponent() const;
};
