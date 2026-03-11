// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatGameplayEffectComponent_BlueprintWrapper.h"

#include "GameplayEffect.h"
#include "GameplayEffectTypes.h"
#include "NipcatGameplayEffectComponent_BlueprintBase.h"

void UNipcatGameplayEffectComponent_BlueprintWrapper::PreGameplayEffectApply(
	FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec,
	FPredictionKey& PredictionKey) const
{
	if (!BlueprintEffectComponent)
	{
		return;
	}
	const FGameplayEffectSpecHandle Handle = FGameplayEffectSpecHandle(new FGameplayEffectSpec(GESpec));
	BlueprintEffectComponent->PreGameplayEffectApply(ActiveGEContainer.Owner, Handle);
}

bool UNipcatGameplayEffectComponent_BlueprintWrapper::CanGameplayEffectApply(
	const FActiveGameplayEffectsContainer& ActiveGEContainer, const FGameplayEffectSpec& GESpec) const
{
	if (!BlueprintEffectComponent)
	{
		return Super::CanGameplayEffectApply(ActiveGEContainer, GESpec);
	}
	const FGameplayEffectSpecHandle Handle = FGameplayEffectSpecHandle(new FGameplayEffectSpec(GESpec));
	return BlueprintEffectComponent->CanGameplayEffectApply(ActiveGEContainer.Owner, Handle);
}

bool UNipcatGameplayEffectComponent_BlueprintWrapper::OnActiveGameplayEffectAdded(
	FActiveGameplayEffectsContainer& ActiveGEContainer, FActiveGameplayEffect& ActiveGE) const
{
	if (!BlueprintEffectComponent)
	{
		return Super::OnActiveGameplayEffectAdded(ActiveGEContainer, ActiveGE);
	}
	Super::OnActiveGameplayEffectAdded(ActiveGEContainer, ActiveGE);
	return BlueprintEffectComponent->OnActiveGameplayEffectAdded(ActiveGEContainer.Owner, ActiveGE);
}

void UNipcatGameplayEffectComponent_BlueprintWrapper::OnGameplayEffectExecuted(
	FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec,
	FPredictionKey& PredictionKey) const
{
	if (!BlueprintEffectComponent)
	{
		return;
	}
	const FGameplayEffectSpecHandle Handle = FGameplayEffectSpecHandle(new FGameplayEffectSpec(GESpec));
	BlueprintEffectComponent->OnGameplayEffectExecuted(ActiveGEContainer.Owner, Handle);
}

void UNipcatGameplayEffectComponent_BlueprintWrapper::OnGameplayEffectApplied(
	FActiveGameplayEffectsContainer& ActiveGEContainer, FGameplayEffectSpec& GESpec,
	FPredictionKey& PredictionKey) const
{
	if (!BlueprintEffectComponent)
	{
		return;
	}
	const FGameplayEffectSpecHandle Handle = FGameplayEffectSpecHandle(new FGameplayEffectSpec(GESpec));
	BlueprintEffectComponent->OnGameplayEffectApplied(ActiveGEContainer.Owner, Handle);
}

void UNipcatGameplayEffectComponent_BlueprintWrapper::OnGameplayEffectStackChange(FActiveGameplayEffectHandle Handle,
	int32 NewCount, int32 OldCount) const
{
	if (!BlueprintEffectComponent)
	{
		return;
	}
	BlueprintEffectComponent->OnGameplayEffectStackChange(Handle, NewCount, OldCount);
}

void UNipcatGameplayEffectComponent_BlueprintWrapper::OnActiveGameplayEffectRemoved(
	const FGameplayEffectRemovalInfo& RemovalInfo, FActiveGameplayEffectsContainer* ActiveGEContainer) const
{
	if (!BlueprintEffectComponent)
	{
		return;
	}
	BlueprintEffectComponent->OnActiveGameplayEffectRemoved(RemovalInfo, ActiveGEContainer->Owner);
}
