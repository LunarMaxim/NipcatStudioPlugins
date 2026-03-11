// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_NipcatActivateAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"

UBTTask_NipcatActivateAbility::UBTTask_NipcatActivateAbility()
{
	NodeName = TEXT("激活能力");

	const FName TargetName = GET_MEMBER_NAME_CHECKED(UBTTask_NipcatActivateAbility, TargetKey);
	TargetKey.AddObjectFilter(this, TargetName, AActor::StaticClass());
	const FName OP1Name = GET_MEMBER_NAME_CHECKED(UBTTask_NipcatActivateAbility, OptionalObjectKey);
	OptionalObjectKey.AddObjectFilter(this, OP1Name, UObject::StaticClass());
	const FName OP2Name = GET_MEMBER_NAME_CHECKED(UBTTask_NipcatActivateAbility, OptionalObject2Key);
	OptionalObject2Key.AddObjectFilter(this, OP2Name, UObject::StaticClass());

	bCreateNodeInstance = true;
	ActivateType = ENipcatBTTaskActivateAbility::ByClass;
	bOnlyActivate = false;
}

EBTNodeResult::Type UBTTask_NipcatActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto TargetComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetAIOwner()->GetPawn()))
	{
		switch (ActivateType)
		{
		case ENipcatBTTaskActivateAbility::ByTags:
			return TargetComponent->TryActivateAbilitiesByTag(AbilityTags, true) ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
			break;
		case ENipcatBTTaskActivateAbility::ByClass:
			{
				if (AbilityClass)
				{
					FGameplayAbilitySpec Spec(AbilityClass);
					FGameplayEventData Payload;
					UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
					if (BB)
					{
						Payload.OptionalObject = BB->GetValue<UBlackboardKeyType_Object>(OptionalObjectKey.GetSelectedKeyID());
						Payload.OptionalObject2 = BB->GetValue<UBlackboardKeyType_Object>(OptionalObject2Key.GetSelectedKeyID());
						Payload.Target = Cast<AActor>(BB->GetValue<UBlackboardKeyType_Object>(TargetKey.GetSelectedKeyID()));
					}
					AbilitySpecHandle = TargetComponent->GiveAbilityAndActivateOnce(Spec);
					if (AbilitySpecHandle.IsValid())
					{
						if (FGameplayAbilitySpec* AbilitySpec = TargetComponent->FindAbilitySpecFromHandle(AbilitySpecHandle))
						{
							ActivatedAbility = AbilitySpec->GetPrimaryInstance();
							if (ActivatedAbility)
							{
								if (ActivatedAbility->IsActive() && !bOnlyActivate)
								{
									ActivatedAbility->OnGameplayAbilityEnded.AddUObject(this, &UBTTask_NipcatActivateAbility::OnAbilityEnded);
									AbilitySpec->Ability->OnGameplayAbilityCancelled.AddUObject(this, &UBTTask_NipcatActivateAbility::OnAbilityCancelled);
									MyOwnerComp = &OwnerComp;
									return EBTNodeResult::InProgress;
								}
								else
								{
									return EBTNodeResult::Succeeded;
								}
							}
						}
					}
				}
			}
			break;
		case ENipcatBTTaskActivateAbility::ByEventTag:
			{
				FGameplayEventData Payload;
				UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
				if (BB)
				{
					Payload.OptionalObject = BB->GetValue<UBlackboardKeyType_Object>(OptionalObjectKey.GetSelectedKeyID());
					Payload.OptionalObject2 = BB->GetValue<UBlackboardKeyType_Object>(OptionalObject2Key.GetSelectedKeyID());
					Payload.Target = Cast<AActor>(BB->GetValue<UBlackboardKeyType_Object>(TargetKey.GetSelectedKeyID()));
				}
				UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(OwnerComp.GetAIOwner()->GetPawn(), EventTag, Payload);
				return EBTNodeResult::Succeeded;
			}
			break;
		}
	}
	return EBTNodeResult::Failed;
}

void UBTTask_NipcatActivateAbility::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	UBlackboardData* BBAsset = GetBlackboardAsset();
	if (BBAsset)
	{
		TargetKey.ResolveSelectedKey(*BBAsset);
		OptionalObjectKey.ResolveSelectedKey(*BBAsset);
		OptionalObject2Key.ResolveSelectedKey(*BBAsset);
	}
}

EBTNodeResult::Type UBTTask_NipcatActivateAbility::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (auto TargetComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerComp.GetAIOwner()->GetPawn()))
	{
		if (AbilitySpecHandle.IsValid())
		{
			TargetComponent->CancelAbilityHandle(AbilitySpecHandle);
			if (bWaitAbilityEndOnAbort)
			{
				return EBTNodeResult::InProgress;
			}
			else
			{
				return EBTNodeResult::Aborted;
			}
		}
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}

void UBTTask_NipcatActivateAbility::OnAbilityEnded(UGameplayAbility* Ability)
{
	Ability->OnGameplayAbilityEnded.RemoveAll(this);
	Ability->OnGameplayAbilityCancelled.RemoveAll(this);
	ActivatedAbility = nullptr;
	AbilitySpecHandle = FGameplayAbilitySpecHandle();
	FinishLatentTask(*MyOwnerComp, EBTNodeResult::Succeeded);
}

void UBTTask_NipcatActivateAbility::OnAbilityCancelled()
{
	if (ActivatedAbility)
	{
		ActivatedAbility->OnGameplayAbilityEnded.RemoveAll(this);
		ActivatedAbility->OnGameplayAbilityCancelled.RemoveAll(this);
		ActivatedAbility = nullptr;
		AbilitySpecHandle = FGameplayAbilitySpecHandle();
	}
	FinishLatentTask(*MyOwnerComp, EBTNodeResult::Aborted);
}
