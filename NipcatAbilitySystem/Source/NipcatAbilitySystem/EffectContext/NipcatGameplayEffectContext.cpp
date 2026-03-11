// Copyright Nipcat Studio All Rights Reserved.

#include "NipcatGameplayEffectContext.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Components/PrimitiveComponent.h"
#include "NipcatAbilitySystem/NipcatAbilitySystemComponent.h"

UE_DEFINE_GAMEPLAY_TAG(DamageHitResult_None, "DamageHitResult.None");


void UDamageInstance::BeginDestroy()
{
	if (InstigatorAbilitySystemComponent && InstigatorGameplayTagCountChangedDelegate.IsValid())
	{
		InstigatorAbilitySystemComponent->RegisterGenericGameplayTagEvent().Remove(InstigatorGameplayTagCountChangedDelegate);
	}
	UObject::BeginDestroy();
}

void UDamageInstance::SetTargetActor(AActor* InTargetActor)
{
	if (InTargetActor != TargetActor)
	{
		if (TargetAbilitySystemComponent)
		{
			TargetAbilitySystemComponent->RegisterGenericGameplayTagEvent().Remove(TargetGameplayTagCountChangedDelegate);
		}
		TargetOwnedTags = FGameplayTagContainer::EmptyContainer;
		
		TargetActor = InTargetActor;
		if (TargetActor)
		{
			TargetAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
			if (TargetAbilitySystemComponent)
			{
				TargetGameplayTagCountChangedDelegate = TargetAbilitySystemComponent->RegisterGenericGameplayTagEvent().AddUObject(
					this, &ThisClass::OnTargetGameplayTagCountChanged);
				TargetOwnedTags = TargetAbilitySystemComponent->GetOwnedGameplayTags();
			}
		}
	}
}

void UDamageInstance::SetHitResult(FHitResult InHitResult)
{
	HitResult = InHitResult;
	if (HitResult.PhysMaterial.IsValid())
	{
		HitResultSurfaceType = HitResult.PhysMaterial.Get()->SurfaceType;
	}
	EffectSpec.Data->GetContext().AddHitResult(InHitResult, true);
}

void UDamageInstance::OnInstigatorGameplayTagCountChanged(const FGameplayTag Tag, int32 Count)
{
	InstigatorOwnedTags = InstigatorAbilitySystemComponent->GetOwnedGameplayTags();
}

void UDamageInstance::OnTargetGameplayTagCountChanged(const FGameplayTag Tag, int32 Count)
{
	TargetOwnedTags = TargetAbilitySystemComponent->GetOwnedGameplayTags();
}

FNipcatGameplayEffectContext* FNipcatGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FNipcatGameplayEffectContext::StaticStruct()))
	{
		return (FNipcatGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FNipcatGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

FNipcatGameplayAbilityActorInfo* FNipcatGameplayAbilityActorInfo::ExtractActorInfo(FGameplayAbilityActorInfo* ActorInfo)
{
	if (ActorInfo)
	{
		return (FNipcatGameplayAbilityActorInfo*)ActorInfo;
	}
	return nullptr;
}

