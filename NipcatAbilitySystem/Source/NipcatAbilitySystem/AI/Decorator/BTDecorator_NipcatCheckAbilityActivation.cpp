// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_NipcatCheckAbilityActivation.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Abilities/GameplayAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Class.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UBTDecorator_NipcatCheckAbilityActivation::UBTDecorator_NipcatCheckAbilityActivation()
{
	NodeName = TEXT("检查激活能力");

	const FName AbilityBlackboardKeyName = GET_MEMBER_NAME_CHECKED(UBTDecorator_NipcatCheckAbilityActivation, AbilityBlackboardKey);
	AbilityBlackboardKey.AddObjectFilter(this, AbilityBlackboardKeyName, UGameplayAbility::StaticClass());
	AbilityBlackboardKey.AddClassFilter(this, AbilityBlackboardKeyName, UGameplayAbility::StaticClass());
	bFromBlackboard = false;
	bOnlyOnce = false;
	bNotifyTick = true;
}

void UBTDecorator_NipcatCheckAbilityActivation::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		AbilityBlackboardKey.ResolveSelectedKey(*BBAsset);
	}
}

bool UBTDecorator_NipcatCheckAbilityActivation::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (auto TargetComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if (bFromBlackboard)
		{
			if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent())
			{
				if (AbilityBlackboardKey.SelectedKeyType == UBlackboardKeyType_Class::StaticClass())
				{
					return CheckByClass(TargetComponent, BB->GetValue<UBlackboardKeyType_Class>(AbilityBlackboardKey.GetSelectedKeyID()));
				}
				else
				{
					UGameplayAbility* Ability = Cast<UGameplayAbility>(BB->GetValue<UBlackboardKeyType_Object>(AbilityBlackboardKey.GetSelectedKeyID()));
					return Ability && Ability->IsActive();
				}
			}
		}
		else
		{
			return CheckByClass(TargetComponent, AbilityClass);
		}
	}

	return false;
}

void UBTDecorator_NipcatCheckAbilityActivation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (!bOnlyOnce && CalculateRawConditionValue(OwnerComp, NodeMemory) == IsInversed())
	{
		OwnerComp.RequestExecution(this);
	}
}

bool UBTDecorator_NipcatCheckAbilityActivation::CheckByClass(UAbilitySystemComponent* TargetComponent, TSubclassOf<UGameplayAbility> CheckAbilityClass) const
{
	if (CheckAbilityClass)
	{
		auto Spec = TargetComponent->FindAbilitySpecFromClass(CheckAbilityClass);
		if (Spec && Spec->Ability)
		{
			for (UGameplayAbility* Ability : Spec->NonReplicatedInstances)
			{
				if (Ability->IsActive())
				{
					return true;
				}
			}
		}
	}
	return false;
}
