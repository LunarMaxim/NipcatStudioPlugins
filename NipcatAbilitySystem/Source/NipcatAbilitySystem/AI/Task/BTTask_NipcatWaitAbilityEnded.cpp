// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_NipcatWaitAbilityEnded.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Abilities/GameplayAbility.h"

UBTTask_NipcatWaitAbilityEnded::UBTTask_NipcatWaitAbilityEnded()
	: bAnyEnded(true)
{
	NodeName = TEXT("等待能力结束");
	bCreateNodeInstance = true;
	FindType = ENipcatBTTaskWaitAbilityEnded::ByClass;
}

EBTNodeResult::Type UBTTask_NipcatWaitAbilityEnded::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto TargetComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if (FindType == ENipcatBTTaskWaitAbilityEnded::ByTag)
		{
			if (AbilityTag.IsValid())
			{
				if (IsAllWaitAbilityEndedByTagFinished(TargetComponent))
				{
					return EBTNodeResult::Succeeded;
				}
				MyOwnerComp = &OwnerComp;
				TargetComponent->OnAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded);
			}
		}
		else
		{
			if (AbilityClass)
			{
				if (IsAllWaitAbilityEndedByClassFinished(TargetComponent))
				{
					return EBTNodeResult::Succeeded;
				}
				MyOwnerComp = &OwnerComp;
				TargetComponent->OnAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded);
			}
		}
	}
	return EBTNodeResult::Aborted;
}

bool UBTTask_NipcatWaitAbilityEnded::IsAllWaitAbilityEndedByClassFinished(UAbilitySystemComponent* AbilityComponent)
{
	const TArray<FGameplayAbilitySpec>& Abilities = AbilityComponent->GetActivatableAbilities();
	for (const FGameplayAbilitySpec& Spec : Abilities)
	{
		if (Spec.Ability == nullptr)
		{
			continue;
		}
		if (Spec.Ability->GetClass() == AbilityClass)
		{
			if (Spec.NonReplicatedInstances.Num() > 0)
			{
				return false;
			}
		}
	}
	return true;
}

bool UBTTask_NipcatWaitAbilityEnded::IsAllWaitAbilityEndedByTagFinished(UAbilitySystemComponent* AbilityComponent)
{
	TArray<FGameplayAbilitySpecHandle> Handles;
	AbilityComponent->FindAllAbilitiesWithTags(Handles, FGameplayTagContainer(AbilityTag));
	return Handles.Num() == 0;
}

void UBTTask_NipcatWaitAbilityEnded::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (auto TargetComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MyOwnerComp->GetAIOwner()->GetPawn()))
	{
		if (FindType == ENipcatBTTaskWaitAbilityEnded::ByTag)
		{
			if (AbilityEndedData.AbilityThatEnded && AbilityEndedData.AbilityThatEnded->GetAssetTags().HasTagExact(AbilityTag))
			{
				if (bAnyEnded || IsAllWaitAbilityEndedByTagFinished(TargetComponent))
				{
					FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
				}
			}
		}
		else
		{
			if (AbilityEndedData.AbilityThatEnded && AbilityEndedData.AbilityThatEnded->GetClass() == AbilityClass)
			{
				if (bAnyEnded || IsAllWaitAbilityEndedByClassFinished(TargetComponent))
				{
					FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
				}
			}
		}
	}
	else
	{
		FinishLatentTask(*MyOwnerComp, EBTNodeResult::Aborted);
	}
}
