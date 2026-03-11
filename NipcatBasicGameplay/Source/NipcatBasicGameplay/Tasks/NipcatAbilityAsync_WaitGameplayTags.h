// Copyright Nipcat Studio All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Abilities/Async/AbilityAsync.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "NipcatAbilityAsync_WaitGameplayTags.generated.h"

class UAbilitySystemComponent;

UCLASS(Abstract)
class NIPCATBASICGAMEPLAY_API UNipcatAbilityAsync_WaitGameplayTags : public UAbilityAsync
{
	GENERATED_BODY()
protected:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncWaitGameplayTagsDelegate, UAbilityAsync*, AsyncAction, FGameplayTag, Tag);

	virtual void Activate() override;
	virtual void EndAction() override;

	virtual void GameplayTagCallback(const FGameplayTag Tag, int32 NewCount);
	virtual void BroadcastDelegate(const FGameplayTag Tag);

	int32 TargetCount = -1;
	FGameplayTagContainer Tags;
	bool OnlyTriggerOnce = false;
	bool CheckOnStarted = true;

	TMap<FGameplayTag, FDelegateHandle> MyHandles;
};

UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatAbilityAsync_WaitGameplayTagsAdded : public UNipcatAbilityAsync_WaitGameplayTags
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FAsyncWaitGameplayTagsDelegate Added;

	/**
	 * Wait until the specified gameplay tag is Added to Target Actor's ability component
	 * If the tag is already present when this task is started, it will immediately broadcast the Added event. It will keep listening as long as OnlyTriggerOnce = false.
	 * If used in an ability graph, this async action will wait even after activation ends. It's recommended to use WaitGameplayTagsAdd instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "NipcatBasicGameplay|Ability|Tasks", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UNipcatAbilityAsync_WaitGameplayTagsAdded* WaitGameplayTagsAddToActor(AActor* TargetActor, FGameplayTagContainer Tags, bool OnlyTriggerOnce=false, bool CheckOnStarted = true);

	virtual void BroadcastDelegate(const FGameplayTag Tag) override;
};

UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatAbilityAsync_WaitGameplayTagsRemoved : public UNipcatAbilityAsync_WaitGameplayTags
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FAsyncWaitGameplayTagsDelegate Removed;

	/**
	 * Wait until the specified gameplay tag is Removed from Target Actor's ability component
	 * If the tag is not present when this task is started, it will immediately broadcast the Removed event. It will keep listening as long as OnlyTriggerOnce = false.
	 * If used in an ability graph, this async action will wait even after activation ends. It's recommended to use WaitGameplayTagsRemove instead.
	 */
	UFUNCTION(BlueprintCallable, Category = "NipcatBasicGameplay|Ability|Tasks", meta = (DefaultToSelf = "TargetActor", BlueprintInternalUseOnly = "TRUE"))
	static UNipcatAbilityAsync_WaitGameplayTagsRemoved* WaitGameplayTagsRemoveFromActor(AActor* TargetActor, FGameplayTagContainer Tags, bool OnlyTriggerOnce=false, bool CheckOnStarted = false);

	virtual void BroadcastDelegate(const FGameplayTag Tag) override;
};