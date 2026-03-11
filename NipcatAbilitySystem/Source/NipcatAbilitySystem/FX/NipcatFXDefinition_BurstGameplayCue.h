// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "UObject/Object.h"
#include "NipcatFXDefinition_BurstGameplayCue.generated.h"

struct FGameplayCueParameters;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, Abstract, meta=(ShowWorldContextPin), CollapseCategories)
class NIPCATABILITYSYSTEM_API UNipcatFXScript_BurstGameplayCue : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	AGameplayCueNotify_Actor* GameplayCueNotifyActor;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(WorldContext = "MyTarget"))
	void OnExecute(AActor* MyTarget, const FGameplayCueParameters& Parameters);

	virtual UWorld* GetWorld() const override;
};

USTRUCT(Blueprintable, BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatFXDefinition_BurstGameplayCue : public FTableRowBase
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Instanced, meta=(ForceInlineRow, Categories="FX"))
	TMap<FGameplayTag, UNipcatFXScript_BurstGameplayCue*> Effects;

	void ExecuteEffects(AActor* MyTarget,
	                    const FGameplayCueParameters& Parameters,
	                    AGameplayCueNotify_Actor* InGameplayCueNotifyActor,
	                    TMap<FGameplayTag, UNipcatFXScript_BurstGameplayCue*>* OverrideScripts) const;

	static void ExecuteEffect(const UNipcatFXScript_BurstGameplayCue* Script,
	                   const FGameplayCueParameters& Parameters,
	                   AActor* MyTarget, AGameplayCueNotify_Actor* InGameplayCueNotifyActor);
};
