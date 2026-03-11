// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "TargetHandlers/WeightedTargetHandler.h"
#include "Perception/AIPerceptionTypes.h"
#include "NipcatWeightedTargetHandler.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATLOCKONTARGET_API UNipcatWeightedTargetHandler : public UWeightedTargetHandler
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soft Lock", meta=(InlineEditConditionToggle))
	bool UseEnableSoftLockInputAngularLimitRequirements = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Soft Lock", meta=(EditCondition="UseEnableSoftLockInputAngularLimitRequirements"))
	FGameplayTagRequirements EnableSoftLockInputAngularLimitRequirements;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Auto Find")
	bool TryLockOnNewTargetAfterClearTarget = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Auto Find", meta=(EditCondition=TryLockOnNewTargetAfterClearTarget, EditConditionHides))
	bool TryLockOnNewTargetAfterClearTargetForSoftLock = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	FAISenseAffiliationFilter DetectionByAffiliation = {true, false, false};;

	virtual void HandleTargetUnlock(ETargetUnlockReason UnlockReason) override;

	virtual void PerformPrimarySamplingPass(FFindTargetContext& Context, TArray<FTargetContext>& OutTargetsData) override;

	virtual void CalcDeltaAngle2D(const FFindTargetContext& Context, FTargetContext& OutTargetContext) const override;

	virtual float CalculateTargetWeight_Implementation(const FFindTargetContext& Context, const FTargetContext& TargetContext) const override;
};
