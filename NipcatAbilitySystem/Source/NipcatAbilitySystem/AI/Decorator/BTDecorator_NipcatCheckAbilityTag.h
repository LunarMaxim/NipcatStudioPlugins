// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_NipcatCheckAbilityTag.generated.h"

UENUM()
enum class ENipcatBTDecoratorCheckAbilityTag:uint8
{
	HasAny UMETA(DisplayName = "拥有任意标签"),
	HasAll UMETA(DisplayName = "拥有所有标签"),
};

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UBTDecorator_NipcatCheckAbilityTag : public UBTDecorator
{
	GENERATED_BODY()
	UBTDecorator_NipcatCheckAbilityTag();

public:
	UPROPERTY(EditAnywhere, DisplayName="包含任意标签", Category="标签检查")
	uint8 bHasAny : 1;
	UPROPERTY(EditAnywhere, DisplayName="仅一次", Category="标签检查")
	uint8 bOnlyOnce : 1;
	UPROPERTY(EditAnywhere, DisplayName="标签", Category="标签检查")
	FGameplayTagContainer Tags;

public:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
};
