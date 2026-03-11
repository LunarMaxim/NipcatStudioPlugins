// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NipcatMontageNotifyAbilityInterface.generated.h"

class UAnimNotifyState_AbilityMontage;

UINTERFACE()
class NIPCATABILITYSYSTEM_API UNipcatMontageNotifyAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATABILITYSYSTEM_API INipcatMontageNotifyAbilityInterface
{
	GENERATED_BODY()

public:
	virtual void NativeNotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const UAnimNotifyState_AbilityMontage* NotifyState, float TotalDuration, const FAnimNotifyEventReference& EventReference);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="AbilityNotify")
	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const UAnimNotifyState_AbilityMontage* NotifyState, float TotalDuration, const FAnimNotifyEventReference& EventReference);

	virtual void NativeNotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const UAnimNotifyState_AbilityMontage* NotifyState, const FAnimNotifyEventReference& EventReference);
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="AbilityNotify")
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const UAnimNotifyState_AbilityMontage* NotifyState, const FAnimNotifyEventReference& EventReference);
};
