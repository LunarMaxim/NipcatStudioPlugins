// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_WaitInputActions.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputActionsMulticast, UInputAction*, TriggeredInputAction);

/**
 * 
 */
UCLASS()
class NIPCATABILITYSYSTEM_API UAbilityTask_WaitInputActions : public UAbilityTask
{
    GENERATED_UCLASS_BODY()

	
	UPROPERTY(BlueprintAssignable)
	FOnInputActionsMulticast OnInputAction;

	UFUNCTION()
	void OnInputActionsTriggerEvent(UInputAction* TriggeredInputAction);
	
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_WaitInputActions* WaitInputActions(UGameplayAbility* OwningAbility, TArray<UInputAction*> InputActions);

protected:

	virtual void OnDestroy(bool AbilityEnded) override;

	UPROPERTY()
	TArray<UInputAction*> InputActions;

	TArray<int32> ActionBindingHandles;

	
};
