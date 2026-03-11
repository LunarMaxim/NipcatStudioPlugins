// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotifyState_AbilityMontage.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "NipcatMontageNotifyAbilityInterface.h"
#include "NipcatAbilitySystem/NipcatAbilitySystemComponent.h"

FString UAnimNotifyState_AbilityMontage::GetNotifyName_Implementation() const
{
#if WITH_EDITOR
	if (!NotifyName.IsEmpty())
	{
		return NotifyName;
	}
#endif
	return Super::GetNotifyName_Implementation();
}

void UAnimNotifyState_AbilityMontage::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (auto Ability = GetNotifyAbility(MeshComp, Animation))
	{
		Ability->NativeNotifyBegin(MeshComp, Animation, this, TotalDuration, EventReference);
	}
}

void UAnimNotifyState_AbilityMontage::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (auto Ability = GetNotifyAbility(MeshComp, Animation))
	{
		Ability->NativeNotifyEnd(MeshComp, Animation, this, EventReference);
	}
}

INipcatMontageNotifyAbilityInterface* UAnimNotifyState_AbilityMontage::GetNotifyAbility(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (auto AbilityComponent = Cast<UNipcatAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(MeshComp->GetOwner())))
	{
		if (UGameplayAbility* Ability = AbilityComponent->FindMontageAbility(Cast<UAnimMontage>(Animation)))
		{
			return Cast<INipcatMontageNotifyAbilityInterface>(Ability);
		}
	}
	return nullptr;
}
