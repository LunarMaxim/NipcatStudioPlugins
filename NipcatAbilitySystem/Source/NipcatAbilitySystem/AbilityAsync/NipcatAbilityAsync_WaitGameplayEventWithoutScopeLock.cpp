// Copyright Epic Games, Inc. All Rights Reserved.

#include "NipcatAbilityAsync_WaitGameplayEventWithoutScopeLock.h"
#include "AbilitySystemGlobals.h"
#include "NipcatAbilitySystem/NipcatAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NipcatAbilityAsync_WaitGameplayEventWithoutScopeLock)

UNipcatAbilityAsync_WaitGameplayEventWithoutScopeLock* UNipcatAbilityAsync_WaitGameplayEventWithoutScopeLock::WaitGameplayEventToActorWithoutScopeLock(AActor* TargetActor, FGameplayTag EventTag, bool OnlyTriggerOnce, bool OnlyMatchExact)
{
	UNipcatAbilityAsync_WaitGameplayEventWithoutScopeLock* MyObj = NewObject<UNipcatAbilityAsync_WaitGameplayEventWithoutScopeLock>();
	MyObj->SetAbilityActor(TargetActor);
	MyObj->Tag = EventTag;
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->OnlyMatchExact = OnlyMatchExact;
	return MyObj;
}

void UNipcatAbilityAsync_WaitGameplayEventWithoutScopeLock::Activate()
{
	Super::Activate();

	if (UNipcatAbilitySystemComponent* ASC = Cast<UNipcatAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->OnGameplayEventReceived.AddDynamic(this, &ThisClass::GameplayEventContainerCallback);
	}
	else
	{
		EndAction();
	}
}

void UNipcatAbilityAsync_WaitGameplayEventWithoutScopeLock::GameplayEventContainerCallback(FGameplayTag MatchingTag, const FGameplayEventData Payload)
{
	if (ShouldBroadcastDelegates())
	{
		if (OnlyMatchExact)
		{
			if (Tag != MatchingTag)
			{
				return;
			}
		}
		else if (!MatchingTag.GetGameplayTagParents().HasTag(Tag))
		{
			return;
		}
		FGameplayEventData TempPayload = Payload;
		TempPayload.EventTag = MatchingTag;
		EventReceived.Broadcast(MoveTemp(TempPayload));
		
		if (OnlyTriggerOnce)
		{
			EndAction();
		}
	}
	else
	{
		EndAction();
	}
}

void UNipcatAbilityAsync_WaitGameplayEventWithoutScopeLock::EndAction()
{
	if (UNipcatAbilitySystemComponent* ASC = Cast<UNipcatAbilitySystemComponent>(GetAbilitySystemComponent()))
	{
		ASC->OnGameplayEventReceived.RemoveDynamic(this, &ThisClass::GameplayEventContainerCallback);
	}
	Super::EndAction();
}

