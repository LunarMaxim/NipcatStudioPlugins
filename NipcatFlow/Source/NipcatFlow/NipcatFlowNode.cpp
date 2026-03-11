// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatFlowNode.h"

#include "NipcatFlowFunctionLibrary.h"


#if WITH_EDITOR
void UNipcatFlowNode::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	BlueprintPostEditChangeProperty(PropertyChangedEvent.GetPropertyName(), PropertyChangedEvent.GetMemberPropertyName());
}

bool UNipcatFlowNode::CanEditChange(const FEditPropertyChain& PropertyChain) const
{
	bool BlueprintCanEdit = true;
	if (GetClass()->IsFunctionImplementedInScript(GET_FUNCTION_NAME_CHECKED(ThisClass, BlueprintCanEditChange)))
	{
		BlueprintCanEdit = BlueprintCanEditChange(PropertyChain.GetActiveNode()->GetValue()->GetFName(), PropertyChain.GetActiveMemberNode()->GetValue()->GetFName());
	}
	return Super::CanEditChange(PropertyChain) && BlueprintCanEdit;
}

#endif


void UNipcatFlowNode::OnRegister()
{
	BP_OnRegister();
}

void UNipcatFlowNode::OnUnregister()
{
	BP_OnUnregister();
}

EFlowSignalMode UNipcatFlowNode::AccessSignalMode(UFlowNode* Node)
{
	if (Node)
	{
		static_cast<UNipcatFlowNode*>(Node)->SignalMode;
	}
	return EFlowSignalMode::Disabled;
}
