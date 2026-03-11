// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "..\FX\NipcatFXDefinition_BurstGameplayCue.h"
#include "NipcatGameplayCueNotifyActor_Burst.generated.h"

UCLASS()
class NIPCATABILITYSYSTEM_API ANipcatGameplayCueNotifyActor_Burst : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()

public:

	ANipcatGameplayCueNotifyActor_Burst();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UNipcatFXScript_BurstGameplayCue*> ScriptInstances;
	
protected:
	
	virtual bool OnExecute_Implementation(AActor* Target, const FGameplayCueParameters& Parameters) override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "- Config -")
	FNipcatFXDefinition_BurstGameplayCue BurstEffects;
	
	virtual bool Recycle() override;
};
