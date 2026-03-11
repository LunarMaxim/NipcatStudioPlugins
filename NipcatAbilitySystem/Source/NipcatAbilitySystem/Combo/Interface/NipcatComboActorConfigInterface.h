// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NipcatComboActorConfigInterface.generated.h"

class UNipcatComboDefinitionConfig;
class UNipcatComboGraph;

UINTERFACE(MinimalAPI, BlueprintType, Blueprintable)
class UNipcatComboActorConfigInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATABILITYSYSTEM_API INipcatComboActorConfigInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Nipcat Ability System|Combo", meta=(CallableWithoutWorldContext))
	TArray<UNipcatComboGraph*> GetDefaultComboGraphs() const;
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="Nipcat Ability System|Combo", meta=(CallableWithoutWorldContext))
	TArray<UNipcatComboDefinitionConfig*> GetDefaultComboDefinitions() const;
};
