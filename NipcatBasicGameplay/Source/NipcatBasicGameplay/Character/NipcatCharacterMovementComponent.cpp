// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatCharacterMovementComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/Character.h"
#include "CharacterRotationModifier.h"
#include "CollisionDebugDrawingPublic.h"
#include "Components/CapsuleComponent.h"
#include "CustomMovement/NipcatCustomMovementMode.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"
#include "NipcatBasicGameplay/Settings/NipcatBasicGameplayDeveloperSettings.h"


UNipcatCharacterMovementComponent::UNipcatCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
{
	// comment is default value
	bOrientRotationToMovement = true; // false
	GravityScale = 2.f; // 1.f
	JumpZVelocity = 650.f; // 420
	AirControl = 0.1f; // 0.05
	MaxWalkSpeed = 500.f; // 600
	MinAnalogWalkSpeed = 20.f; // 0
	BrakingDecelerationWalking = 800.f; // 2048
	bUseSeparateBrakingFriction = true; // false
	// BrakingFrictionFactor = 1.f; // 2.f
	GroundFriction = 5.f; // 8.f
	PerchRadiusThreshold = 20.f; // 0.f
	LedgeCheckThreshold = 0.f; // 4.f
	RotationRate = FRotator(0.f, 600.f, 0.f);
	NavAgentProps.bCanCrouch = true; // false
	// bUseAccelerationForPaths = true; // false
	NavMovementProperties.bUseAccelerationForPaths = true; // false
}

void UNipcatCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(CharacterOwner);
}

FGameplayTag UNipcatCharacterMovementComponent::FindCustomMovementModeTag(uint8 InCustomMovementMode)
{
	if (const auto Tag = UNipcatBasicGameplayDeveloperSettings::Get()->CustomMovementModeDefinitions.Find(InCustomMovementMode))
	{
		return *Tag;
	}
	return FGameplayTag::EmptyTag;
}

UNipcatCustomMovementMode* UNipcatCharacterMovementComponent::FindCustomMovementMode(uint8 InCustomMovementMode) const
{
	if (const auto Tag = UNipcatBasicGameplayDeveloperSettings::Get()->CustomMovementModeDefinitions.Find(InCustomMovementMode))
	{
		if (const auto Mode = CustomMovementModes.Find(*Tag))
		{
			return *Mode;
		}
	}
	return nullptr;
}

UNipcatCustomMovementMode* UNipcatCharacterMovementComponent::FindCustomMovementModeByTag(
	FGameplayTag InCustomMovementModeTag) const
{
	
	if (const auto ModePtr = CustomMovementModes.Find(InCustomMovementModeTag))
	{
		return *ModePtr;
	}
	return nullptr;
}


void UNipcatCharacterMovementComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	ProcessRootMotionPostConvertToWorld.BindUObject(this, &UNipcatCharacterMovementComponent::OnProcessRootMotionPostConvertToWorld);
}

void UNipcatCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                                      FActorComponentTickFunction* ThisTickFunction)
{
	DeltaT = DeltaTime;
	BasedMovementUpdated = false;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UNipcatCharacterMovementComponent::CanAttemptJump() const
{
	return IsJumpAllowed() &&
		(!bWantsToCrouch || bCanJumpOnCrouching) &&
		(IsMovingOnGround() || IsFalling());
}

float UNipcatCharacterMovementComponent::BoostAirControl(float DeltaTime, float TickAirControl,
	const FVector& FallAcceleration)
{
	if (bAirControlBoostVelocityThresholdIncludeZ)
	{
		// Allow a burst of initial acceleration
		if (AirControlBoostMultiplier > 0.f && Velocity.SizeSquared() < FMath::Square(AirControlBoostVelocityThreshold))
		{
			TickAirControl = FMath::Min(1.f, AirControlBoostMultiplier * TickAirControl);
		}

		return TickAirControl;
	}
	return Super::BoostAirControl(DeltaTime, TickAirControl, FallAcceleration);
}

void UNipcatCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	FRotator Rotation;
	if (GetModifiedRotator(DeltaTime, Rotation))
	{
		MoveUpdatedComponent(FVector::ZeroVector, Rotation, /*bSweep*/ false);
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

bool UNipcatCharacterMovementComponent::GetModifiedRotator(float DeltaTime, FRotator& OutRotation) const
{
	UCharacterRotationModifier* CurrentModifier = GetRotationModifier();
	if (!CurrentModifier)
	{
		return false;
	}

	if (!HasValidData() || (!CharacterOwner->Controller && !bRunPhysicsWithNoController))
	{
		return false;
	}

	FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
	CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

	FRotator DesiredRotation = CurrentModifier->GetDesiredRotation(CharacterOwner, CurrentRotation, DeltaTime);

	if (ShouldRemainVertical())
	{
		DesiredRotation.Pitch = 0.f;
		DesiredRotation.Yaw = FRotator::NormalizeAxis(DesiredRotation.Yaw);
		DesiredRotation.Roll = 0.f;
	}
	else
	{
		DesiredRotation.Normalize();
	}


	// Accumulate a desired new rotation.
	constexpr float AngleTolerance = 1e-3f;

	const FRotator DeltaRot = GetDeltaRotation(DeltaTime);
	DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

	if (!CurrentRotation.Equals(DesiredRotation, AngleTolerance))
	{
		// PITCH
		if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
		{
			DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
		}

		// YAW
		if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
		{
			DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
		}

		// ROLL
		if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
		{
			DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
		}

		// Set the new rotation.
		DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
	}
	OutRotation = DesiredRotation;
	return true;
}

FRotator UNipcatCharacterMovementComponent::GetDeltaRotation(float DeltaTime) const
{
	FRotator ScaledRotationRate = RotationRate;
	if (const UCharacterRotationModifier* CurrentModifier = GetRotationModifier())
	{
		ScaledRotationRate *= CurrentModifier->GetRotationRateScale(GetCharacterOwner(), GetOwner()->GetActorRotation(), DeltaTime);
	}
	return FRotator(NipcatGetAxisDeltaRotation(ScaledRotationRate.Pitch, DeltaTime),
					NipcatGetAxisDeltaRotation(ScaledRotationRate.Yaw, DeltaTime),
					NipcatGetAxisDeltaRotation(ScaledRotationRate.Roll, DeltaTime));
}

float UNipcatCharacterMovementComponent::NipcatGetAxisDeltaRotation(float InAxisRotationRate, float DeltaTime) const
{
	// Values over 360 don't do anything, see FMath::FixedTurn. However we are trying to avoid giant floats from overflowing other calculations.
	return (InAxisRotationRate >= 0.f) ? FMath::Min(InAxisRotationRate * DeltaTime, 360.f) : 360.f;
}

UCharacterRotationModifier* UNipcatCharacterMovementComponent::GetRotationModifier() const
{
	if (!RotationModeModifierStack.IsEmpty())
	{
		return RotationModeModifierStack.Last();
	}

	return nullptr;
}

void UNipcatCharacterMovementComponent::AddRotationModifier(UCharacterRotationModifier* RotationModifier)
{
	if (!RotationModifier)
	{ 
		return;
	}

	RotationModifier->Initialize(GetCharacterOwner());

	if (RotationModeModifierStack.IsEmpty())
	{
		bOldAllowPhysicsRotationDuringAnimRootMotion = bAllowPhysicsRotationDuringAnimRootMotion;
	}
	
	RotationModeModifierStack.Add(RotationModifier);
	RotationModeModifierStack.StableSort([](UCharacterRotationModifier& A, UCharacterRotationModifier& B)
		{
			return A.Priority < B.Priority;
		}
	);

	bAllowPhysicsRotationDuringAnimRootMotion = RotationModeModifierStack.Last()->bEnabledDuringAnimRootMotion;
}


void UNipcatCharacterMovementComponent::RemoveRotationModifier(UCharacterRotationModifier* RotationModifier)
{
	RotationModeModifierStack.RemoveAll(
		[=](const UCharacterRotationModifier* Item) { return Item == RotationModifier; });

	if (RotationModeModifierStack.IsEmpty())
	{
		bAllowPhysicsRotationDuringAnimRootMotion = bOldAllowPhysicsRotationDuringAnimRootMotion;
	}
	else
	{
		bAllowPhysicsRotationDuringAnimRootMotion = RotationModeModifierStack.Last()->bEnabledDuringAnimRootMotion;
	}
}

FTransform UNipcatCharacterMovementComponent::OnProcessRootMotionPostConvertToWorld(const FTransform& InRootMotion, UCharacterMovementComponent* CharacterMovementComponent, float DeltaSeconds) const
{
	FTransform FinalRootMotion = InRootMotion;
	
	if (DisableEdgeProtectionTag.IsValid() && ASC->HasMatchingGameplayTag(DisableEdgeProtectionTag))
	{
		return FinalRootMotion;
	}

	
	const FVector Translation = FinalRootMotion.GetLocation();
	
	// 检测角色正前方是否有悬崖 这里不能使用RootMotion方向，不稳定
	FVector Direction;
	TArray<FHitResult> OutHits;
	FVector RootMotionDirection = FinalRootMotion.GetLocation().GetSafeNormal2D();
	if (RootMotionDirection.IsNearlyZero())
	{
		Direction = CharacterOwner->GetActorForwardVector();
	}
	else
	{
		TMap<float, FVector> NearestDirections;
		// F
		NearestDirections.Add(RootMotionDirection.Dot(CharacterOwner->GetActorForwardVector()), CharacterOwner->GetActorForwardVector());
		// B
		NearestDirections.Add(RootMotionDirection.Dot(-CharacterOwner->GetActorForwardVector()), -CharacterOwner->GetActorForwardVector());
		// L
		NearestDirections.Add(RootMotionDirection.Dot(-CharacterOwner->GetActorRightVector()),-CharacterOwner->GetActorRightVector() );
		// R
		NearestDirections.Add(RootMotionDirection.Dot(CharacterOwner->GetActorRightVector()), CharacterOwner->GetActorRightVector());
		TArray<float> Distances;
		NearestDirections.GenerateKeyArray(Distances);
		Direction = *NearestDirections.Find(FMath::Max(Distances));
	}
	
	const FVector Start = GetOwner()->GetActorLocation() + Direction * (EdgeProtectionFirstTraceDistance + GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius());
	const FVector End = Start - GetOwner()->GetActorLocation() + GetFeetLocation() - FVector(0, 0, EdgeProtectionFirstTraceHeight + GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius());

#if WITH_EDITOR
	if (bEdgeProtectionDebug)
	{
		// 绘制第一次射线检测
		DrawDebugLine(GetWorld(), Start, End, FColor::Red, false, 2 * DeltaSeconds);
	}
#endif

	ECollisionChannel CollisionChannel = UEngineTypes::ConvertToCollisionChannel(UNipcatBasicGameplayDeveloperSettings::Get()->FootIKTraceChannel);
	// 如果没打到，说明运动方向为悬崖
	if (!GetWorld()->LineTraceMultiByChannel(OutHits, Start, End, CollisionChannel))
	{
		TArray<FHitResult> SecondOutHits;
		const FVector SecondStart = GetFeetLocation() + Direction * (EdgeProtectionSecondTraceDistance + GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius()) - FVector(0, 0, EdgeProtectionSecondTraceHeight);
		const FVector SecondEnd = SecondStart - 2 * Direction * (EdgeProtectionSecondTraceDistance + GetCharacterOwner()->GetCapsuleComponent()->GetScaledCapsuleRadius());
		
#if WITH_EDITOR
		if (bEdgeProtectionDebug)
		{
			// 绘制第二次射线检测
			DrawSphereSweeps(GetWorld(), SecondStart, SecondEnd, EdgeProtectionSecondTraceSphereRadius, SecondOutHits, 2 * DeltaSeconds);
		}
#endif
		
		// 打第二次射线检测悬崖法线。使用Sphere以避免检测不到很薄的木板桥
		if (GetWorld()->SweepMultiByChannel(SecondOutHits, SecondStart, SecondEnd, FQuat::Identity, CollisionChannel, FCollisionShape::MakeSphere(EdgeProtectionSecondTraceSphereRadius)))
		{
			FHitResult SecondOutHit = SecondOutHits.Last();
			// 获取命中表面的Normal2D
			const FVector Normal = SecondOutHit.ImpactNormal.GetSafeNormal2D();
			// Translation在Normal2D方向上的分量
			const FVector NormalComponent = Normal * Translation.Size() * FVector::DotProduct(Translation.GetSafeNormal2D(), Normal);
			// Translation在切线方向上的分量，即修正后的速度方向
			const FVector TangentComponent = Translation - NormalComponent;
			FinalRootMotion.SetLocation(TangentComponent);

#if WITH_EDITOR
			if (bEdgeProtectionDebug)
			{
				// 绘制Normal
				DrawDebugDirectionalArrow(GetWorld(), SecondOutHit.ImpactPoint, SecondOutHit.ImpactPoint + Normal * 100, 10, FColor::Blue, false, 2 * DeltaSeconds, 0, 2);
			}
#endif

			// 在修正后的速度方向再进行一次检测，以避免滑出锐角边缘
			TArray<FHitResult> ThirdOutHits;
			const FVector ThirdStart = GetFeetLocation() + TangentComponent.GetSafeNormal2D() * EdgeProtectionFirstTraceDistance;
			const FVector ThirdEnd = ThirdStart - FVector(0, 0, EdgeProtectionFirstTraceHeight);

#if WITH_EDITOR
			if (bEdgeProtectionDebug)
			{
				// 绘制第三次射线检测
				DrawDebugLine(GetWorld(), ThirdStart, ThirdEnd, FColor:: Yellow, false, 2 * DeltaSeconds);
			}
#endif
			// 如果第三次射线检测没打到东西，则完全禁用水平方向的RootMotion
			if (!GetWorld()->LineTraceMultiByChannel(ThirdOutHits, ThirdStart, ThirdEnd, CollisionChannel))
			{
				FinalRootMotion.SetLocation(FVector(0, 0, FinalRootMotion.GetLocation().Z));
			}
		}
	}
	return FinalRootMotion;
	
}


FVector UNipcatCharacterMovementComponent::ConstrainAnimRootMotionVelocity(const FVector& RootMotionVelocity,
                                                                           const FVector& CurrentVelocity) const
{
	FVector NewVelocity = Super::ConstrainAnimRootMotionVelocity(RootMotionVelocity, CurrentVelocity);
	if (CurrentRootMotion.HasOverrideVelocity())
	{
		CurrentRootMotion.AccumulateOverrideRootMotionVelocity(DeltaT, *CharacterOwner, *this, NewVelocity);
	}
	if (CurrentRootMotion.HasAdditiveVelocity())
	{
		CurrentRootMotion.AccumulateAdditiveRootMotionVelocity(DeltaT, *CharacterOwner, *this, NewVelocity);
	}
	
	if (ASC)
	{
		FGameplayTagContainer SourceTagContainer;
		ASC->GetOwnedGameplayTags(SourceTagContainer);

		if (bDisableRootMotionScale && DisableRootMotionScaleRequirements.RequirementsMet(SourceTagContainer))
		{
			const FTransform ComponentToWorld = CharacterOwner->GetMesh()->GetComponentTransform();
			
			FVector ComponentSpaceVelocity = ComponentToWorld.InverseTransformVector(NewVelocity);
			ComponentSpaceVelocity *= ComponentToWorld.GetScale3D().Reciprocal();
			NewVelocity = ComponentToWorld.TransformVector(ComponentSpaceVelocity);
		}
	}
	
	return OnProcessRootMotionPostConvertToWorld(FTransform(NewVelocity), nullptr, DeltaT).GetLocation();
}

void UNipcatCharacterMovementComponent::OnTeleported()
{
	Super::OnTeleported();
	if (OnTeleport.IsBound()) { OnTeleport.Broadcast(); }
}

void UNipcatCharacterMovementComponent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation,
                                                          const FVector& OldVelocity)
{
	if (OldVelocity.IsNearlyZero() && !Velocity.IsNearlyZero())
	{
		if (OnHasVelocity.IsBound())
		{
			OnHasVelocity.Broadcast();
		}
	}
	else if (!OldVelocity.IsNearlyZero() && Velocity.IsNearlyZero())
	{
		if (OnNoVelocity.IsBound())
		{
			OnNoVelocity.Broadcast();
		}
	}
}

FVector UNipcatCharacterMovementComponent::ComputeSlideVector(const FVector& Delta, const float Time,
	const FVector& Normal, const FHitResult& Hit) const
{
	if (bSlidingHoldSpeedMagnitude)
	{
		const double SpeedMagnitude = Delta.Length() * Time;
		const FVector Result = Super::ComputeSlideVector(Delta, Time, Normal, Hit);
		return Result.GetSafeNormal() * SpeedMagnitude;
	}
	return Super::ComputeSlideVector(Delta, Time, Normal, Hit);
}

FVector UNipcatCharacterMovementComponent::ConsumeInputVector()
{
	if (GetLastInputVector().IsNearlyZero() && !GetPendingInputVector().IsNearlyZero())
	{
		if (OnHasAcceleration.IsBound())
		{
			OnHasAcceleration.Broadcast();
		}
	}
	
	auto InputVector{Super::ConsumeInputVector()};
	
	FRotator DeltaRotation;
	if (!bIgnoreBaseRotation && TryGetMovementBaseDeltaRotation(DeltaRotation))
	{
		// Offset the input vector to keep it relative to the movement base.
		InputVector = DeltaRotation.RotateVector(InputVector);
	}

	return InputVector;
}

void UNipcatCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid,
	float BrakingDeceleration)
{
	FRotator DeltaRotation;
	if (!bIgnoreBaseRotation && TryGetMovementBaseDeltaRotation(DeltaRotation))
	{
		// Offset the velocity to keep it relative to the movement base.
		Velocity = DeltaRotation.RotateVector(Velocity);
	}
	
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
}

