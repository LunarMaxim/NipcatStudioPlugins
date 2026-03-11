// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitInputAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputActionMulticast, UInputAction*, TriggeredInputAction);

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UAbilityTask_WaitInputAction : public UAbilityTask
{
    GENERATED_UCLASS_BODY()

	
	UPROPERTY(BlueprintAssignable)
	FOnInputActionMulticast OnInputAction;

	UFUNCTION()
	void OnInputActionTriggerEvent(UInputAction* TriggeredInputAction);
	
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitInputAction* WaitInputAction(UGameplayAbility* OwningAbility,UInputAction* InputAction, ETriggerEvent TriggerEvent);

protected:

	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY()
	UInputAction* InputAction;

	UPROPERTY()
	ETriggerEvent TriggerEvent;

	int32 ActionBindingHandle;

	
};
