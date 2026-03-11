// Copyright Nipcat Studio All Rights Reserved.


#include "RootMotionModifier_RotateToInputOrTarget.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "KismetAnimationLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "MotionWarpingComponent.h"
#include "NipcatBasicGameplay/Interface/NipcatBasicGameplayCharacterInterface.h"


void URootMotionModifier_RotateToInputOrTarget::Update(const FMotionWarpingUpdateContext& Context)
{
	Super::Update(Context);
	// Mark for removal if we jumped to a position outside the warping window
	if (GetState() == ERootMotionModifierState::Active && PreviousPosition < EndTime && (CurrentPosition > EndTime || CurrentPosition < StartTime))
	{
		SetState(ERootMotionModifierState::MarkedForRemoval);
	}
}

void URootMotionModifier_RotateToInputOrTarget::OnStateChanged(ERootMotionModifierState LastState)
{
	if (!(bUseInterp && UpdateInputVectorDuringWarping))
	{
		if (LastState != ERootMotionModifierState::Active &&  ERootMotionModifierState::Active == GetState())
		{
			LastInputVector = FVector::ZeroVector;
			if (const ACharacter* CharacterOwner = Cast<ACharacter>(GetActorOwner()))
			{
				if (const UCharacterMovementComponent* CharacterMovement = CharacterOwner->GetCharacterMovement())
				{
					LastInputVector = CharacterMovement->GetLastInputVector();
					LastInputVector.Normalize();
				}

				if (bUseDegreeThreshold && !LastInputVector.IsNearlyZero())
				{
					const FVector ForwardVector = CharacterOwner->GetActorForwardVector();

					const float ForwardCosAngle = FVector::DotProduct(ForwardVector, LastInputVector);
					const float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

					const float AngleToRotate = ForwardDeltaDegree - DegreeThreshold;
					if (AngleToRotate > 0.f)
					{
						FVector Axis = FVector::CrossProduct(ForwardVector, LastInputVector);
						Axis.Normalize();

						//right
						if (Axis.Z > 0)
						{
							float Alpha = DegreeThreshold / ForwardDeltaDegree;
							FQuat TotalRotation = FQuat::FindBetweenNormals(ForwardVector, LastInputVector);
							TotalRotation = FQuat::FastLerp(FQuat::Identity, TotalRotation, Alpha);
							LastInputVector = TotalRotation.RotateVector(ForwardVector);
						}
						//left
						else
						{
							float Alpha = DegreeThreshold / ForwardDeltaDegree;
							FQuat TotalRotation = FQuat::FindBetweenNormals(LastInputVector, ForwardVector);
							TotalRotation = FQuat::FastLerp(FQuat::Identity, TotalRotation.Inverse(), Alpha);
							LastInputVector = TotalRotation.RotateVector(ForwardVector);
						}
					}
				}
			}
		}
	}

	Super::OnStateChanged(LastState);
}

