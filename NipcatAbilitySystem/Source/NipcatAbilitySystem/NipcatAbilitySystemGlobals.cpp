// Copyright Nipcat Studio All Rights Reserved.

#include "NipcatAbilitySystemGlobals.h"

#include "Attributes/NipcatAttributeSetInitter.h"
#include "EffectContext/NipcatGameplayEffectContext.h"
#include "GameFramework/Character.h"

UNipcatAbilitySystemGlobals::UNipcatAbilitySystemGlobals(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UNipcatAbilitySystemGlobals::ReloadAttributeDefaultValues()
{
	ReloadAttributeDefaults();
}

#if WITH_EDITOR
void UNipcatAbilitySystemGlobals::ReloadAttributeDefaultsOnPreBeginPIE(const bool bIsSimulatingInEditor)
{
	ReloadAttributeDefaults();
}
#endif


void UNipcatAbilitySystemGlobals::InitGlobalData()
{
	Super::InitGlobalData();
#if WITH_EDITOR
	if (GIsEditor)
	{
		FEditorDelegates::PreBeginPIE.AddUObject(this, &ThisClass::ReloadAttributeDefaultsOnPreBeginPIE);
	}
#endif
}

void UNipcatAbilitySystemGlobals::AllocAttributeSetInitter()
{
	GlobalAttributeSetInitter = TSharedPtr<FAttributeSetInitter>(new FNipcatAttributeSetInitter());
}

FGameplayAbilityActorInfo* UNipcatAbilitySystemGlobals::AllocAbilityActorInfo() const
{
	return new FNipcatGameplayAbilityActorInfo();
}

FGameplayEffectContext* UNipcatAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FNipcatGameplayEffectContext();
}

void UNipcatAbilitySystemGlobals::InitGameplayCueParameters(FGameplayCueParameters& CueParameters,
                                                            const FGameplayEffectContextHandle& EffectContext)
{
	Super::InitGameplayCueParameters(CueParameters, EffectContext);
	CueParameters.Instigator = EffectContext.GetInstigator();
	CueParameters.EffectCauser = EffectContext.GetEffectCauser();
	const FHitResult* HitResult = EffectContext.GetHitResult();
	if (!HitResult)
	{
		return;
	}
	CueParameters.PhysicalMaterial = HitResult->PhysMaterial;
	if (AActor* TargetActor = HitResult->GetActor())
	{
		if (const ACharacter* TargetCharacter = Cast<ACharacter>(TargetActor))
		{
			CueParameters.TargetAttachComponent = TargetCharacter->GetMesh();
		}
		else
		{
			CueParameters.TargetAttachComponent = TargetActor->GetRootComponent();
		}
	}
}

void UNipcatAbilitySystemGlobals::InitGameplayCueParameters_GESpec(FGameplayCueParameters& CueParameters,
                                                                   const FGameplayEffectSpec& Spec)
{
	Super::InitGameplayCueParameters_GESpec(CueParameters, Spec);
}

void UNipcatAbilitySystemGlobals::InitGameplayCueParameters(FGameplayCueParameters& CueParameters,
	const FGameplayEffectSpecForRPC& Spec)
{
	Super::InitGameplayCueParameters(CueParameters, Spec);
}
