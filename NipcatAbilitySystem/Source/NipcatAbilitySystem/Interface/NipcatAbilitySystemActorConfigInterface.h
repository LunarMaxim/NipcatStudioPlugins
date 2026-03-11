// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAbilitySystem/AbilitySet/NipcatAbilitySet.h"
#include "UObject/Interface.h"
#include "NipcatAbilitySystemActorConfigInterface.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UNipcatAbilitySystemActorConfigInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATABILITYSYSTEM_API INipcatAbilitySystemActorConfigInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Nipcat Ability System")
	FName GetAttributeGroupName();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Nipcat Ability System")
	TArray<UNipcatAbilitySet*> GetDefaultAbilitySets();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Nipcat Ability System")
	TMap<TSubclassOf<UGameplayAbility>, float> GetDefaultGameplayAbilitiesToGrant();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Nipcat Ability System")
	TMap<TSubclassOf<UGameplayAbility>, float> GetDefaultGameplayAbilitiesToActivate();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Nipcat Ability System")
	TMap<TSubclassOf<UGameplayEffect>, float> GetDefaultGameplayEffects();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Nipcat Ability System")
	TArray<TSubclassOf<UAttributeSet>> GetDefaultAttributeSets();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="Nipcat Ability System")
	FGameplayTagContainer GetDefaultGameplayTags();
};
