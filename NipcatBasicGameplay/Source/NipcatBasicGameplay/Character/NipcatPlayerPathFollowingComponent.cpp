// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatPlayerPathFollowingComponent.h"

#include "AnimCharacterMovementLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"


void UNipcatPlayerPathFollowingComponent::UpdateDecelerationData()
{
	// no point in updating if we don't have a MovementComp
	if (NavMovementInterface.GetObject() == nullptr)
	{
		return;
	}

	if (!bStopMovementOnFinish)
	{
		return;
	}

	const float CurrentMaxSpeed = NavMovementInterface->GetMaxSpeedForNavMovement();
	bool bUpdatePathSegment = (DecelerationSegmentIndex == INDEX_NONE);
	if (CurrentMaxSpeed != CachedBrakingMaxSpeed)
	{
		const float PrevBrakingDistance = CachedBrakingDistance;
		
		UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(NavMovementInterface.GetObject());
		CachedBrakingDistance = UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(
			FVector::UnitX() * CurrentMaxSpeed,
			CharacterMovementComponent->bUseSeparateBrakingFriction, CharacterMovementComponent->BrakingFriction,
			CharacterMovementComponent->GroundFriction, CharacterMovementComponent->BrakingFrictionFactor,
			CharacterMovementComponent->BrakingDecelerationWalking).Length();
		
		CachedBrakingMaxSpeed = CurrentMaxSpeed;
		
		bUpdatePathSegment = bUpdatePathSegment || (PrevBrakingDistance != CachedBrakingDistance);
	}

	if (bUpdatePathSegment && Path.IsValid())
	{
		DecelerationSegmentIndex = 0;

		const TArray<FNavPathPoint>& PathPoints = Path->GetPathPoints();
		FVector::FReal PathLengthFromEnd = 0.;

		for (int32 Idx = PathPoints.Num() - 1; Idx > 0; Idx--)
		{
			const FVector::FReal PathSegmentLength = FVector::Dist(PathPoints[Idx].Location, PathPoints[Idx - 1].Location);
			PathLengthFromEnd += PathSegmentLength;

			if (PathLengthFromEnd > CachedBrakingDistance)
			{
				DecelerationSegmentIndex = Idx - 1;
				break;
			}
		}

		UE_VLOG(GetOwner(), LogPathFollowing, Log, TEXT("Updated deceleration segment: %d (MaxSpeed:%.2f, BrakingDistance:%.2f"), DecelerationSegmentIndex, CachedBrakingMaxSpeed, CachedBrakingDistance);
	}
}

void UNipcatPlayerPathFollowingComponent::FollowPathSegment(float DeltaTime)
{
		if (!Path.IsValid() || NavMovementInterface == nullptr)
	{
		return;
	}

	const FVector CurrentLocation = NavMovementInterface->GetFeetLocation();
	const FVector CurrentTarget = GetCurrentTargetLocation();
	
	// set to false by default, we will set set this back to true if appropriate
	bIsDecelerating = false;

	const bool bAccelerationBased = NavMovementInterface->UseAccelerationForPathFollowing();
	if (bAccelerationBased)
	{
		CurrentMoveInput = (CurrentTarget - CurrentLocation).GetSafeNormal();
		
		if (bStopMovementOnFinish && (MoveSegmentStartIndex >= Path->GetPathPoints().Num() - 2))
		{
			const FVector PathEnd = Path->GetEndLocation();
			const FVector::FReal DistToEndSq = FVector::DistSquared(CurrentLocation, PathEnd);
			const bool bShouldDecelerate = DistToEndSq < FMath::Square(
				(NavMovementInterface->GetVelocityForNavMovement().Length() == NavMovementInterface->GetMaxSpeedForNavMovement() ? CachedBrakingDistance : PredictCurrentBrakingDistance()) +
				(StartDecelerationBeforeReachingAcceptanceRadius ? AcceptanceRadius : 0.0f));
			
			if (bShouldDecelerate)
			{
				bIsDecelerating = true;

				// const FVector::FReal  SpeedPct = FMath::Clamp(FMath::Sqrt(DistToEndSq) / CachedBrakingDistance, 0., 1.);
				// CurrentMoveInput *= SpeedPct;
				CurrentMoveInput = FVector::Zero();
				return;
			}
		}

		if (MovementInputLimitAngle >= 0 && MovementInputLimitAngle < 180)
		{
			const FVector ForwardVector = Cast<AActor>(NavMovementInterface->GetOwnerAsObject())->GetActorForwardVector();

			const float ForwardCosAngle = FVector::DotProduct(ForwardVector, CurrentMoveInput);
			const float ForwardDeltaDegree = FMath::RadiansToDegrees(FMath::Acos(ForwardCosAngle));

			const float AngleToRotate = ForwardDeltaDegree - MovementInputLimitAngle;
			if (AngleToRotate > 0.f)
			{
				FVector Axis = FVector::CrossProduct(ForwardVector, CurrentMoveInput);
				Axis.Normalize();

				const float Alpha = MovementInputLimitAngle / ForwardDeltaDegree;
				
				//right
				if (Axis.Z > 0)
				{
					FQuat TotalRotation = FQuat::FindBetweenNormals(ForwardVector, CurrentMoveInput);
					TotalRotation = FQuat::FastLerp(FQuat::Identity, TotalRotation, Alpha);
					CurrentMoveInput = TotalRotation.RotateVector(ForwardVector);
				}
				//left
				else
				{
					FQuat TotalRotation = FQuat::FindBetweenNormals(CurrentMoveInput, ForwardVector);
					TotalRotation = FQuat::FastLerp(FQuat::Identity, TotalRotation.Inverse(), Alpha);
					CurrentMoveInput = TotalRotation.RotateVector(ForwardVector);
				}
			}
		}

		PostProcessMove.ExecuteIfBound(this, CurrentMoveInput);
		if (UPawnMovementComponent* PawnMovementComponent = Cast<UPawnMovementComponent>(NavMovementInterface.GetObject()))
		{
			PawnMovementComponent->AddInputVector(CurrentMoveInput, true);
		}
		else
		{
			NavMovementInterface->RequestPathMove(CurrentMoveInput);
		}
	}
	else
	{
		FVector MoveVelocity = (CurrentTarget - CurrentLocation) / DeltaTime;

		const int32 LastSegmentStartIndex = Path->GetPathPoints().Num() - 2;
		const bool bNotFollowingLastSegment = (MoveSegmentStartIndex < LastSegmentStartIndex);

		PostProcessMove.ExecuteIfBound(this, MoveVelocity);
		NavMovementInterface->RequestDirectMove(MoveVelocity, bNotFollowingLastSegment);
	}
}

float UNipcatPlayerPathFollowingComponent::PredictCurrentBrakingDistance()
{
	const UCharacterMovementComponent* CharacterMovementComponent = Cast<UCharacterMovementComponent>(NavMovementInterface.GetObject());
	return UAnimCharacterMovementLibrary::PredictGroundMovementStopLocation(
		CharacterMovementComponent->Velocity,
		CharacterMovementComponent->bUseSeparateBrakingFriction, CharacterMovementComponent->BrakingFriction,
		CharacterMovementComponent->GroundFriction, CharacterMovementComponent->BrakingFrictionFactor,
		CharacterMovementComponent->BrakingDecelerationWalking).Length();
}
