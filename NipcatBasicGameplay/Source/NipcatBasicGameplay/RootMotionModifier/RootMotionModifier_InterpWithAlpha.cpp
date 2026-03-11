// Copyright Nipcat Studio All Rights Reserved.


#include "RootMotionModifier_InterpWithAlpha.h"

#include "MotionWarpingComponent.h"
#include "GameFramework/Character.h"

URootMotionModifier_InterpWithAlpha::URootMotionModifier_InterpWithAlpha(const FObjectInitializer& ObjectInitializer):
	Super(ObjectInitializer), UseFixedTransform(false), UseWarpTargetAsAdditive(false), WarpTranslation(true), TranslationInterpSpeed(0), IgnoreZ(true),
	WarpRotation(true),
	RotationInterpSpeed(0),
	HasTarget(false)
{
}

FTransform URootMotionModifier_InterpWithAlpha::ProcessRootMotion(const FTransform& InRootMotion, const float DeltaSeconds)
{
	if (!HasTarget && !UseFixedTransform)
	{
		return InRootMotion;
	}

	const ACharacter* Character = Cast<ACharacter>(GetActorOwner());
	if (!Character)
	{
		return InRootMotion;
	}

	const FTransform ActorTransform = Character->GetTransform();
	const FTransform MeshRelativeTransform = FTransform(Character->GetBaseRotationOffset(),
	                                                    Character->GetBaseTranslationOffset());
	const FTransform MeshTransform = MeshRelativeTransform * ActorTransform;
	FTransform Result = InRootMotion;

	if (WarpRotation)
	{
		float RotationAlpha = FixedRotationInterpAlpha;
		if (!UseFixedRotationInterpAlpha)
		{
			RotationAlpha = FMath::Clamp((CurrentPosition - ActualStartTime) / (EndTime - ActualStartTime), 0.f, 1.f);
			RotationAlpha = FAlphaBlend::AlphaToBlendOption(RotationAlpha, TranslationEasingFunc, TranslationEasingCurve);
		}

		
		if (UseWarpTargetAsAdditive || UseFixedTransform)
		{
			Result.SetRotation(FMath::QInterpTo(FQuat::Identity,
			                                    FQuat::Slerp(FQuat::Identity, UseFixedTransform ? FixedRotation.ToOrientationQuat() : WarpTargetTransform.GetRotation(),
			                                                 RotationAlpha), DeltaSeconds,
			                                    RotationInterpSpeed) * Result.GetRotation());
		}
		else
		{
			const FQuat OriginRotation = ActorTransform.GetRotation();
			const FQuat LerpRotation = FMath::QInterpTo(OriginRotation,
			                                            FQuat::Slerp(OriginRotation, WarpTargetTransform.GetRotation(),
			                                                         RotationAlpha), DeltaSeconds, RotationInterpSpeed);
			Result.SetRotation(
				(ActorTransform.InverseTransformRotation(LerpRotation).Rotator() - ActorTransform.
					InverseTransformRotation(
						OriginRotation).
					Rotator()).GetNormalized().Quaternion());
		}
	}

	if (WarpTranslation)
	{
		float TransitionAlpha = FixedRotationInterpAlpha;
		if (!UseFixedTranslationInterpAlpha)
		{
			TransitionAlpha = FMath::Clamp((CurrentPosition - ActualStartTime) / (EndTime - ActualStartTime), 0.f,1.f);
			TransitionAlpha = FAlphaBlend::AlphaToBlendOption(TransitionAlpha, TranslationEasingFunc,TranslationEasingCurve);
		}

		if (UseWarpTargetAsAdditive || UseFixedTransform)
		{
			const FVector TargetLocation = FMath::VInterpTo(FVector::ZeroVector, FMath::Lerp(
				                                                FVector::ZeroVector,
				                                                UseFixedTransform ? FixedTranslation : WarpTargetTransform.GetLocation(),
				                                                TransitionAlpha), DeltaSeconds, TranslationInterpSpeed);
			FVector RelativeLocation = MeshTransform.Rotator().
			                                         UnrotateVector(TargetLocation);
			if (IgnoreZ)
			{
				RelativeLocation.Z = Result.GetLocation().Z;
			}
			Result.SetLocation(Result.GetLocation() + RelativeLocation);
		}
		else
		{
			const FVector TargetLocation = FMath::VInterpTo(ActorTransform.GetLocation(), FMath::Lerp(
				                                                ActorTransform.GetLocation(),
				                                                WarpTargetTransform.GetLocation(),
				                                                TransitionAlpha), DeltaSeconds, TranslationInterpSpeed);
			FVector RelativeLocation = MeshTransform.Rotator().
			                                         UnrotateVector(TargetLocation - ActorTransform.GetLocation());
			if (IgnoreZ)
			{
				RelativeLocation.Z = Result.GetLocation().Z;
			}
			Result.SetLocation(RelativeLocation);
		}
	}

	return Result;
}

void URootMotionModifier_InterpWithAlpha::Update(const FMotionWarpingUpdateContext& Context)
{
	Super::Update(Context);
	const UMotionWarpingComponent* OwnerComp = GetOwnerComponent();
	if (const FMotionWarpingTarget* Target = OwnerComp->FindWarpTarget(WarpTarget))
	{
		WarpTargetTransform = Target->GetTargetTrasform();
		HasTarget = true;
	}
	else
	{
		HasTarget = false;
	}
}
