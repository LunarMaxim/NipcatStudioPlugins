// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_NipcatWaitAbilityTag.generated.h"

class UAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UBTTask_NipcatWaitAbilityTag : public UBTTaskNode
{
	GENERATED_BODY()
	UBTTask_NipcatWaitAbilityTag();

public:
	UPROPERTY(EditAnywhere, DisplayName="标签", Category="WaitAbilityTag")
	FGameplayTag AbilityTag;
	UPROPERTY(EditAnywhere, DisplayName="是否是移除标签", Category="WaitAbilityTag")
	uint8 bRemove : 1;

	FDelegateHandle DelegateHandle;
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> ASC;
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> MyOwnerComp;

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION()
	virtual void GameplayTagCallback(const FGameplayTag Tag, int32 NewCount);
	UFUNCTION()
	void FinishedClean();
};
