// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Tasks/TargetingTask.h"

#include "NipcatTargetingSelectionTask_GameplayTag.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class NIPCATBASICGAMEPLAY_API UNipcatTargetingSelectionTask_GameplayTag : public UTargetingTask
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="Actor"))
	FGameplayTag Tag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="Actor"))
	FGameplayTagQuery TagQuery;
	
	virtual void Execute(const FTargetingRequestHandle& TargetingHandle) const override;

	static bool AddTargetActor(const FTargetingRequestHandle& TargetingHandle, AActor* Actor);
	
	virtual UWorld* GetWorld() const override;
};
