// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatFlowAsset.h"

#include "NipcatFlowNode.h"
#include "NipcatFlowOwnerInterface.h"

void UNipcatFlowAsset::StartFlow(IFlowDataPinValueSupplierInterface* DataPinValueSupplier)
{
	OnFlowStart();
	if (GetOwner())
	{
		if (const auto Interface = Cast<INipcatFlowOwnerInterface>(GetOwner()))
		{
			Interface->OnFlowStart();
		}
	}
	Super::StartFlow(DataPinValueSupplier);
}

void UNipcatFlowAsset::FinishFlow(const EFlowFinishPolicy InFinishPolicy, const bool bRemoveInstance)
{
	Super::FinishFlow(InFinishPolicy, bRemoveInstance);
	OnFlowFinish();
	if (GetOwner())
	{
		if (const auto Interface = Cast<INipcatFlowOwnerInterface>(GetOwner()))
		{
			Interface->OnFlowFinish(InFinishPolicy);
		}
	}
}

void UNipcatFlowAsset::RegisterNode(const FGuid& NewGuid, UFlowNode* NewNode)
{
	Super::RegisterNode(NewGuid, NewNode);
	if (const auto NipcatFlowNode = Cast<UNipcatFlowNode>(NewNode))
	{
		NipcatFlowNode->OnRegister();
	}
}

void UNipcatFlowAsset::UnregisterNode(const FGuid& NodeGuid)
{
	if (const auto NipcatFlowNode = Cast<UNipcatFlowNode>(GetNode(NodeGuid)))
	{
		NipcatFlowNode->OnUnregister();
	}
	Super::UnregisterNode(NodeGuid);
}
