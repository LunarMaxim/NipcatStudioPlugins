// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatLinkedAnimNotify.h"
#include "UObject/Interface.h"
#include "NipcatLinkableAnimNotifyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UNipcatLinkableAnimNotifyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATABILITYSYSTEM_API INipcatLinkableAnimNotifyInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	TArray<FNipcatLinkedAnimNotify> GetLinkedAnimNotifies();
};