void UNipcatCharacterMovementComponent::UpdateBasedRotation(FRotator& FinalRotation, const FRotator& ReducedRotation)
{
	BasedMovementUpdated = true;
	DeltaBaseRotation = ReducedRotation;
	Super::UpdateBasedRotation(FinalRotation, ReducedRotation);
}

bool UNipcatCharacterMovementComponent::TryGetMovementBaseDeltaRotation(FRotator& DeltaRotation)
{
	if (BasedMovementUpdated)
	{
		DeltaRotation = DeltaBaseRotation;
		return true;
	}
	
	DeltaRotation = FRotator::ZeroRotator;

	if (!CharacterOwner)
	{
		return false;
	}
	const FBasedMovementInfo& BasedMovement = CharacterOwner->GetBasedMovement();
	if (!BasedMovement.MovementBase)
	{
		return false;
	}
	if (!MovementBaseUtility::IsDynamicBase(BasedMovement.MovementBase))
	{
		return false;
	}

	const auto* Body{BasedMovement.MovementBase->GetBodyInstance(BasedMovement.BoneName)};
	if (Body == nullptr)
	{
		return false;
	}
	
	FVector NewBaseLocation;
	FQuat NewBaseQuat;
	FQuat DeltaQuat = FQuat::Identity;
	if (!MovementBaseUtility::GetMovementBaseTransform(CharacterOwner->GetMovementBase(), BasedMovement.BoneName, NewBaseLocation, NewBaseQuat))
	{
		return false;
	}

	// Find change in rotation
	if (!OldBaseQuat.Equals(NewBaseQuat, 1e-8f))
	{
		DeltaQuat = NewBaseQuat * OldBaseQuat.Inverse();
	}

	DeltaRotation = DeltaQuat.Rotator();

	return true;
}

