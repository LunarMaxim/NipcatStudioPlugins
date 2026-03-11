// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "FootstepAnimEventsModifier.h"
#include "FootLockAnimCurveModifier.generated.h"

struct FFootSampleState;

USTRUCT(BlueprintType)
struct FFootLockFoot : public FFootDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Default")
	FName LockCurve;
};

UCLASS()
class NIPCATLOCOMOTIONSYSTEMEDITOR_API UFootLockAnimCurveModifier : public UAnimationModifier
{
	GENERATED_BODY()

	/** Rate used to sample the animation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (Units="Hz", UIMin=1, AllowPrivateAccess=true))
	int SampleRate;

	/** Threshold for determining if a foot bone position can be considered to be on the ground level */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta=(AllowPrivateAccess=true))
	float GroundThreshold;

	/**
	 * Threshold to start finding the smallest foot bone translation speed.
	 *
	 * Note that the foot bone translation speed is normalize therefore when a footstep occurs
	 * the speed will be very close to zero, thus for most cases this value won't need to be changed.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (ClampMin=0.0f, ClampMax=1.0f, AllowPrivateAccess=true))
	float SpeedThreshold;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (TitleProperty = "FootBoneName", AllowPrivateAccess=true))
	TArray<FFootLockFoot> FootDefinitions;
	
public:
	UFootLockAnimCurveModifier();

	virtual void OnApply_Implementation(UAnimSequence* InAnimation) override;
	virtual void OnRevert_Implementation(UAnimSequence* AnimationSequence) override;

private:
	float ComputeBoneSpeed(const FAnimPose& InPose, const FAnimPose& InFuturePose, float InDelta,
		FName InFootBoneName) const;
	bool CanWePlaceEventAtSample(const FFootSampleState& InFootSampleState, EDetectionTechnique DetectionTechnique) const;
};
