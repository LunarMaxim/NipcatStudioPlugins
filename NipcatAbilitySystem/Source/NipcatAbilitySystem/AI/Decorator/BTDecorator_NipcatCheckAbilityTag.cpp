// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_NipcatCheckAbilityTag.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"

UBTDecorator_NipcatCheckAbilityTag::UBTDecorator_NipcatCheckAbilityTag()
	: bHasAny(true)
{
	bNotifyTick = true;
	bOnlyOnce = true;
}

bool UBTDecorator_NipcatCheckAbilityTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (auto TargetComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if (bHasAny)
		{
			return TargetComponent->HasAnyMatchingGameplayTags(Tags);
		}
		else
		{
			return TargetComponent->HasAllMatchingGameplayTags(Tags);
		}
	}
	return false;
}

void UBTDecorator_NipcatCheckAbilityTag::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	if (!bOnlyOnce)
	{
		bool Rel = CalculateRawConditionValue(OwnerComp, NodeMemory);
		if (Rel == IsInversed())
		{
			OwnerComp.RequestExecution(this);
		}
	}
}
