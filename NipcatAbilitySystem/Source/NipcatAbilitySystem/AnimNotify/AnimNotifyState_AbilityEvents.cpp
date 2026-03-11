// Copyright Nipcat Studio All Rights Reserved.


#include "AnimNotifyState_AbilityEvents.h"

#include "AbilitySystemBlueprintLibrary.h"

FString UAnimNotifyState_AbilityEvents::GetNotifyName_Implementation() const
{
	return NotifyName.IsEmpty() ? Super::GetNotifyName_Implementation() : NotifyName;
}

void UAnimNotifyState_AbilityEvents::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	if (MeshComp || MeshComp->GetOwner() && StartEventTag.IsValid())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), StartEventTag, FGameplayEventData());
	}
}

void UAnimNotifyState_AbilityEvents::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);
	if (MeshComp || MeshComp->GetOwner() && TickEventTag.IsValid())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), TickEventTag, FGameplayEventData());
	}
}

void UAnimNotifyState_AbilityEvents::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	if (MeshComp || MeshComp->GetOwner() && EndEventTag.IsValid())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EndEventTag, FGameplayEventData());
	}
}
