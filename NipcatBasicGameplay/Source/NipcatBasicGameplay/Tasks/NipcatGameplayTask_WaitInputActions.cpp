// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatGameplayTask_WaitInputActions.h"

#include "EnhancedInputComponent.h"

UNipcatGameplayTask_WaitInputActions* UNipcatGameplayTask_WaitInputActions::WaitInputActions(
	TScriptInterface<IGameplayTaskOwnerInterface> TaskOwner, TArray<UInputAction*> InputActions, const uint8 Priority)
{
	UNipcatGameplayTask_WaitInputActions* MyTask = NewTaskUninitialized<UNipcatGameplayTask_WaitInputActions>();
	if (MyTask)
	{
		MyTask->InitTask(*TaskOwner, Priority);
		MyTask->InputActions = InputActions;
	}
	return MyTask;
}

void UNipcatGameplayTask_WaitInputActions::OnInputActionTriggerEvent(UInputAction* InputAction)
{
	if(OnInputActionTriggered.IsBound())
		OnInputActionTriggered.Broadcast(InputAction);
}

void UNipcatGameplayTask_WaitInputActions::Activate()
{
	AActor* Owner=GetOwnerActor();
	UEnhancedInputComponent* InputComponent= Cast<UEnhancedInputComponent>(Owner->InputComponent);

	for (auto InputAction : InputActions)
	{
		ActionBindingHandles.Add(InputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &UNipcatGameplayTask_WaitInputActions::OnInputActionTriggerEvent, InputAction).GetHandle());
	}
}

void UNipcatGameplayTask_WaitInputActions::OnDestroy(bool bInOwnerFinished)
{
	AActor* Owner = GetOwnerActor();
	UEnhancedInputComponent* InputComponent= Cast<UEnhancedInputComponent>(Owner->InputComponent);
	if (InputComponent)
	{
		for (auto ActionBinding : ActionBindingHandles)
		{
			InputComponent->RemoveBindingByHandle(ActionBinding);
		}
	}

	Super::OnDestroy(bOwnerFinished);
}
