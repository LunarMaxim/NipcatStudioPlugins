// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_NipcatWaitAbilityTag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"

UBTTask_NipcatWaitAbilityTag::UBTTask_NipcatWaitAbilityTag()
{
	NodeName = TEXT("等待能力标签添加或者删除");
	bRemove = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_NipcatWaitAbilityTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (AbilityTag.IsValid())
	{
		if (auto AbilityComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetAIOwner()->GetPawn()))
		{
			if (bRemove && AbilityComponent->GetGameplayTagCount(AbilityTag) == 0)
			{
				return EBTNodeResult::Succeeded;
			}
			if (!bRemove && AbilityComponent->GetGameplayTagCount(AbilityTag) > 0)
			{
				return EBTNodeResult::Succeeded;
			}
			DelegateHandle = ASC->RegisterGameplayTagEvent(AbilityTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UBTTask_NipcatWaitAbilityTag::GameplayTagCallback);
			ASC = AbilityComponent;
			MyOwnerComp = &OwnerComp;
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;
}

EBTNodeResult::Type UBTTask_NipcatWaitAbilityTag::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	FinishedClean();
	MyOwnerComp = nullptr;
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_NipcatWaitAbilityTag::GameplayTagCallback(const FGameplayTag Tag, int32 NewCount)
{
	if ((bRemove && NewCount == 0) || (!bRemove && NewCount > 0))
	{
		FinishedClean();
		UBehaviorTreeComponent* MyOwnerCompTemp = MyOwnerComp;
		MyOwnerComp = nullptr;
		FinishLatentTask(*MyOwnerCompTemp, EBTNodeResult::Succeeded);
	}
}

void UBTTask_NipcatWaitAbilityTag::FinishedClean()
{
	if (ASC.IsValid() && AbilityTag.IsValid())
	{
		ASC->UnregisterGameplayTagEvent(DelegateHandle, AbilityTag, EGameplayTagEventType::NewOrRemoved);
	}
}
