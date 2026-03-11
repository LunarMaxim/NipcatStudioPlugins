// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatGameplayTask_WaitInputAction.h"

#include "EnhancedInputComponent.h"

UNipcatGameplayTask_WaitInputAction* UNipcatGameplayTask_WaitInputAction::WaitInputAction(
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, UInputAction* InputAction, const uint8 Priority)
{
	UNipcatGameplayTask_WaitInputAction* MyTask = NewTaskUninitialized<UNipcatGameplayTask_WaitInputAction>();
	if (MyTask)
	{
		MyTask->InitTask(*TaskOwner, Priority);
		MyTask->InputAction = InputAction;
	}
	return MyTask;
}

void UNipcatGameplayTask_WaitInputAction::OnInputActionTriggerEvent(UInputAction* TriggeredInputAction)
{
	if(OnInputActionTriggered.IsBound())
		OnInputActionTriggered.Broadcast(TriggeredInputAction);
	EndTask();
}

void UNipcatGameplayTask_WaitInputAction::Activate()
{
	AActor* Owner=GetOwnerActor();
	UEnhancedInputComponent* InputComponent= Cast<UEnhancedInputComponent>(Owner->InputComponent);
	
	ActionBindingHandle = InputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &UNipcatGameplayTask_WaitInputAction::OnInputActionTriggerEvent, InputAction).GetHandle();
}

void UNipcatGameplayTask_WaitInputAction::OnDestroy(bool bInOwnerFinished)
{
	AActor* Owner = GetOwnerActor();
	UEnhancedInputComponent* InputComponent= Cast<UEnhancedInputComponent>(Owner->InputComponent);
	if (InputComponent)
	{
		InputComponent->RemoveBindingByHandle(ActionBindingHandle);
	}

	Super::OnDestroy(bOwnerFinished);
}
