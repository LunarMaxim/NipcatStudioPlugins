// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_NipcatCheckAbilityActivation.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;
/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UBTDecorator_NipcatCheckAbilityActivation : public UBTDecorator
{
	GENERATED_BODY()
	UBTDecorator_NipcatCheckAbilityActivation();

public:
	UPROPERTY(EditAnywhere, DisplayName="来自黑板", Category="能力激活检查")
	uint8 bFromBlackboard : 1;
	UPROPERTY(EditAnywhere, DisplayName="能力", Category="能力激活检查", meta=(EditCondition="bFromBlackboard"))
	FBlackboardKeySelector AbilityBlackboardKey;
	UPROPERTY(EditAnywhere, DisplayName="能力", Category="能力类", meta=(EditCondition="!bFromBlackboard"))
	TSubclassOf<UGameplayAbility> AbilityClass;
	UPROPERTY(EditAnywhere, DisplayName="仅一次", Category="能力激活检查")
	uint8 bOnlyOnce : 1;

protected:
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	bool CheckByClass(UAbilitySystemComponent* TargetComponent, TSubclassOf<UGameplayAbility> CheckAbilityClass) const;
};
