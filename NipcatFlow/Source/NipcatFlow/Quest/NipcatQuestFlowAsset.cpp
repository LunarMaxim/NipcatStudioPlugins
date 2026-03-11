// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatQuestFlowAsset.h"


UNipcatQuestFlowAsset::UNipcatQuestFlowAsset()
{
	CustomInputs.Add(NipcatQuestFlowCustomInputNames::OnQuestAccepted);
	CustomInputs.Add(NipcatQuestFlowCustomInputNames::OnQuestFinish);
}

