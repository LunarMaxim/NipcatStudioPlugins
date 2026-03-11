// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAdditionalEffectsGameplayEffectComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "Misc/DataValidation.h"

#define LOCTEXT_NAMESPACE "AdditionalEffectsGameplayEffectComponent"

void UNipcatAdditionalEffectsGameplayEffectComponent::PreGameplayEffectApply(FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec,
	FPredictionKey& PredictionKey) const
{
	ApplyExtraEffectToTarget(PreApplicationGameplayEffectsToSelf, ActiveGEContainer, GESpec, PredictionKey, true);
	ApplyExtraEffectToTarget(PreApplicationGameplayEffects, ActiveGEContainer, GESpec, PredictionKey);
}

void UNipcatAdditionalEffectsGameplayEffectComponent::OnGameplayEffectApplied(FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey) const
{
	if (!ensureMsgf(ActiveGEContainer.Owner, TEXT("OnGameplayEffectApplied is passed an ActiveGEContainer which lives within an ASC but that ASC was somehow null")))
	{
		return;
	}

	ApplyExtraEffectToTarget(OnApplicationGameplayEffectsToSelf, ActiveGEContainer, GESpec, PredictionKey, true);
	ApplyExtraEffectToTarget(OnApplicationGameplayEffects, ActiveGEContainer, GESpec, PredictionKey);
}

void UNipcatAdditionalEffectsGameplayEffectComponent::OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const
{
	Super::OnActiveGameplayEffectRemoved(RemovalInfo, ActiveGEContainer);
	
	FScopedActiveGameplayEffectLock ActiveScopeLock(*ActiveGEContainer);

	const FActiveGameplayEffect* ActiveGE = RemovalInfo.ActiveEffect;
	if (!ensure(ActiveGE))
	{
		UE_LOG(LogGameplayEffects, Error, TEXT("FGameplayEffectRemovalInfo::ActiveEffect was not populated in OnActiveGameplayEffectRemoved"));
		return;
	}

	UAbilitySystemComponent* ASC = ActiveGEContainer->Owner;
	if (!ensure(IsValid(ASC)))
	{
		UE_LOG(LogGameplayEffects, Error, TEXT("ActiveGEContainer was invalid in OnActiveGameplayEffectRemoved"));
		return;
	}

	// Determine the appropriate type of effect to apply depending on whether the effect is being prematurely removed or not
	const TArray<TSubclassOf<UGameplayEffect>>& ExpiryEffects = (RemovalInfo.bPrematureRemoval ? OnCompletePrematurely : OnCompleteNormal);

	// Mix-in the always-executing GameplayEffects
	TArray<TSubclassOf<UGameplayEffect>> AllGameplayEffects{ ExpiryEffects };
	AllGameplayEffects.Append(OnCompleteAlways);

	for (const TSubclassOf<UGameplayEffect>& CurExpiryEffect : AllGameplayEffects)
	{
		if (const UGameplayEffect* CurExpiryCDO = CurExpiryEffect.GetDefaultObject())
		{
			FGameplayEffectSpec NewSpec;
			NewSpec.InitializeFromLinkedSpec(CurExpiryCDO, ActiveGE->Spec);

			ASC->ApplyGameplayEffectSpecToSelf(NewSpec);
		}
	}
}

void UNipcatAdditionalEffectsGameplayEffectComponent::ApplyExtraEffectToTarget(TArray<FConditionalGameplayEffect> Effects, FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec, FPredictionKey& PredictionKey, bool bApplyToSelf ) const
{
	const float GELevel = GESpec.GetLevel();
	const FGameplayEffectContextHandle& GEContextHandle = GESpec.GetEffectContext();

	/** other effects that need to be applied to the target if this effect is successful */
	TArray<FGameplayEffectSpecHandle> TargetEffectSpecs;
	for (const FConditionalGameplayEffect& ConditionalEffect : Effects)
	{
		const UGameplayEffect* GameplayEffectDef = ConditionalEffect.EffectClass.GetDefaultObject();
		if (!GameplayEffectDef)
		{
			continue;
		}

		if (ConditionalEffect.CanApply(GESpec.CapturedSourceTags.GetActorTags(), GELevel))
		{
			FGameplayEffectSpecHandle SpecHandle;
			if (bOnApplicationCopyDataFromOriginalSpec)
			{
				SpecHandle = FGameplayEffectSpecHandle(new FGameplayEffectSpec());
				SpecHandle.Data->InitializeFromLinkedSpec(GameplayEffectDef, GESpec);
			}
			else
			{
				SpecHandle = ConditionalEffect.CreateSpec(GEContextHandle, GELevel);
			}

			if (ensure(SpecHandle.IsValid()))
			{
				TargetEffectSpecs.Add(SpecHandle);
			}
		}
	}

	// Add all of the dynamically linked ones that don't have any conditions applied to them
	// Note: I believe this functionality should be removed as its usage is dynamic and thus not 
	// synchronized between client/server.  I've deprecated the variable to help enforce this.
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
	TargetEffectSpecs.Append(GESpec.TargetEffectSpecs);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

	UAbilitySystemComponent& AppliedToASC = bApplyToSelf ? *GESpec.GetContext().GetInstigatorAbilitySystemComponent() : *ActiveGEContainer.Owner;
	for (const FGameplayEffectSpecHandle& TargetSpec : TargetEffectSpecs)
	{
		if (TargetSpec.IsValid())
		{
			AppliedToASC.ApplyGameplayEffectSpecToSelf(*TargetSpec.Data.Get(), PredictionKey);
		}
	}
}

#if WITH_EDITOR
EDataValidationResult UNipcatAdditionalEffectsGameplayEffectComponent::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (GetOwner()->DurationPolicy == EGameplayEffectDurationType::Instant)
	{
		const bool bHasOnCompleteEffects = (OnCompleteAlways.Num() + OnCompleteNormal.Num() + OnCompletePrematurely.Num() > 0);
		if (bHasOnCompleteEffects)
		{
			Context.AddError(FText::FormatOrdered(LOCTEXT("InstantDoesNotWorkWithOnComplete", "Instant GE will never receive OnComplete for {0}."), FText::FromString(GetClass()->GetName())));
			Result = EDataValidationResult::Invalid;
		}
	}
	else if (GetOwner()->Period.Value > 0.0f)
	{
		if (OnApplicationGameplayEffects.Num() > 0 || OnApplicationGameplayEffectsToSelf.Num() > 0)
		{
			Context.AddWarning(LOCTEXT("IsPeriodicAndHasOnApplication", "Periodic GE has OnApplicationGameplayEffects. Those GE's will only be applied once."));
		}
	}

	return Result;
}
#endif // WITH_EDITOR

#undef LOCTEXT_NAMESPACE
