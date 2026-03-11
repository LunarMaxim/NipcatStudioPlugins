// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatWeightedTargetHandler.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "LockOnTargetDefines.h"
#include "LockOnTargetComponent.h"
#include "NipcatLockOnTargetComponent.h"
#include "TargetManager.h"
#include "TargetComponent.h"

void UNipcatWeightedTargetHandler::HandleTargetUnlock(ETargetUnlockReason UnlockReason)
{
	LOT_SCOPED_EVENT(WTH_HandleTargetUnlock);

	bool IsSoftLock = false;
	const auto NipcatLockOnTargetComponent = Cast<UNipcatLockOnTargetComponent>(GetLockOnTargetComponent());
	if (NipcatLockOnTargetComponent)
	{
		IsSoftLock = NipcatLockOnTargetComponent->TargetExceptionWhenSoftLock;
	}
	
	if (TryLockOnNewTargetAfterClearTarget && IsAnyUnlockReasonSet(AutoFindTargetFlags, UnlockReason) && (TryLockOnNewTargetAfterClearTargetForSoftLock || !IsSoftLock))
	{
		TryFindTarget(true);

		if (GetLockOnTargetComponent()->IsTargetLocked())
		{
			if (IsSoftLock)
			{
				if (NipcatLockOnTargetComponent)
				{
					NipcatLockOnTargetComponent->SetSoftLockEnabled(true);
				}
			}
		}
	}
	else
	{
		GetLockOnTargetComponent()->ClearTargetManual();
	}
}

void UNipcatWeightedTargetHandler::PerformPrimarySamplingPass(FFindTargetContext& Context,
	TArray<FTargetContext>& OutTargetsData)
{
	const TSet<UTargetComponent*>& RegisteredTargets = UTargetManager::Get(*GetWorld()).GetRegisteredTargets();

	OutTargetsData.Empty();
	OutTargetsData.Reserve(RegisteredTargets.Num());

	for (UTargetComponent* const Target : RegisteredTargets)
	{
		if (ShouldSkipTargetPrimaryPass(Context, Target))
		{
			continue;
		}

		for (const FName TargetSocket : Target->GetSockets())
		{
			const FTargetInfo CurrentTarget = { Target, TargetSocket };

			//Skip already captured Target and Socket.
			if (Context.CapturedTarget.Target == CurrentTarget)
			{
				continue;
			}

			FTargetContext TargetContext = CreateTargetContext(Context, CurrentTarget);

			//Check if in view cone.
			const float DeltaConeAngle = FMath::RadiansToDegrees(FMath::Acos(Context.ViewRotationMatrix.GetScaledAxis(EAxis::X) | TargetContext.Direction));

			if (DeltaConeAngle > ViewConeAngle)
			{
				continue;
			}

			/** Begin Nipcat Modify */
			
			//Check Team Attitude
			if (const auto SourceTeamAgent = Cast<IGenericTeamAgentInterface>(Context.InstigatorPawn))
			{
				if (Target->GetOwner()->Implements<UGenericTeamAgentInterface>())
				{
					if (!FAISenseAffiliationFilter::ShouldSenseTeam(SourceTeamAgent, *Target->GetOwner(), DetectionByAffiliation.GetAsFlags()))
					{
						continue;
					}
				}
			}

			if (!Context.PlayerInputDirection.IsZero() )
			{
				CalcDeltaAngle2D(Context, TargetContext);
				bool ForceApplyPlayerInputAngularRangeLimit = false;
				
				if (UseEnableSoftLockInputAngularLimitRequirements)
				{
					if (const auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Context.InstigatorPawn.Get()))
					{
						ForceApplyPlayerInputAngularRangeLimit = EnableSoftLockInputAngularLimitRequirements.RequirementsMet(ASC->GetOwnedGameplayTags());
					}
				}
				
				if (Context.Mode == EFindTargetContextMode::Switch || ForceApplyPlayerInputAngularRangeLimit)
				{
					if (TargetContext.DeltaAngle2D > PlayerInputAngularRange)
					{
						continue;
					}
				}
			}
			
			/** End Nipcat Modify */

			OutTargetsData.Add(TargetContext);
		}
	}
}

void UNipcatWeightedTargetHandler::CalcDeltaAngle2D(const FFindTargetContext& Context,
                                                    FTargetContext& OutTargetContext) const
{
	if (Context.Mode == EFindTargetContextMode::Switch)
	{
		Super::CalcDeltaAngle2D(Context, OutTargetContext);
	}
	else
	{
		OutTargetContext.DeltaDirection2D = FVector2D((OutTargetContext.Location - GetLockOnTargetComponent()->GetOwner()->GetActorLocation()).GetSafeNormal2D());
		OutTargetContext.DeltaAngle2D = FMath::RadiansToDegrees(FMath::Acos(OutTargetContext.DeltaDirection2D | Context.PlayerInputDirection));
	}
}

float UNipcatWeightedTargetHandler::CalculateTargetWeight_Implementation(const FFindTargetContext& Context,
	const FTargetContext& TargetContext) const
{
	float OutWeight = 0.f;

	const float WeightSum = DistanceWeight + DeltaAngleWeight + PlayerInputWeight + TargetPriorityWeight;

	if (!FMath::IsNearlyZero(WeightSum))
	{
		const float WeightNormalizer = 1.f / WeightSum;

		auto ApplyFactor = [&OutWeight, WeightNormalizer, this](float Weight, float Ratio)
		{
			const float Factor = FMath::Clamp(Ratio, MinimumFactorThreshold, 1.f);
			OutWeight += PureDefaultWeight * Weight * WeightNormalizer * Factor;
		};

		if (DistanceWeight > UE_KINDA_SMALL_NUMBER)
		{
			const float Ratio = TargetContext.DistanceSq / FMath::Square(DistanceMaxFactor);
			ApplyFactor(DistanceWeight, Ratio);
		}

		if (DeltaAngleWeight > UE_KINDA_SMALL_NUMBER)
		{
			const float Ratio = FMath::RadiansToDegrees(FMath::Acos(TargetContext.Direction | Context.SolverViewDirection)) / DeltaAngleMaxFactor;
			ApplyFactor(DeltaAngleWeight, Ratio);
		}

		/** Begin Nipcat Modify */
		if (PlayerInputWeight > UE_KINDA_SMALL_NUMBER)
		{
			const float Ratio = TargetContext.DeltaAngle2D / PlayerInputAngularRange;
			ApplyFactor(PlayerInputWeight, Ratio);
		}
		/** End Nipcat Modify */

		if (TargetPriorityWeight > UE_KINDA_SMALL_NUMBER)
		{
			ApplyFactor(TargetPriorityWeight, TargetContext.Target->Priority);
		}
	}

	return OutWeight;
}
