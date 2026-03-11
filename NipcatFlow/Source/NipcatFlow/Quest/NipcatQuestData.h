// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlowAsset.h"

#include "NipcatQuestData.generated.h"


/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class NIPCATFLOW_API UNipcatQuestData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	
    static const FPrimaryAssetType QuestType;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText Title;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(MultiLine))
	FText Description;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(AllowedClasses="/Script/NipcatFlow.NipcatQuestFlowAsset"))
	TSoftObjectPtr<UFlowAsset> QuestFlow;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FFlowAssetParamsPtr FlowParams;
	
	virtual FPrimaryAssetId GetPrimaryAssetId() const override;
};