void UNipcatCharacterMovementComponent::ComputeFloorDist(const FVector& CapsuleLocation, float LineDistance,
	float SweepDistance, FFindFloorResult& OutFloorResult, float SweepRadius,
	const FHitResult* DownwardSweepResult) const
{
	// TODO Copied with modifications from UCharacterMovementComponent::ComputeFloorDist().
	// TODO After the release of a new engine version, this code should be updated to match the source code.

	// ReSharper disable All

	// UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("[Role:%d] ComputeFloorDist: %s at location %s"), (int32)CharacterOwner->GetLocalRole(), *GetNameSafe(CharacterOwner), *CapsuleLocation.ToString());
	OutFloorResult.Clear();

	float PawnRadius, PawnHalfHeight;
	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	bool bSkipSweep = false;
	if (DownwardSweepResult != NULL && DownwardSweepResult->IsValidBlockingHit())
	{
		// Only if the supplied sweep was vertical and downward.
		const bool bIsDownward = GetGravitySpaceZ(DownwardSweepResult->TraceStart - DownwardSweepResult->TraceEnd) > 0;
		const bool bIsVertical = ProjectToGravityFloor(DownwardSweepResult->TraceStart - DownwardSweepResult->TraceEnd).SizeSquared() <= UE_KINDA_SMALL_NUMBER;
		if (bIsDownward && bIsVertical)
		{
			// Reject hits that are barely on the cusp of the radius of the capsule
			if (IsWithinEdgeTolerance(DownwardSweepResult->Location, DownwardSweepResult->ImpactPoint, PawnRadius))
			{
				// Don't try a redundant sweep, regardless of whether this sweep is usable.
				bSkipSweep = true;

				const bool bIsWalkable = IsWalkable(*DownwardSweepResult);
				const float FloorDist = UE_REAL_TO_FLOAT(GetGravitySpaceZ(CapsuleLocation - DownwardSweepResult->Location));
				OutFloorResult.SetFromSweep(*DownwardSweepResult, FloorDist, bIsWalkable);

				if (bIsWalkable)
				{
					// Use the supplied downward sweep as the floor hit result.
					return;
				}
			}
		}
	}

	// We require the sweep distance to be >= the line distance, otherwise the HitResult can't be interpreted as the sweep result.
	if (SweepDistance < LineDistance)
	{
		ensure(SweepDistance >= LineDistance);
		return;
	}

	bool bBlockingHit = false;

	FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(ComputeFloorDist), false, CharacterOwner);
	// Having a character base on a component within a cluster union will cause replication problems.
	// The issue is that ACharacter::SetBase() gets a GeometryCollectionComponent passed to it when standing on the DynamicPlatform
	// and that GC is never simulating, and since it's not simulating it's stopping the based movement flow there for simulated proxies.
	QueryParams.bTraceIntoSubComponents = true;
	QueryParams.bReplaceHitWithSubComponents = false;

	FCollisionResponseParams ResponseParam;
	InitCollisionParams(QueryParams, ResponseParam);
	const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

	// Sweep test
	if (!bSkipSweep && SweepDistance > 0.f && SweepRadius > 0.f)
	{
		// Use a shorter height to avoid sweeps giving weird results if we start on a surface.
		// This also allows us to adjust out of penetrations.
		const float ShrinkScale = 0.9f;
		const float ShrinkScaleOverlap = 0.1f;
		float ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScale);
		float TraceDist = SweepDistance + ShrinkHeight;
		FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(SweepRadius, PawnHalfHeight - ShrinkHeight);

		FHitResult Hit(1.f);
		bBlockingHit = FloorSweepTest(Hit, CapsuleLocation, CapsuleLocation + TraceDist * GetGravityDirection(), CollisionChannel, CapsuleShape, QueryParams, ResponseParam);

		if (bBlockingHit)
		{
			// Reject hits adjacent to us, we only care about hits on the bottom portion of our capsule.
			// Check 2D distance to impact point, reject if within a tolerance from radius.
			if (Hit.bStartPenetrating || !IsWithinEdgeTolerance(CapsuleLocation, Hit.ImpactPoint, CapsuleShape.Capsule.Radius))
			{
				// Use a capsule with a slightly smaller radius and shorter height to avoid the adjacent object.
				// Capsule must not be nearly zero or the trace will fall back to a line trace from the start point and have the wrong length.
				CapsuleShape.Capsule.Radius = FMath::Max(0.f, CapsuleShape.Capsule.Radius - SWEEP_EDGE_REJECT_DISTANCE - UE_KINDA_SMALL_NUMBER);
				if (!CapsuleShape.IsNearlyZero())
				{
					ShrinkHeight = (PawnHalfHeight - PawnRadius) * (1.f - ShrinkScaleOverlap);
					TraceDist = SweepDistance + ShrinkHeight;
					CapsuleShape.Capsule.HalfHeight = FMath::Max(PawnHalfHeight - ShrinkHeight, CapsuleShape.Capsule.Radius);
					Hit.Reset(1.f, false);

					bBlockingHit = FloorSweepTest(Hit, CapsuleLocation, CapsuleLocation + TraceDist * GetGravityDirection(), CollisionChannel, CapsuleShape, QueryParams, ResponseParam);
				}
			}

			// Reduce hit distance by ShrinkHeight because we shrank the capsule for the trace.
			// We allow negative distances here, because this allows us to pull out of penetrations.
			const float MaxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, PawnRadius);
			const float SweepResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

			OutFloorResult.SetFromSweep(Hit, SweepResult, false);
			if (Hit.IsValidBlockingHit() && IsWalkable(Hit))
			{
				if (SweepResult <= SweepDistance)
				{
					// Hit within test distance.
					OutFloorResult.bWalkableFloor = true;
					return;
				}
			}
		}
	}

	// Since we require a longer sweep than line trace, we don't want to run the line trace if the sweep missed everything.
	// We do however want to try a line trace if the sweep was stuck in penetration.
	if (!OutFloorResult.bBlockingHit && !OutFloorResult.HitResult.bStartPenetrating)
	{
		OutFloorResult.FloorDist = SweepDistance;
		return;
	}

	// Line trace
	if (LineDistance > 0.f)
	{
		const float ShrinkHeight = PawnHalfHeight;
		const FVector LineTraceStart = CapsuleLocation;
		const float TraceDist = LineDistance + ShrinkHeight;
		const FVector Down = TraceDist * GetGravityDirection();
		QueryParams.TraceTag = SCENE_QUERY_STAT_NAME_ONLY(FloorLineTrace);

		FHitResult Hit(1.f);
		bBlockingHit = GetWorld()->LineTraceSingleByChannel(Hit, LineTraceStart, LineTraceStart + Down, CollisionChannel, QueryParams, ResponseParam);

		if (bBlockingHit)
		{
			if (Hit.Time > 0.f)
			{
				// Reduce hit distance by ShrinkHeight because we started the trace higher than the base.
				// We allow negative distances here, because this allows us to pull out of penetrations.
				const float MaxPenetrationAdjust = FMath::Max(MAX_FLOOR_DIST, PawnRadius);
				const float LineResult = FMath::Max(-MaxPenetrationAdjust, Hit.Time * TraceDist - ShrinkHeight);

				OutFloorResult.bBlockingHit = true;
				if (LineResult <= LineDistance && IsWalkable(Hit))
				{
					// Keep the data from the previous sweep test, which will be required later to properly resolve
					// penetration in the UCharacterMovementComponent::PhysWalking() function. By default, penetration
					// is only resolved when line trace starts in penetration and the sweep test is ignored.

					const auto NormalPrevious{OutFloorResult.HitResult.Normal};
					const auto PenetrationDepthPrevious{OutFloorResult.HitResult.PenetrationDepth};
					const auto bStartPenetratingPrevious{OutFloorResult.HitResult.bStartPenetrating};
					const auto HitObjectHandlePrevious{OutFloorResult.HitResult.HitObjectHandle};

					OutFloorResult.SetFromLineTrace(Hit, OutFloorResult.FloorDist, LineResult, true);

					OutFloorResult.HitResult.Normal = NormalPrevious;
					OutFloorResult.HitResult.PenetrationDepth = PenetrationDepthPrevious;
					OutFloorResult.HitResult.bStartPenetrating = bStartPenetratingPrevious;
					OutFloorResult.HitResult.HitObjectHandle = HitObjectHandlePrevious;

					return;
				}
			}
		}
	}

	// No hits were acceptable.
	OutFloorResult.bWalkableFloor = false;

	// ReSharper restore All
}

void UNipcatCharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	// SCOPE_CYCLE_COUNTER(STAT_CharPhysWalking);

if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->GetController() && !bRunPhysicsWithNoController && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && (CharacterOwner->GetLocalRole() != ROLE_SimulatedProxy)))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	if (!UpdatedComponent->IsQueryCollisionEnabled())
	{
		SetMovementMode(MOVE_Walking);
		return;
	}

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	const EMovementMode StartingMovementMode = MovementMode;
	const uint8 StartingCustomMovementMode = CustomMovementMode;

	// Perform the move
	while ( (remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->GetController() || bRunPhysicsWithNoController || HasAnimRootMotion() || CurrentRootMotion.HasOverrideVelocity() || (CharacterOwner->GetLocalRole() == ROLE_SimulatedProxy)) )
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Save current values
		UPrimitiveComponent * const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;

		RestorePreAdditiveRootMotionVelocity();

		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		const FVector OldVelocity = Velocity;
		Acceleration = FVector::VectorPlaneProject(Acceleration, -GetGravityDirection());
		
		static const auto* EnsureAlwaysEnabledConsoleVariable{
			IConsoleManager::Get().FindConsoleVariable(TEXT("p.LedgeMovement.ApplyDirectMove"))
		};
		check(EnsureAlwaysEnabledConsoleVariable != nullptr)
		
		// Apply acceleration
		const bool bSkipForLedgeMove = bTriedLedgeMove && EnsureAlwaysEnabledConsoleVariable->GetBool();
		if( !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && !bSkipForLedgeMove )
		{
			CalcVelocity(timeTick, GroundFriction, false, GetMaxBrakingDeceleration());
		}
		
		ApplyRootMotionToVelocity(timeTick);
		
		if (MovementMode != StartingMovementMode || CustomMovementMode != StartingCustomMovementMode)
		{
			// Root motion could have taken us out of our current mode
			// No movement has taken place this movement tick so we pass on full time/past iteration count
			StartNewPhysics(remainingTime+timeTick, Iterations-1);
			return;
		}

		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;

		if ( bZeroDelta )
		{
			remainingTime = 0.f;
		}
		else
		{
			// try to move forward
			MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);

			if (IsSwimming()) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
			else if (MovementMode != StartingMovementMode || CustomMovementMode != StartingCustomMovementMode)
			{
				// pawn ended up in a different mode, probably due to the step-up-and-over flow
				// let's refund the estimated unused time (if any) and keep moving in the new mode
				const float DesiredDist = Delta.Size();
				if (DesiredDist > UE_KINDA_SMALL_NUMBER)
				{
					const float ActualDist = ProjectToGravityFloor(UpdatedComponent->GetComponentLocation() - OldLocation).Size();
					remainingTime += timeTick * (1.f - FMath::Min(1.f,ActualDist/DesiredDist));
				}
				StartNewPhysics(remainingTime,Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
		}

		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if ( bCheckLedges && !CurrentFloor.IsWalkableFloor() )
		{
			// calculate possible alternate movement
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, OldFloor);
			if ( !NewDelta.IsZero() )
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta/timeTick;
				remainingTime += timeTick;
				Iterations--;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ( (bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					HandleWalkingOffLedge(OldFloor.HitResult.ImpactNormal, OldFloor.HitResult.Normal, OldLocation, timeTick);
					if (IsMovingOnGround())
					{
						
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}
						
				AdjustFloorHeight();
				SetBaseFromFloor(CurrentFloor);
			}
			// Always resolve penetration, even if the floor is walkable.

			if ((bAllowImprovedPenetrationAdjustment || !CurrentFloor.IsWalkableFloor()) &&
				CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + MAX_FLOOR_DIST * -GetGravityDirection();
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, UpdatedComponent->GetComponentQuat());
				bForceNextFloorCheck = true;
			}

			// check if just entered water
			if ( IsSwimming() )
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsQueryCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(OldFloor, CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump) )
				{
					return;
				}
				bCheckedFall = true;
			}
		}


		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround())
		{
			// Make velocity reflect actual move
			if( !bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity() && timeTick >= MIN_TICK_TIME)
			{
				// TODO-RootMotionSource: Allow this to happen during partial override Velocity, but only set allowed axes?
				Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / timeTick;
				MaintainHorizontalGroundVelocity();
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (UpdatedComponent->GetComponentLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}	
	}

	if (IsMovingOnGround())
	{
		MaintainHorizontalGroundVelocity();
	}
}

void UNipcatCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	Super::PhysCustom(deltaTime, Iterations);
	if (const auto FoundMovementMode = FindCustomMovementMode(CustomMovementMode))
	{
		FoundMovementMode->OnUpdate(this, deltaTime);
	}
}

void UNipcatCharacterMovementComponent::SetMovementMode(EMovementMode NewMovementMode, uint8 NewCustomMode)
{
	if (NewMovementMode != MOVE_Custom)
	{
		NewCustomMode = 0;
	}

	const auto OldCustomMovementMode = FindCustomMovementMode(CustomMovementMode);
	const auto NewCustomMovementMode = FindCustomMovementMode(NewCustomMode);

	bool OldMovementModeExited = false;
	bool NewMovementModeEntered = false;
	
	if (MovementMode == MOVE_Custom && (NewMovementMode != MOVE_Custom || NewCustomMode != CustomMovementMode))
	{
		if (OldCustomMovementMode)
		{
			const FGameplayTag OldCustomMovementModeTag = FindCustomMovementModeTag(CustomMovementMode);
			if (OldCustomMovementModeTag.IsValid())
			{
				UAbilitySystemBlueprintLibrary::RemoveLooseGameplayTags(GetOwner(),FGameplayTagContainer(OldCustomMovementModeTag));
			}
			
			OldCustomMovementMode->PreExit(this);
			OldMovementModeExited = true;
		}
	}

	if (NewMovementMode == MOVE_Custom && (MovementMode != MOVE_Custom || NewCustomMode != CustomMovementMode))
	{
		if (NewCustomMovementMode)
		{
			(NewCustomMovementMode)->PreEnter(this);
			NewMovementModeEntered = true;
		}
	}
	
	Super::SetMovementMode(NewMovementMode, NewCustomMode);
	
	if (OldCustomMovementMode && OldMovementModeExited)
	{
		(OldCustomMovementMode)->PostExit(this);
	}

	if (NewCustomMovementMode && NewMovementModeEntered)
	{
		const FGameplayTag NewCustomMovementModeTag = FindCustomMovementModeTag(CustomMovementMode);
		if (NewCustomMovementModeTag.IsValid())
		{
			UAbilitySystemBlueprintLibrary::AddLooseGameplayTags(GetOwner(),FGameplayTagContainer(NewCustomMovementModeTag));
		}
		
		NewCustomMovementMode->PostEnter(this);
	}
}

