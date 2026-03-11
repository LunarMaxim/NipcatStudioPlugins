// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Nodes/FlowNode.h"
#include "NipcatFlowNode.generated.h"

/**
 * 
 */
UCLASS(Abstract, CollapseCategories)
class NIPCATFLOW_API UNipcatFlowNode : public UFlowNode
{
	GENERATED_BODY()
	
public:
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual bool CanEditChange(const FEditPropertyChain& PropertyChain) const override;
#endif

	UFUNCTION(BlueprintImplementableEvent, CallInEditor)
	void BlueprintPostEditChangeProperty(FName PropertyName, FName MemberPropertyName);

	UFUNCTION(BlueprintImplementableEvent, CallInEditor)
	bool BlueprintCanEditChange(FName PropertyName, FName MemberPropertyName) const;
	
	UFUNCTION()
	virtual void OnRegister();
	
	UFUNCTION()
	virtual void OnUnregister();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnRegister")
	void BP_OnRegister();

	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnUnregister")
	void BP_OnUnregister();
	
	static EFlowSignalMode AccessSignalMode(UFlowNode* Node);

};
