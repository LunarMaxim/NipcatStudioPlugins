// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NipcatQuestComponentOwnerInterface.generated.h"

class UNipcatQuestComponent;
// This class does not need to be modified.
UINTERFACE(BlueprintType)
class UNipcatQuestComponentOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATFLOW_API INipcatQuestComponentOwnerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Nipcat Quest", meta=(CallableWithoutWorldContext))
	UNipcatQuestComponent* GetNipcatQuestComponent() const;
};