void UNipcatCharacterMovementComponent::AddCustomMovementInput(FVector InputValue, float DeltaTime)
{
	if (MovementMode == MOVE_Custom)
	{
		if (const auto FoundMovementMode = FindCustomMovementMode(CustomMovementMode))
		{
			FoundMovementMode->AddCustomMovementInput(this, InputValue, DeltaTime);
		}
	}
}

FRotator UNipcatCharacterMovementComponent::ComputeOrientToMovementRotation(const FRotator& CurrentRotation, float DeltaTime, FRotator& DeltaRotation) const
{
	if (MovementMode == MOVE_Custom)
	{
		return CurrentRotation;
	}
	return Super::ComputeOrientToMovementRotation(CurrentRotation, DeltaTime, DeltaRotation);
}

void UNipcatCharacterMovementComponent::HandleImpact(const FHitResult& Hit, float TimeSlice, const FVector& MoveDelta)
{
	// 1. 基本检查：撞到的是不是 Character
	ACharacter* OtherChar = Cast<ACharacter>(Hit.GetActor());
	if (!OtherChar) return;

	UNipcatCharacterMovementComponent* OtherCMC = Cast<UNipcatCharacterMovementComponent>(OtherChar->GetCharacterMovement());
	if (!OtherCMC) return;

	if (!CanPushOthers() || !OtherCMC->CanBePushed())
	{
		return;
	}
	
	// 2. 检查我们是否在主动推挤 (防止被动被撞时触发)
	// 只有当我们自己的速度方向是指向对方时，才算推挤
	// MoveDelta 是这一帧原本打算移动的量
	if (FVector::DotProduct(MoveDelta, Hit.Normal) > -0.1f) 
	{
		// 我们的移动方向没有朝向墙壁(对方)，说明可能是擦肩而过，或者对方撞我
		return; 
	}

	// 3. 获取质量
	float MyMass = CharacterOwner->GetCapsuleComponent()->CalculateMass();
	float OtherMass = OtherChar->GetCapsuleComponent()->CalculateMass();
	if (OtherMass <= 0.f) OtherMass = 1.f; // 防止除0

	// 4. 计算推挤权重 (质量比)
	// 比如：我100kg，他50kg -> Ratio = 100 / (100+50) = 0.66 (我主导)
	// 比如：我50kg，他100kg -> Ratio = 50 / (150) = 0.33 (推不动)
	float PushRatio = MyMass / (MyMass + OtherMass);
    
	// 如果比率太小（推石头），直接放弃，视为撞墙
	if (PushRatio < 0.1f) return;

	// 5. 核心逻辑：直接位移 (解决 RootMotion 和 滑行问题)
    
	// 我们原本这一帧想走 MoveDelta，但被挡住了。
	// 我们根据权重，强行把对方挤开。
	// 注意：只在水平面上挤
	FVector PushMove = MoveDelta.ProjectOnTo(Hit.Normal); 
	PushMove.Z = 0.f;

	// 我们施加给对方的位移量
	// 这里乘以 PushRatio 是模拟“越重推得越快”，或者你可以简化为固定距离
	FVector StepToPush = PushMove * PushRatio;
	StepToPush = OtherCMC->OnProcessRootMotionPostConvertToWorld(FTransform(FRotator::ZeroRotator, StepToPush, FVector::OneVector), this, TimeSlice).GetTranslation();

	// 【关键点】：使用 SafeMoveUpdatedComponent (或 MoveUpdatedComponent)
	// 这不会改变对方的 Velocity，而是直接修改 Transform
	// 这允许对方同时播放 RootMotion，两个位移会叠加
	FHitResult PushHit;
	OtherCMC->MoveUpdatedComponent(StepToPush, OtherChar->GetActorQuat(), true, &PushHit);
	
	// 在推完对方后，让他尝试贴合地面，防止推到半空中
	OtherCMC->UpdateFloorFromAdjustment();
	
	// 6. 可选：反作用力 (让自己减速)
	// 因为我们把对方推走了，我们可以尝试“再次移动自己”填补空缺
	// 这样看起来就是紧贴着的
	// Super::HandleImpact 已经把我们停下了，我们可以尝试再走一点
	// 但通常不需要，下一帧 Input 会继续驱动我们往前
	
	Super::HandleImpact(Hit, TimeSlice, MoveDelta);
	
}

bool UNipcatCharacterMovementComponent::CanPushOthers() const
{
	if (bEnablePushOthers && ASC)
	{
		FGameplayTagContainer TagContainer;
		ASC->GetOwnedGameplayTags(TagContainer);
		return CanPushOthersRequirements.RequirementsMet(TagContainer);
	}
	return bEnablePushOthers;
}

bool UNipcatCharacterMovementComponent::CanBePushed() const
{
	if (bEnableBePushed && ASC)
	{
		FGameplayTagContainer TagContainer;
		ASC->GetOwnedGameplayTags(TagContainer);
		return CanBePushedRequirements.RequirementsMet(TagContainer);
	}
	return bEnableBePushed;
}