FTransform URootMotionModifier_RotateToInputOrTarget::ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds)
{
	AActor* ActorOwner = GetActorOwner();
	if (ActorOwner == nullptr)
	{
		return InRootMotion;
	}

	if (!IgnoredTags.IsEmpty())
	{
		const UAbilitySystemComponent* ASC = Cast<IAbilitySystemInterface>(ActorOwner)->GetAbilitySystemComponent();
		if (ASC == nullptr)
		{
			return InRootMotion;
		}
		if (ASC->HasAnyMatchingGameplayTags(IgnoredTags))
		{
			return InRootMotion;
		}
	}

	const UCharacterMovementComponent* CharacterMovement = Cast<ACharacter>(ActorOwner)->GetCharacterMovement();
	if (CharacterMovement == nullptr)
	{
		return InRootMotion;
	}

	// UseInterp时才允许UpdateInputVector
	if (UpdateInputVectorDuringWarping && bUseInterp)
	{
		LastInputVector = CharacterMovement->GetLastInputVector();
	}
	else
	{
		// 如果至少捕获一次Target，且不持续更新Input，则不再启用InputVector
		if (TargetActor)
		{
			LastInputVector = FVector::Zero();
		}
	}
	
	FVector TargetDirection = LastInputVector;

	// 如果有目标，重载TargetDirection
	if (AActor* FoundTargetActor = INipcatBasicGameplayCharacterInterface::Execute_NipcatBasicGameplay_GetTargetActor(ActorOwner))
	{
		TargetActor = FoundTargetActor;
		FVector TargetLocation = INipcatBasicGameplayCharacterInterface::Execute_NipcatBasicGameplay_GetTargetLocation(ActorOwner);
		TargetDirection = (TargetLocation - ActorOwner->GetActorLocation()).GetSafeNormal2D();
	}

	if (TargetDirection == FVector::Zero())
	{
		return InRootMotion;
	}
	
	FTransform FinalRootMotion = InRootMotion;

	const FTransform CharacterTransform = ActorOwner->GetTransform();

	const FRotator CharacterRotation = CharacterTransform.Rotator();
	const FRotator TargetRotation = TargetDirection.ToOrientationRotator();
	
	const FQuat CharacterQuat = CharacterTransform.GetRotation();
	const FQuat TargetQuat = TargetDirection.ToOrientationQuat();
	FQuat InterpolatedRotation;

	// bool bUsingFasterDirection = false;
	
	
	if (bUseInterp)
	{
		FRotator InterpTargetRotation = TargetRotation;
		
		const float RelativeTargetDirection = UKismetAnimationLibrary::CalculateDirection(TargetDirection, CharacterRotation);
		
		bool IsRightSide = RelativeTargetDirection > 0.f;
		/*
		if (bPreferFasterRotateDirection)
		{
			const float TurnRightRequiredAngle = IsRightSide ? RelativeTargetDirection : 360 + RelativeTargetDirection;
			const float TurnLeftRequiredAngle = IsRightSide ? 360 - RelativeTargetDirection : -RelativeTargetDirection;
			
			const float TurnRightRequiredTime = UKismetMathLibrary::SafeDivide(TurnRightRequiredAngle , InterpSpeedRight);
			const float TurnLeftRequiredTime = UKismetMathLibrary::SafeDivide(TurnLeftRequiredAngle , InterpSpeedLeft);
			
			const bool IsRightSideFaster = TurnRightRequiredTime < TurnLeftRequiredTime;
			
			if (IsRightSide != IsRightSideFaster)
			{
				bUsingFasterDirection = true;
				IsRightSide = IsRightSideFaster;
				InterpTargetRotation = CharacterRotation + (IsRightSideFaster ? FRotator(0, 179, 0) : FRotator(0, -179, 0));
			}
		}*/
		
		InterpolatedRotation = FMath::RInterpConstantTo(CharacterRotation, InterpTargetRotation, DeltaSeconds, IsRightSide ? InterpSpeedRight : InterpSpeedLeft).Quaternion();
	}
	else
	{
		const float TimeRemaining = (EndTime - PreviousPosition);
		const float Alpha = FMath::Clamp(DeltaSeconds / TimeRemaining, 0.f, 1.f);
		InterpolatedRotation = FQuat::Slerp(CharacterQuat, TargetQuat, Alpha);
	}
	
	const FQuat InterpolatedQuat = InterpolatedRotation * CharacterQuat.Inverse();
	const FQuat DeltaQuat = InterpolatedQuat * InRootMotion.GetRotation();

	FinalRootMotion.SetRotation(DeltaQuat.Vector().GetSafeNormal2D().ToOrientationQuat());

	if (RotateAxis != None)
	{
		ACharacter* CharacterOwner = Cast<ACharacter>(GetActorOwner());
		const FTransform MeshRelativeTransform = FTransform(CharacterOwner->GetBaseRotationOffset(), CharacterOwner->GetBaseTranslationOffset());
		const FTransform MeshTransform = MeshRelativeTransform * ActorOwner->GetActorTransform();

		const FVector DesiredTranslation = MeshTransform.TransformPosition(InRootMotion.GetTranslation());

		FName BoneName;
		if (TargetDirection.Dot(ActorOwner->GetActorForwardVector()) < 0 && TurnBackRotateAxis != None)
		// if (bUsingFasterDirection)
		{
			switch (TurnBackRotateAxis)
			{
			case LeftFoot:
				{
					BoneName = LeftFootBoneName;
				}
				break;
			case RightFoot:
				{
					BoneName = RightFootBoneName;
				}
				break;
			default:
				{
					BoneName = RotateBackAroundBoneName;
				}
			}
		}
		else
		{
			switch (RotateAxis)
			{
			case LeftFoot:
				{
					BoneName = LeftFootBoneName;
				}
				break;
			case RightFoot:
				{
					BoneName = RightFootBoneName;
				}
				break;
			default:
				{
					BoneName = RotateAroundBoneName;
				}
			}
		}
		
		FVector BoneTranslation = GetBoneTransform(BoneName).GetTranslation();
		BoneTranslation.Z = DesiredTranslation.Z;

		const FVector CurrentDirection = DesiredTranslation - BoneTranslation;
		const FVector DesiredDirection = (InterpolatedRotation * CharacterQuat.Inverse()).RotateVector(CurrentDirection);
		const FVector DestDesiredTranslation = BoneTranslation + DesiredDirection;
		const FVector DeltaTranslation = MeshTransform.InverseTransformPosition(DestDesiredTranslation);

		FinalRootMotion.SetTranslation(DeltaTranslation);
	}

	return FinalRootMotion;
}

FTransform URootMotionModifier_RotateToInputOrTarget::GetBoneTransform(const FName& InBoneSocketName) const
{
	if (const ACharacter* CharacterOwner = Cast<ACharacter>(GetActorOwner()))
	{
		if (const USkeletalMeshComponent* CharacterMesh = CharacterOwner->GetMesh())
		{
			return InBoneSocketName.IsNone() ? CharacterOwner->GetTransform() : CharacterMesh->GetSocketTransform(InBoneSocketName);
		}
	}

	return FTransform::Identity;
}