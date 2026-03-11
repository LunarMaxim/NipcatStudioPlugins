// Copyright Nipcat Studio All Rights Reserved.


#include "FootLockAnimCurveModifier.h"

struct FFootSampleState
{
	double GroundLevel = UE_DOUBLE_BIG_NUMBER;
	double RefBoneTranslationDotRefBoneToFootBoneVec = 0.0;
	double PrevRefBoneTranslationDotRefBoneToFootBoneVec = 0.0;
	bool bIsFootBoneInGround = false;
	bool bWasFootBoneInGround = false; 
	float FootBoneSpeed = 0.0f;
	float PrevFootBoneSpeed = 0.0f;
	float MaxFootSpeed = -MAX_FLT;
	float MinFootSpeedBelowThreshold = MAX_FLT;
	float TimeAtMinFootSpeedBelowThreshold = MAX_FLT;
};

UFootLockAnimCurveModifier::UFootLockAnimCurveModifier()
{
	SampleRate = 60;
	GroundThreshold = 4.0f;
	SpeedThreshold = 0.1f;
}

void UFootLockAnimCurveModifier::OnApply_Implementation(UAnimSequence* InAnimation)
{
	Super::OnApply_Implementation(InAnimation);

	if (InAnimation == nullptr)
	{
		UE_LOG(LogAnimation, Error, TEXT("FootstepAnimEventsModifierBase failed. Reason: Invalid Animation"));
		return;
	}
	
	// Disable root motion lock during application
	TGuardValue<bool> ForceRootLockGuard(InAnimation->bForceRootLock, false);
	
				
	for (const FFootLockFoot& FootDef : FootDefinitions)
	{
		UAnimationBlueprintLibrary::AddCurve(InAnimation, FootDef.LockCurve, ERawCurveTrackTypes::RCT_Float, false);
	}
	
	// Process animation asset
	{
		const FAnimPoseEvaluationOptions AnimPoseEvalOptions { EAnimDataEvalType::Raw, true, false, false, nullptr, true, false };
		const float SequenceLength = InAnimation->GetPlayLength();
		const float SampleStep = 1.0f / static_cast<float>(SampleRate);
		const int SampleNum = FMath::TruncToInt(SequenceLength / SampleStep);
		
		TArray<FFootSampleState> FootSampleStates;
		FootSampleStates.Init(FFootSampleState(), FootDefinitions.Num());
		
		// Get ground levels and max speed values.
		for (int SampleIndex = 0; SampleIndex < SampleNum; ++SampleIndex)
		{
			const float SampleTime = FMath::Clamp(static_cast<float>(SampleIndex) * SampleStep, 0.0f, SequenceLength);
			const float FutureSampleTime = FMath::Clamp((static_cast<float>(SampleIndex) + 1.0f) * SampleStep, 0.0f, SequenceLength);

			FAnimPose AnimPose;
			FAnimPose FutureAnimPose;
			
			UAnimPoseExtensions::GetAnimPoseAtTime(InAnimation, SampleTime, AnimPoseEvalOptions, AnimPose);
			UAnimPoseExtensions::GetAnimPoseAtTime(InAnimation, FutureSampleTime, AnimPoseEvalOptions, FutureAnimPose);
			
			for (int FootIndex = 0; FootIndex < FootDefinitions.Num(); ++FootIndex)
			{
				const FFootDefinition & FootDef = FootDefinitions[FootIndex];
				FFootSampleState & FootState = FootSampleStates[FootIndex];
				
				FootState.GroundLevel = FMath::Min(FootState.GroundLevel, UAnimPoseExtensions::GetBonePose(AnimPose, FootDef.FootBoneName, EAnimPoseSpaces::World).GetLocation().Z);
				FootState.MaxFootSpeed = FMath::Max(FootState.MaxFootSpeed, FMath::Abs(ComputeBoneSpeed(AnimPose, FutureAnimPose, SampleStep, FootDef.FootBoneName)));
			}
		}

		// Generate animation events
		for (int SampleIndex = 0; SampleIndex < (SampleNum - 1); ++SampleIndex)
		{
			const float SampleTime = FMath::Clamp(static_cast<float>(SampleIndex) * SampleStep, 0.0f, SequenceLength);
			const float FutureSampleTime = FMath::Clamp((static_cast<float>(SampleIndex) + 1.0f) * SampleStep, 0.0f, SequenceLength);

			FAnimPose AnimPose;
			FAnimPose FutureAnimPose;
			
			// Query animation pose at the current sample time
			UAnimPoseExtensions::GetAnimPoseAtTime(InAnimation, SampleTime, AnimPoseEvalOptions, AnimPose);
			UAnimPoseExtensions::GetAnimPoseAtTime(InAnimation, FutureSampleTime, AnimPoseEvalOptions, FutureAnimPose);
			
			// Process all foot definitions
			for (int CurrFootIdx = 0; CurrFootIdx < FootDefinitions.Num(); ++CurrFootIdx)
			{
				const FFootLockFoot & FootDef = FootDefinitions[CurrFootIdx];
				FFootSampleState & FootState = FootSampleStates[CurrFootIdx];
				
				// Update current sample info
				{
					const FTransform FootBoneTransform = UAnimPoseExtensions::GetBonePose(AnimPose, FootDef.FootBoneName, EAnimPoseSpaces::World);

					// Method: PassThroughReferenceBone
					{
						// Get reference bone translation
						const FTransform ReferenceBoneTransform = UAnimPoseExtensions::GetBonePose(AnimPose, FootDef.ReferenceBoneName, EAnimPoseSpaces::World);
						const FTransform FutureReferenceBoneTransform = UAnimPoseExtensions::GetBonePose(FutureAnimPose, FootDef.ReferenceBoneName, EAnimPoseSpaces::World);
						const FVector ReferenceBoneTranslation = FutureReferenceBoneTransform.GetLocation() - ReferenceBoneTransform.GetLocation();

						// Get foot bone direction with respect to reference bone
						const FVector ReferenceBoneToFootBoneVector = FootBoneTransform.GetLocation() - ReferenceBoneTransform.GetLocation();
					
						FootState.RefBoneTranslationDotRefBoneToFootBoneVec = FVector::DotProduct(ReferenceBoneTranslation, ReferenceBoneToFootBoneVector);
					}

					// Method: FootBoneReachesGround
					FootState.bIsFootBoneInGround = FMath::Abs(FootState.GroundLevel - FootBoneTransform.GetLocation().Z) <= GroundThreshold;

					// Method: FootBoneSpeed
					FootState.FootBoneSpeed = FMath::Abs(ComputeBoneSpeed(AnimPose, FutureAnimPose, SampleStep, FootDef.FootBoneName)) / FootState.MaxFootSpeed;

					// Keep track of sample with lowest speed below threshold
					if (FootState.FootBoneSpeed < SpeedThreshold)
					{
						if (FootState.FootBoneSpeed < FootState.MinFootSpeedBelowThreshold)
						{
							FootState.TimeAtMinFootSpeedBelowThreshold = SampleTime;
							FootState.MinFootSpeedBelowThreshold = FootState.FootBoneSpeed;
						}
					}
				}

				if (FootState.bIsFootBoneInGround && FootState.FootBoneSpeed < SpeedThreshold)
				{
					UAnimationBlueprintLibrary::AddFloatCurveKey(InAnimation, FootDef.LockCurve, SampleTime, 1.0f);
				}
				else
				{
					UAnimationBlueprintLibrary::AddFloatCurveKey(InAnimation, FootDef.LockCurve, SampleTime, 0.0f);
				}
				
				// Update foot state
				{
					const bool bDidWePlaceAnyEventsUsingFootBoneSpeed = CanWePlaceEventAtSample(FootState, EDetectionTechnique::FootBoneSpeed);

					// Reset minimum values if we are above speed threshold
					if (bDidWePlaceAnyEventsUsingFootBoneSpeed)
					{
						FootState.TimeAtMinFootSpeedBelowThreshold = MAX_FLT;
						FootState.MinFootSpeedBelowThreshold = MAX_FLT;
					}
					
					// Keep track of foot info
					FootState.PrevFootBoneSpeed = FootState.FootBoneSpeed;
					FootState.bWasFootBoneInGround = FootState.bIsFootBoneInGround;
					FootState.PrevRefBoneTranslationDotRefBoneToFootBoneVec = FootState.RefBoneTranslationDotRefBoneToFootBoneVec;
				}
			}
		}
	}
}

