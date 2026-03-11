// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatFlow/NipcatFlowAsset.h"
#include "NipcatQuestFlowAsset.generated.h"

namespace NipcatQuestFlowCustomInputNames
{
	inline FName OnQuestAccepted = FName(TEXT("OnQuestAccepted"));
	inline FName OnQuestFinish = FName(TEXT("OnQuestFinish"));
}

class UNipcatDialogueSubsystem;
/**
 * 
 */
UCLASS()
class NIPCATFLOW_API UNipcatQuestFlowAsset : public UNipcatFlowAsset
{
	GENERATED_BODY()
	
public:
	UNipcatQuestFlowAsset();
	
};
