// Copyright Nipcat Studio All Rights Reserved.

#include "NipcatAbilityAsync_WaitGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NipcatAbilityAsync_WaitGameplayTags)

void UNipcatAbilityAsync_WaitGameplayTags::Activate()
{
	Super::Activate();

	check(TargetCount >= 0);
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC && ShouldBroadcastDelegates())
	{
		for (FGameplayTag Tag : Tags)
		{
			FDelegateHandle MyHandle = ASC->RegisterGameplayTagEvent(Tag).AddUObject(this, &UNipcatAbilityAsync_WaitGameplayTags::GameplayTagCallback);
			MyHandles.Add(Tag, MyHandle);

			if (CheckOnStarted)
			{
				if ((TargetCount == 0 && ASC->GetTagCount(Tag) == 0) || (TargetCount > 0 && ASC->GetTagCount(Tag) >= TargetCount))
				{
					BroadcastDelegate(Tag);
					if (OnlyTriggerOnce)
					{
						EndAction();
					}
				}
			}
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("%s: AbilitySystemComponent is nullptr! Could not register for gameplay tag event with Tags = %s."), *GetName(), *Tags.ToString());
		EndAction();
	}
}

void UNipcatAbilityAsync_WaitGameplayTags::GameplayTagCallback(const FGameplayTag InTag, int32 NewCount)
{
	if (NewCount == TargetCount)
	{
		if (ShouldBroadcastDelegates())
		{
			BroadcastDelegate(InTag);
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
}

void UNipcatAbilityAsync_WaitGameplayTags::BroadcastDelegate(const FGameplayTag Tag)
{
}

void UNipcatAbilityAsync_WaitGameplayTags::EndAction()
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (ASC)
	{
		for (FGameplayTag Tag : Tags)
		{
			const FDelegateHandle* MyHandle = MyHandles.Find(Tag);
			if (MyHandle && MyHandle->IsValid())
			{
				ASC->RegisterGameplayTagEvent(Tag).Remove(*MyHandle);
			}
		}
	}
	Super::EndAction();
}


UNipcatAbilityAsync_WaitGameplayTagsAdded* UNipcatAbilityAsync_WaitGameplayTagsAdded::WaitGameplayTagsAddToActor(AActor* TargetActor, FGameplayTagContainer Tags, bool OnlyTriggerOnce/*=false*/, bool CheckOnStarted)
{
	UNipcatAbilityAsync_WaitGameplayTagsAdded* MyObj = NewObject<UNipcatAbilityAsync_WaitGameplayTagsAdded>();
	MyObj->SetAbilityActor(TargetActor);
	MyObj->Tags = Tags;
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->TargetCount = 1;
	MyObj->CheckOnStarted = CheckOnStarted;

	return MyObj;
}

void UNipcatAbilityAsync_WaitGameplayTagsAdded::BroadcastDelegate(const FGameplayTag Tag)
{
	Added.Broadcast(this, Tag);
}


UNipcatAbilityAsync_WaitGameplayTagsRemoved* UNipcatAbilityAsync_WaitGameplayTagsRemoved::WaitGameplayTagsRemoveFromActor(AActor* TargetActor, FGameplayTagContainer Tags, bool OnlyTriggerOnce/*=false*/, bool CheckOnStarted)
{
	UNipcatAbilityAsync_WaitGameplayTagsRemoved* MyObj = NewObject<UNipcatAbilityAsync_WaitGameplayTagsRemoved>();
	MyObj->SetAbilityActor(TargetActor);
	MyObj->Tags = Tags;
	MyObj->OnlyTriggerOnce = OnlyTriggerOnce;
	MyObj->TargetCount = 0;
	MyObj->CheckOnStarted = CheckOnStarted;

	return MyObj;
}

void UNipcatAbilityAsync_WaitGameplayTagsRemoved::BroadcastDelegate(const FGameplayTag Tag)
{
	Removed.Broadcast(this, Tag);
}

