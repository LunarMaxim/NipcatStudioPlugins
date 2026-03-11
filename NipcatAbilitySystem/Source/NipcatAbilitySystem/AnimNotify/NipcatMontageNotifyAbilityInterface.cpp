// Fill out your copyright notice in the Description page of Project Settings.


#include "NipcatMontageNotifyAbilityInterface.h"


void INipcatMontageNotifyAbilityInterface::NativeNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const UAnimNotifyState_AbilityMontage* NotifyState, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Execute_NotifyBegin(Cast<UObject>(this), MeshComp, Animation, NotifyState, TotalDuration, EventReference);
}

void INipcatMontageNotifyAbilityInterface::NativeNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const UAnimNotifyState_AbilityMontage* NotifyState, const FAnimNotifyEventReference& EventReference)
{
	Execute_NotifyEnd(Cast<UObject>(this), MeshComp, Animation, NotifyState, EventReference);
}
