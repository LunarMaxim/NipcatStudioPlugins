// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "NipcatInteractTypes.generated.h"

class INipcatInteractableInterface;
class UNipcatInteractAction;
class UNipcatInteractCondition;
class UInputAction;


USTRUCT(BlueprintType)
struct NIPCATBASICGAMEPLAY_API FNipcatInteractStateConfig
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="InteractState"))
	FGameplayTag StateTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText InteractName;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	TArray<UNipcatInteractCondition*> EnterStateConditions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (InlineEditConditionToggle))
	bool HasInteractActions;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta = (EditCondition = "HasInteractActions"))
	TArray<UNipcatInteractCondition*> InteractConditions;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta = (EditCondition = "HasInteractActions"))
	TArray<UNipcatInteractAction*> SucceedActions;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta = (EditCondition = "HasInteractActions"))
	TArray<UNipcatInteractAction*> FailedActions;
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct NIPCATBASICGAMEPLAY_API FNipcatInteractConfig
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TSoftObjectPtr<UInputAction> InputAction;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(TitleProperty="StateTag"))
	TArray<FNipcatInteractStateConfig> States;

};
