// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tasks/TargetingFilterTask_BasicFilterTemplate.h"
#include "TargetingFilterTask_Team.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UTargetingFilterTask_Team : public UTargetingFilterTask_BasicFilterTemplate
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bDetectFriendlies : 1 = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bDetectNeutrals : 1 = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	uint32 bDetectEnemies : 1 = true;
	
	virtual bool ShouldFilterTarget(const FTargetingRequestHandle& TargetingHandle, const FTargetingDefaultResultData& TargetData) const override;
};
