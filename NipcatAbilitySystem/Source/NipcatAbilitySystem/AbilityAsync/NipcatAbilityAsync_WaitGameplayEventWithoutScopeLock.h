// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "NipcatAbilityAsync_WaitGameplayEventWithoutScopeLock.generated.h"

class UNipcatAbilitySystemComponent;

UCLASS()
class NIPCATABILITYSYSTEM_API UNipcatAbilityAsync_WaitGameplayEventWithoutScopeLock : public UAbilityAsync
{
	GENERATED_BODY()

public:
	/**
	 * Wait until the specified gameplay tag event is triggered on a target ability system component
	 * It will keep listening as long as OnlyTriggerOnce = false
	 * If OnlyMatchExact = false it will trigger for nested tags
	 * If used in an ability graph, this async action will wait even after activation ends. It's recommended to use WaitGameplayEvent instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "NipcatAbilitySystem|Async", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UNipcatAbilityAsync_WaitGameplayEventWithoutScopeLock* WaitGameplayEventToActorWithoutScopeLock(AActor* TargetActor, FGameplayTag EventTag, bool OnlyTriggerOnce = false, bool OnlyMatchExact = true);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEventReceivedDelegate, FGameplayEventData, Payload);

	UPROPERTY(BlueprintAssignable)
	FEventReceivedDelegate EventReceived;

protected:
	virtual void Activate() override;
	virtual void EndAction() override;

	UFUNCTION()
	virtual void GameplayEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData Payload);

	FGameplayTag Tag;
	bool OnlyTriggerOnce = false;
	bool OnlyMatchExact = false;

};
