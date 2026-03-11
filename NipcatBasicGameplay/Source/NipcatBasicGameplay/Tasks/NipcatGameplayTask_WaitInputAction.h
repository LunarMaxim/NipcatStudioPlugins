// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTask.h"
#include "NipcatGameplayTask_WaitInputAction.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInputActionTriggeredMulticast, UInputAction*, InputAction);

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatGameplayTask_WaitInputAction : public UGameplayTask
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, Category = "NipcatBasicGameplay|GameplayTasks", meta = (AdvancedDisplay = "TaskOwner", DefaultToSelf = "TaskOwner", BlueprintInternalUseOnly = "TRUE"))
	static UNipcatGameplayTask_WaitInputAction* WaitInputAction(TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, UInputAction* InputAction, const uint8 Priority = 192);

	UPROPERTY(BlueprintAssignable)
	FOnInputActionTriggeredMulticast OnInputActionTriggered;
	
	UFUNCTION()
	void OnInputActionTriggerEvent(UInputAction* TriggeredInputAction);

	virtual void Activate() override;
	
protected:
	virtual void OnDestroy(bool bInOwnerFinished) override;

	UPROPERTY()
	UInputAction* InputAction;
	
	int32 ActionBindingHandle;
};
