// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_NipcatCompareAbilityAttribute.generated.h"

UENUM()
namespace ENipcatDecoratorCompareAbilityAttribute
{
	enum Type : uint8
	{
		GreaterThanOrEqualTo UMETA(DisplayName="大于等于"),
		Greater UMETA(DisplayName="大于"),
		Equal UMETA(DisplayName="等于(近似)"),
		LessThanOrEqualTo UMETA(DisplayName="小于等于"),
		Less UMETA(DisplayName="小于")
	};
}

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UBTDecorator_NipcatCompareAbilityAttribute : public UBTDecorator
{
	GENERATED_BODY()
	UBTDecorator_NipcatCompareAbilityAttribute();

public:
	UPROPERTY(EditAnywhere, DisplayName="属性名", Category="属性对比")
	FGameplayAttribute Attribute;
	UPROPERTY(EditAnywhere, DisplayName="除数属性名", Category="属性对比")
	FGameplayAttribute DivisorAttribute;
	UPROPERTY(EditAnywhere, DisplayName="操作符", Category="属性对比")
	TEnumAsByte<ENipcatDecoratorCompareAbilityAttribute::Type> Operator;
	UPROPERTY(EditAnywhere, DisplayName="近似", Category="属性对比", meta=(EditCondition="Operator==ENipcatDecoratorCompareAbilityAttribute::Equal"))
	float ErrorTolerance;
	UPROPERTY(EditAnywhere, DisplayName="对比数字", Category="属性对比")
	float CompareValue;
	UPROPERTY(EditAnywhere, DisplayName="仅一次", Category="属性对比")
	uint8 bOnlyOnce : 1;

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
