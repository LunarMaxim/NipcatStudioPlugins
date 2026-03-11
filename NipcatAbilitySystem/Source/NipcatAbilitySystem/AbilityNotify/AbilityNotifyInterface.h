// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "AbilityNotifyInterface.generated.h"

class UNipcatAbilityTask_PlayMontageAndWait;
class UGameplayAbility;
UINTERFACE(MinimalAPI, BlueprintType)
class UAbilityNotifyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATABILITYSYSTEM_API IAbilityNotifyInterface
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	void NotifyExecute(UGameplayAbility* OwningAbility, UAnimMontage* Montage, UNipcatAbilityTask_PlayMontageAndWait* PlayMontageTask, float TriggerTime, float ActualTriggerTime) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	void PreviewExecute(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TriggerTime, float CurrentAnimationTime) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	FName GetNotifyID(UGameplayAbility* OwningAbility) const;

	UFUNCTION(BlueprintNativeEvent, Category="AbilityNotify")
	FLinearColor GetNotifyColor() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	bool CanExecuteNotify(UGameplayAbility* OwningAbility, UAnimMontage* Montage, float TriggerTime, float CurrentAnimationTime) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	bool GetAlwaysTriggerBeforeStartTimeSeconds() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="AbilityNotify")
	bool GetAlwaysTriggerAfterInterrupt() const;
	
	UFUNCTION(BlueprintImplementableEvent, Category="AbilityNotify", CallInEditor)
	void BlueprintPostEditChangeProperty(FName PropertyName, FName MemberPropertyName);
	
	UFUNCTION(BlueprintImplementableEvent, Category="AbilityNotify", CallInEditor)
	void BlueprintPostDuplicate(UPARAM(ref) FAnimNotifyEvent& EventReference);
	
	UFUNCTION(BlueprintImplementableEvent, Category="AbilityNotify", CallInEditor)
	void BlueprintOnAnimNotifyCreatedInEditor(UPARAM(ref) FAnimNotifyEvent& ContainingAnimNotifyEvent);

};
