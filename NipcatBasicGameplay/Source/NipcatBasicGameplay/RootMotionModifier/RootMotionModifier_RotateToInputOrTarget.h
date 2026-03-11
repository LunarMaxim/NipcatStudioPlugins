// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RootMotionModifier.h"
#include "GameplayTagContainer.h"
#include "RootMotionModifier_RotateToInputOrTarget.generated.h"

struct FGameplayTagContainer;

UENUM(BlueprintType)
enum ERotateAxisType : uint8
{
	None,
	LeftFoot,
	RightFoot,
	Custom
};

class USkeletalMeshComponent;
/**
 * 
 */
UCLASS(meta = (DisplayName = "Rotate To Input Or Target"))
class NIPCATBASICGAMEPLAY_API URootMotionModifier_RotateToInputOrTarget : public URootMotionModifier
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interp")
	bool bUseInterp;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interp", meta=(EditCondition = "bUseInterp", EditConditionHides))
	// bool bPreferFasterRotateDirection = true;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interp", meta=(EditCondition = "bUseInterp", EditConditionHides, ClampMin = 1, UIMin = 1))
	float InterpSpeedLeft = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interp", meta=(EditCondition = "bUseInterp", EditConditionHides, ClampMin = 1, UIMin = 1))
	float InterpSpeedRight = 200.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta=(EditCondition = "bUseInterp", EditConditionHides))
	bool UpdateInputVectorDuringWarping = false;
	
	/** If true, Degree threshold will be used. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta=(EditCondition = "!(bUseInterp && UpdateInputVectorDuringWarping)", EditConditionHides))
	bool bUseDegreeThreshold;

	/** Degree threshold during rotation warp of the root motion. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "bUseDegreeThreshold", ClampMin = "0", UIMin = "0", ClampMax = "180", UIMax = "180", EditConditionHides))
	float DegreeThreshold;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TEnumAsByte<ERotateAxisType> RotateAxis = LeftFoot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	TEnumAsByte<ERotateAxisType> TurnBackRotateAxis = RightFoot;

	/** The name of the bone that is being rotated around its axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "RotateAxis == ERotateAxisType::Custom", EditConditionHides))
	FName RotateAroundBoneName = NAME_None;
	
	/** The name of the bone that is being rotated around its axis. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "TurnBackRotateAxis == ERotateAxisType::Custom", EditConditionHides))
	FName RotateBackAroundBoneName = NAME_None;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "RotateAxis == ERotateAxisType::LeftFoot || TurnBackRotateAxis == ERotateAxisType::LeftFoot", EditConditionHides))
	FName LeftFootBoneName = FName(TEXT("foot_l"));

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (EditCondition = "RotateAxis == ERotateAxisType::RightFoot ||  TurnBackRotateAxis == ERotateAxisType::RightFoot", EditConditionHides))
	FName RightFootBoneName = FName(TEXT("foot_r"));
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config")
	FGameplayTagContainer IgnoredTags;

	virtual void Update(const FMotionWarpingUpdateContext& Context) override;
	
	/** Called when the state of the modifier changes. */
	virtual void OnStateChanged(ERootMotionModifierState LastState) override;

	/** Process RootMotion. */
	virtual FTransform ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds) override;

	/** Get Bone Transform int world space. */
	FTransform GetBoneTransform(const FName& InBoneSocketName) const;

protected:

	UPROPERTY()
	FVector LastInputVector;
	
	UPROPERTY()
	AActor* TargetActor;
};
