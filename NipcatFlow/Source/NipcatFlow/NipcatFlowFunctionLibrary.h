// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlowTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NipcatFlowFunctionLibrary.generated.h"

struct FFlowPin;
class UFlowNode;
/**
 * 
 */
UCLASS()
class NIPCATFLOW_API UNipcatFlowFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf=FlowNode, ScriptMethod))
	static EFlowSignalMode GetSignalMode(UFlowNode* FlowNode);
	
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf=FlowNode, ScriptMethod))
	static UFlowNode* GetConnectedNode(UFlowNode* FlowNode, FName OutputName);
	
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf=FlowNode, ScriptMethod))
	static void RequestReconstruction(UFlowNode* FlowNode);
	
	UFUNCTION(BlueprintCallable, meta=(DefaultToSelf=FlowNode, ScriptMethod))
	static void TriggerOutputWithIndex(UFlowNode* FlowNode, int32 Index, bool bFinish);
	
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf=FlowNode, ScriptMethod))
	static bool IsInputConnectedAtIndex(UFlowNode* FlowNode, int32 Index);
	
	UFUNCTION(BlueprintPure, meta=(DefaultToSelf=FlowNode, ScriptMethod))
	static bool IsOutputConnectedAtIndex(UFlowNode* FlowNode, int32 Index);

};
