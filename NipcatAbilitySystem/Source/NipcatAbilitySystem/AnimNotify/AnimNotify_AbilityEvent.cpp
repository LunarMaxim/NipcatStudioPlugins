// Copyright Nipcat Studio All Rights Reserved.


#include "AnimNotify_AbilityEvent.h"

#include "AbilitySystemBlueprintLibrary.h"

FString UAnimNotify_AbilityEvent::GetNotifyName_Implementation() const
{
	return NotifyName.IsEmpty() ? Super::GetNotifyName_Implementation() : NotifyName;
}

void UAnimNotify_AbilityEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	if (MeshComp || MeshComp->GetOwner())
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(MeshComp->GetOwner(), EventTag, FGameplayEventData());
	}
}
