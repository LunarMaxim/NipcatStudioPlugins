// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NipcatFlowComponentOwnerInterface.generated.h"

struct FGameplayTag;
class UNipcatFlowComponent;
// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType, MinimalAPI)
class UNipcatFlowComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATFLOW_API INipcatFlowComponentOwnerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	UNipcatFlowComponent* GetNipcatFlowComponent(UPARAM(meta=(GameplayTagFilter="Flow.ComponentType")) FGameplayTag FlowComponentType);
};
