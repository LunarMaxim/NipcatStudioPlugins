// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_NipcatCompareAbilityAttribute.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"

UBTDecorator_NipcatCompareAbilityAttribute::UBTDecorator_NipcatCompareAbilityAttribute()
{
	bNotifyTick = true;
	ErrorTolerance = SMALL_NUMBER;
	Operator = ENipcatDecoratorCompareAbilityAttribute::Less;
}

bool UBTDecorator_NipcatCompareAbilityAttribute::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	if (auto TargetComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetAIOwner()->GetPawn()))
	{
		bool bFound = false;
		float Value = TargetComponent->GetGameplayAttributeValue(Attribute, bFound);
		bool bDivisorFound = false;
		float DivisorValue = TargetComponent->GetGameplayAttributeValue(DivisorAttribute, bDivisorFound);
		switch (Operator)
		{
		case ENipcatDecoratorCompareAbilityAttribute::GreaterThanOrEqualTo:
			return (DivisorValue > 0 ? Value / DivisorValue : Value) >= CompareValue;
			break;
		case ENipcatDecoratorCompareAbilityAttribute::Greater:
			return (DivisorValue > 0 ? Value / DivisorValue : Value) > CompareValue;
			break;
		case ENipcatDecoratorCompareAbilityAttribute::Equal:
			return FMath::IsNearlyEqual(DivisorValue > 0 ? Value / DivisorValue : Value, ErrorTolerance);
			break;
		case ENipcatDecoratorCompareAbilityAttribute::LessThanOrEqualTo:
			return (DivisorValue > 0 ? Value / DivisorValue : Value) <= CompareValue;
			break;
		case ENipcatDecoratorCompareAbilityAttribute::Less:
			return (DivisorValue > 0 ? Value / DivisorValue : Value) < CompareValue;
			break;
		default:
			return false;
		}
	}
	return false;
}

void UBTDecorator_NipcatCompareAbilityAttribute::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
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
