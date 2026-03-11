// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "NipcatGameplayTask_WaitInputActions.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputActionsTriggeredMulticast, UInputAction*, TriggeredInputAction);

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatGameplayTask_WaitInputActions : public UGameplayTask
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "NipcatBasicGameplay|GameplayTasks", meta = (AdvancedDisplay = "TaskOwner", DefaultToSelf = "TaskOwner", BlueprintInternalUseOnly = "TRUE"))
	static UNipcatGameplayTask_WaitInputActions* WaitInputActions(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, TArray<UInputAction*> InputActions, const uint8 Priority = 192);

	UPROPERTY(BlueprintAssignable)
	FOnInputActionsTriggeredMulticast OnInputActionTriggered;
	
	UFUNCTION()
	void OnInputActionTriggerEvent(UInputAction* InputAction);
	
	virtual void Activate() override;
	
protected:
	virtual void OnDestroy(bool bInOwnerFinished) override;

	UPROPERTY()
	TArray<UInputAction*> InputActions;
	
	TArray<int32> ActionBindingHandles;
};
