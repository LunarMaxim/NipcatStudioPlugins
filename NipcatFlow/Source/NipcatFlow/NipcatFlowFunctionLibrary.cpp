// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatFlowFunctionLibrary.h"

#if WITH_EDITOR
#include "Graph/FlowGraph.h"
#endif

#include "NipcatFlowNode.h"
#include "Nodes/FlowNode.h"

EFlowSignalMode UNipcatFlowFunctionLibrary::GetSignalMode(UFlowNode* FlowNode)
{
	return UNipcatFlowNode::AccessSignalMode(FlowNode);
}

UFlowNode* UNipcatFlowFunctionLibrary::GetConnectedNode(UFlowNode* FlowNode, FName OutputName)
{
	if (FlowNode)
	{
		return FlowNode->GetFlowAsset()->GetNode(FlowNode->GetConnection(OutputName).NodeGuid);
	}
	return nullptr;
}

void UNipcatFlowFunctionLibrary::RequestReconstruction(UFlowNode* FlowNode)
{
#if WITH_EDITOR
	if (FlowNode)
	{
		FlowNode->RequestReconstruction();
		if (const auto FlowAsset = FlowNode->GetFlowAsset())
		{
			if (const auto Graph = FlowAsset->GetGraph())
			{
				CastChecked<UFlowGraph>(Graph)->RefreshGraph();
			}
		}
	}
#endif
}

void UNipcatFlowFunctionLibrary::TriggerOutputWithIndex(UFlowNode* FlowNode, int32 Index, bool bFinish)
{
	if (FlowNode)
	{
		const auto& OutputPins = FlowNode->GetOutputPins();
		if (OutputPins.IsValidIndex(Index))
		{
			FlowNode->TriggerOutput(OutputPins[Index].PinName, bFinish);
		}
	}
}

bool UNipcatFlowFunctionLibrary::IsInputConnectedAtIndex(UFlowNode* FlowNode, int32 Index)
{
	if (FlowNode)
	{
		if (FlowNode->GetInputPins().IsValidIndex(Index))
		{
			return FlowNode->IsInputConnected(FlowNode->GetInputPins()[Index]);
		}
	}
	return false;
}

bool UNipcatFlowFunctionLibrary::IsOutputConnectedAtIndex(UFlowNode* FlowNode, int32 Index)
{
	if (FlowNode)
	{
		if (FlowNode->GetOutputPins().IsValidIndex(Index))
		{
			return FlowNode->IsOutputConnected(FlowNode->GetOutputPins()[Index]);
		}
	}
	return false;
}
