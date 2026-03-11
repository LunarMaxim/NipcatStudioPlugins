// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NipcatFlowOwnerInterface.generated.h"

enum class EFlowFinishPolicy : uint8;
// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType, MinimalAPI)
class UNipcatFlowOwnerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATFLOW_API INipcatFlowOwnerInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION()
	virtual void OnFlowStart();
	
	UFUNCTION()
	virtual void OnFlowFinish(const EFlowFinishPolicy& FinishPolicy);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnFlowStart")
	void BP_OnFlowStart();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnFlowFinish")
	void BP_OnFlowFinish(const EFlowFinishPolicy& FinishPolicy);
};
