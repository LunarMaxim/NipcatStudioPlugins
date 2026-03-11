// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatGameplayCueNotifyActor_Burst.h"
#include "Engine/World.h"
#include "TimerManager.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include "NipcatAbilitySystem/EffectContext/NipcatGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NipcatGameplayCueNotifyActor_Burst)


const float DefaultBurstLatentLifetime = 5.0f;


//////////////////////////////////////////////////////////////////////////
// AGameplayCueNotify_BurstLatent
//////////////////////////////////////////////////////////////////////////
ANipcatGameplayCueNotifyActor_Burst::ANipcatGameplayCueNotifyActor_Burst()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

	PrimaryActorTick.bStartWithTickEnabled = false;
	bAutoDestroyOnRemove = true;
	bAllowMultipleWhileActiveEvents = false;
	NumPreallocatedInstances = 3;
	
	Recycle();
}

bool ANipcatGameplayCueNotifyActor_Burst::OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	UWorld* World = GetWorld();

	TMap<FGameplayTag, UNipcatFXScript_BurstGameplayCue*> OverrideScripts;
	if (const FNipcatGameplayEffectContext* TypedContext = FNipcatGameplayEffectContext::ExtractEffectContext(Parameters.EffectContext))
	{
		if (const UDamageInstance* DamageInstance = TypedContext->DamageInstance.Get())
		{
			if (DamageInstance->DamageDefinition.bUseOverrideBurstEffects)
			{
				OverrideScripts = DamageInstance->DamageDefinition.OverrideBurstEffects.Effects;
			}
			if (DamageInstance->DamageDefinition.bUseAdditionalBurstEffects)
			{
				DamageInstance->DamageDefinition.AdditionalBurstEffects.ExecuteEffects(Target, Parameters, this, nullptr);
			}
		}
	}
	BurstEffects.ExecuteEffects(Target, Parameters, this, &OverrideScripts);

	// Handle GC removal by default. This is a simple default to handle all cases we can currently think of.
	// If we didn't do this, we'd be relying on every BurstLatent GC manually setting up its removal within BP graphs,
	// or some inference based on parameters.
	if (World)
	{
		const float Lifetime = FMath::Max<float>(AutoDestroyDelay, DefaultBurstLatentLifetime);
		World->GetTimerManager().SetTimer(FinishTimerHandle, this, &AGameplayCueNotify_Actor::GameplayCueFinishedCallback, Lifetime);
	}

	return false;
}

bool ANipcatGameplayCueNotifyActor_Burst::Recycle()
{
	ScriptInstances.Empty();
	return Super::Recycle();
}

