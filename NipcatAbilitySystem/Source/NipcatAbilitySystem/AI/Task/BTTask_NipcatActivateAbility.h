// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpecHandle.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_NipcatActivateAbility.generated.h"

class UGameplayAbility;

UENUM()
enum class ENipcatBTTaskActivateAbility:uint8
{
	ByTags UMETA(DisplayName = "按标签激活，然后立即结束"),
	ByEventTag UMETA(DisplayName = "按事件标签激活，然后立即结束"),
	ByClass UMETA(DisplayName = "按能力类激活，等待能力结束"),
};

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UBTTask_NipcatActivateAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_NipcatActivateAbility();

public:
	UPROPERTY(EditAnywhere, DisplayName="激活方式", Category="ActivateAbility")
	ENipcatBTTaskActivateAbility ActivateType;
	UPROPERTY(EditAnywhere, DisplayName="能力标签", Category="ActivateAbility", meta=(EditCondition="ActivateType==ENipcatBTTaskActivateAbility::ByTags"))
	FGameplayTagContainer AbilityTags;
	UPROPERTY(EditAnywhere, DisplayName="事件标签", Category="ActivateAbility", meta=(EditCondition="ActivateType==ENipcatBTTaskActivateAbility::ByEventTag"))
	FGameplayTag EventTag;
	UPROPERTY(EditAnywhere, Category="ActivateAbility", meta=(EditCondition="ActivateType==ENipcatBTTaskActivateAbility::ByClass"))
	TSubclassOf<UGameplayAbility> AbilityClass;
	UPROPERTY(EditAnywhere, DisplayName="Target", Category="ActivateAbility", meta=(EditCondition="ActivateType==ENipcatBTTaskActivateAbility::ByClass||ActivateType==ENipcatBTTaskActivateAbility::ByEventTag"))
	FBlackboardKeySelector TargetKey;
	UPROPERTY(EditAnywhere, DisplayName="可选对象参数1", Category="ActivateAbility", meta=(EditCondition="ActivateType==ENipcatBTTaskActivateAbility::ByClass||ActivateType==ENipcatBTTaskActivateAbility::ByEventTag"))
	FBlackboardKeySelector OptionalObjectKey;
	UPROPERTY(EditAnywhere, DisplayName="可选对象参数2", Category="ActivateAbility", meta=(EditCondition="ActivateType==ENipcatBTTaskActivateAbility::ByClass||ActivateType==ENipcatBTTaskActivateAbility::ByEventTag"))
	FBlackboardKeySelector OptionalObject2Key;
	UPROPERTY(EditAnywhere, DisplayName="仅激活", Category="ActivateAbility")
	uint8 bOnlyActivate : 1;
	UPROPERTY(EditAnywhere, DisplayName="中止节点时等待能力结束再中止", Category="ActivateAbility")
	uint8 bWaitAbilityEndOnAbort : 1;


	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> MyOwnerComp;
	UPROPERTY()
	TObjectPtr<UGameplayAbility> ActivatedAbility;
	UPROPERTY()
	FGameplayAbilitySpecHandle AbilitySpecHandle;


	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;

protected:
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	UFUNCTION()
	void OnAbilityEnded(UGameplayAbility* Ability);
	void OnAbilityCancelled();
};
