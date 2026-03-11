// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "NipcatQuestInstance.generated.h"

class UFlowAsset;

UENUM(BlueprintType)
enum class ENipcatQuestProgress : uint8
{
	NotStarted,
	InProgress,
	Completed
};

class UNipcatQuestData;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class NIPCATFLOW_API UNipcatQuestInstance : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	TObjectPtr<UNipcatQuestData> QuestData;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	ENipcatQuestProgress QuestProgress;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, SaveGame)
	FGameplayTagContainer QuestState;
};
