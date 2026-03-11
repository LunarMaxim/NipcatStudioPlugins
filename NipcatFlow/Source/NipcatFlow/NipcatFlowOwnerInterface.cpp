// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatFlowOwnerInterface.h"


void INipcatFlowOwnerInterface::OnFlowStart()
{
	Execute_BP_OnFlowStart(_getUObject());
}

void INipcatFlowOwnerInterface::OnFlowFinish(const EFlowFinishPolicy& FinishPolicy)
{
	Execute_BP_OnFlowFinish(_getUObject(), FinishPolicy);
}
