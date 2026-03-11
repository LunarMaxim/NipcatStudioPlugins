// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatFXDefinition_BurstGameplayCue.h"

#include "NipcatAbilitySystem/GameplayCue/NipcatGameplayCueNotifyActor_Burst.h"


void UNipcatFXScript_BurstGameplayCue::OnExecute_Implementation(AActor* MyTarget,
                                                                const FGameplayCueParameters& Parameters)
{
}

UWorld* UNipcatFXScript_BurstGameplayCue::GetWorld() const
{
	if (GameplayCueNotifyActor)
	{
		return GameplayCueNotifyActor->GetWorld();
	}
	return UObject::GetWorld();
}

void FNipcatFXDefinition_BurstGameplayCue::ExecuteEffects(AActor* MyTarget, const FGameplayCueParameters& Parameters,
                                                          AGameplayCueNotify_Actor* InGameplayCueNotifyActor,
                                                          TMap<FGameplayTag, UNipcatFXScript_BurstGameplayCue*>*
                                                          OverrideScripts) const
{
	for (const auto& Pair : Effects)
	{
		if (UNipcatFXScript_BurstGameplayCue** ScriptPtr = OverrideScripts->Find(Pair.Key))
		{
			const UNipcatFXScript_BurstGameplayCue* Script = *ScriptPtr;
			ExecuteEffect(Script, Parameters, MyTarget, InGameplayCueNotifyActor);
		}
		else if (Pair.Value)
		{
			ExecuteEffect(Pair.Value, Parameters, MyTarget, InGameplayCueNotifyActor);
		}
	}
}

void FNipcatFXDefinition_BurstGameplayCue::ExecuteEffect(const UNipcatFXScript_BurstGameplayCue* Script,
                                                         const FGameplayCueParameters& Parameters,
                                                         AActor* MyTarget,
                                                         AGameplayCueNotify_Actor* InGameplayCueNotifyActor)
{
	UNipcatFXScript_BurstGameplayCue* ScriptInstance = DuplicateObject(Script, InGameplayCueNotifyActor);
	if (ANipcatGameplayCueNotifyActor_Burst* NipcatNotifyActor = Cast<ANipcatGameplayCueNotifyActor_Burst>(
		InGameplayCueNotifyActor))
	{
		NipcatNotifyActor->ScriptInstances.Add(ScriptInstance);
	}
	ScriptInstance->GameplayCueNotifyActor = InGameplayCueNotifyActor;
	ScriptInstance->OnExecute(MyTarget, Parameters);
}
