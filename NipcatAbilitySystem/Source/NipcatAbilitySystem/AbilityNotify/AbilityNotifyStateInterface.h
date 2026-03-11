// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityNotifyStateInterface.generated.h"

class UNipcatAbilityTask_PlayMontageAndWait;
class UGameplayAbility;
UINTERFACE(MinimalAPI, BlueprintType)
class UAbilityNotifyStateInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATABILITYSYSTEM_API IAbilityNotifyStateInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	void NotifyBegin(UGameplayAbility* OwningAbility, UAnimMontage* Montage, UNipcatAbilityTask_PlayMontageAndWait* PlayMontageTask, float TriggerTime, float CurrentAnimationTime, float TotalDuration) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	void NotifyTick(UGameplayAbility* OwningAbility, UAnimMontage* Montage, UNipcatAbilityTask_PlayMontageAndWait* PlayMontageTask, float FrameDeltaTime) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	void NotifyEnd(UGameplayAbility* OwningAbility, UAnimMontage* Montage, UNipcatAbilityTask_PlayMontageAndWait* PlayMontageTask, bool bInterrupted) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	void PreviewBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TriggerTime, float CurrentAnimationTime, float TotalDuration) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	void PreviewTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	void PreviewEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	FName GetNotifyStateID(UGameplayAbility* OwningAbility) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	bool CanExecuteNotifyState(UGameplayAbility* OwningAbility, UAnimMontage* Montage, float TriggerTime, float CurrentAnimationTime, float TotalDuration) const;
	
	UFUNCTION(BlueprintNativeEvent, Category="AbilityNotify")
	FLinearColor GetNotifyColor() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category="AbilityNotify", CallInEditor)
	void BlueprintPostEditChangeProperty(FName PropertyName, FName MemberPropertyName);
	
	UFUNCTION(BlueprintImplementableEvent, Category="AbilityNotify", CallInEditor)
	void BlueprintPostDuplicate(UPARAM(ref) FAnimNotifyEvent& EventReference);
	
	UFUNCTION(BlueprintImplementableEvent, Category="AbilityNotify", CallInEditor)
	void BlueprintOnAnimNotifyCreatedInEditor(UPARAM(ref) FAnimNotifyEvent& ContainingAnimNotifyEvent);
};
