// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_NipcatWaitAbilityEnded.generated.h"

class UAbilitySystemComponent;
class UGameplayAbility;

UENUM()
enum class ENipcatBTTaskWaitAbilityEnded:uint8
{
	ByTag UMETA(DisplayName = "按激活标签"),
	ByClass UMETA(DisplayName = "按按能力类"),
};

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UBTTask_NipcatWaitAbilityEnded : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_NipcatWaitAbilityEnded();

public:
	UPROPERTY(EditAnywhere, DisplayName="查找能力方式", Category="WaitAbilityEnded")
	ENipcatBTTaskWaitAbilityEnded FindType;
	UPROPERTY(EditAnywhere, DisplayName="任一结束", Category="WaitAbilityEnded")
	uint8 bAnyEnded : 1;
	UPROPERTY(EditAnywhere, DisplayName="标签", Category="WaitAbilityEnded", meta=(EditCondition="FindType==ENipcatBTTaskWaitAbilityEnded::ByTag"))
	FGameplayTag AbilityTag;
	UPROPERTY(EditAnywhere, Category="WaitAbilityEnded", meta=(EditCondition="FindType==ENipcatBTTaskWaitAbilityEnded::ByClass"))
	TSubclassOf<UGameplayAbility> AbilityClass;

	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> MyOwnerComp;

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	bool IsAllWaitAbilityEndedByClassFinished(UAbilitySystemComponent* AbilityComponent);
	bool IsAllWaitAbilityEndedByTagFinished(UAbilitySystemComponent* AbilityComponent);
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
};
