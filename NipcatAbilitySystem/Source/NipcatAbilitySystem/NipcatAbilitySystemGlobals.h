// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "AbilitySystemGlobals.h"
#include "NipcatAbilitySystemGlobals.generated.h"

class UAbilitySystemComponent;

UCLASS(Config=Game)
class UNipcatAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_UCLASS_BODY()

	UFUNCTION(BlueprintPure)
	TArray<FSoftObjectPath> GetGlobalAttributeSetDefaultsTableNames() { return GetGlobalAttributeSetDefaultsTablePaths(); }
	
	UFUNCTION(BlueprintCallable)
	void ReloadAttributeDefaultValues();

#if WITH_EDITOR
	void ReloadAttributeDefaultsOnPreBeginPIE(const bool bIsSimulatingInEditor);
#endif
	
	//~UAbilitySystemGlobals interface
	virtual void InitGlobalData() override;
	virtual void AllocAttributeSetInitter() override;
	virtual FGameplayAbilityActorInfo* AllocAbilityActorInfo() const override;
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	virtual void InitGameplayCueParameters(FGameplayCueParameters& CueParameters, const FGameplayEffectContextHandle& EffectContext) override;
	virtual void InitGameplayCueParameters_GESpec(FGameplayCueParameters& CueParameters, const FGameplayEffectSpec& Spec) override;
	virtual void InitGameplayCueParameters(FGameplayCueParameters& CueParameters, const FGameplayEffectSpecForRPC& Spec) override;
	//~End of UAbilitySystemGlobals interface
};