void UFootLockAnimCurveModifier::OnRevert_Implementation(UAnimSequence* AnimationSequence)
{
	Super::OnRevert_Implementation(AnimationSequence);
	
	for (const FFootLockFoot& FootDef : FootDefinitions)
	{
		UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, FootDef.LockCurve);
	}
	
}

float UFootLockAnimCurveModifier::ComputeBoneSpeed(const FAnimPose& InPose, const FAnimPose& InFuturePose, float InDelta, FName InFootBoneName) const
{
	check(!FMath::IsNearlyZero(InDelta))
			
	const FTransform FootBoneTransform = UAnimPoseExtensions::GetBonePose(InPose, InFootBoneName, EAnimPoseSpaces::World);
	const FTransform FutureFootBoneTransform = UAnimPoseExtensions::GetBonePose(InFuturePose, InFootBoneName, EAnimPoseSpaces::World);
	const double FootBoneDistance = (FutureFootBoneTransform.GetLocation() - FootBoneTransform.GetLocation()).Length();
			
	return static_cast<float>(FootBoneDistance / InDelta);
}

bool UFootLockAnimCurveModifier::CanWePlaceEventAtSample(const FFootSampleState& InFootSampleState, EDetectionTechnique DetectionTechnique) const
{
	switch(DetectionTechnique)
	{
	case EDetectionTechnique::PassThroughReferenceBone: return InFootSampleState.RefBoneTranslationDotRefBoneToFootBoneVec > 0.0f && InFootSampleState.PrevRefBoneTranslationDotRefBoneToFootBoneVec < 0.0f;
	case EDetectionTechnique::FootBoneReachesGround: return !InFootSampleState.bWasFootBoneInGround && InFootSampleState.bIsFootBoneInGround;
	case EDetectionTechnique::FootBoneSpeed: return InFootSampleState.PrevFootBoneSpeed < SpeedThreshold && InFootSampleState.FootBoneSpeed >= SpeedThreshold;
	default: return false;
	}
}