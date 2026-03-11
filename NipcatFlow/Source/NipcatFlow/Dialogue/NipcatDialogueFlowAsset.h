// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatFlow/NipcatFlowAsset.h"
#include "NipcatDialogueFlowAsset.generated.h"

class UNipcatDialogueSubsystem;
/**
 * 
 */
UCLASS()
class NIPCATFLOW_API UNipcatDialogueFlowAsset : public UNipcatFlowAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bCanSkip = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = "bCanSkip"))
	FText SkipDescription;
	
	virtual void OnFlowStart() override;
	virtual void OnFlowFinish() override;
	
	UNipcatDialogueSubsystem* GetDialogueSubsystem() const;
};
