// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAbilitySystem/EffectContext/NipcatGameplayEffectContext.h"

#include "NipcatAbilitySystemWorldSubsystem.generated.h"

USTRUCT(BlueprintType)
struct FDamageInstanceArray
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	TArray<UDamageInstance*> Array;
};

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UNipcatAbilitySystemWorldSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	UDamageInstance* AllocDamageInstance(UObject* Outer, const FDamageDefinition& DamageDefinition, const FGameplayEffectSpecHandle InEffectSpec);
	
protected:
	UPROPERTY(VisibleAnywhere)
	TMap<UObject*, FDamageInstanceArray> DamageInstances;
};
