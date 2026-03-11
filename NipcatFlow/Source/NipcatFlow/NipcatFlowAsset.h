// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"
#include "NipcatFlowAsset.generated.h"


/**
 * 
 */
UCLASS()
class NIPCATFLOW_API UNipcatFlowAsset : public UFlowAsset
{
	GENERATED_BODY()
	
public:
	virtual void StartFlow(IFlowDataPinValueSupplierInterface* DataPinValueSupplier = nullptr) override;
	virtual void FinishFlow(const EFlowFinishPolicy InFinishPolicy, const bool bRemoveInstance = true) override;
	
	virtual void RegisterNode(const FGuid& NewGuid, UFlowNode* NewNode) override;
	virtual void UnregisterNode(const FGuid& NodeGuid) override;
	
	UFUNCTION()
	virtual void OnFlowStart() {};
	
	UFUNCTION()
	virtual void OnFlowFinish() {};
};

