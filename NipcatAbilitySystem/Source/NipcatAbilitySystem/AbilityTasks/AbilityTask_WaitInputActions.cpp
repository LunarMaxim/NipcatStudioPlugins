// Copyright Nipcat Studio All Rights Reserved.


#include "AbilityTask_WaitInputActions.h"

#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"

UAbilityTask_WaitInputActions::UAbilityTask_WaitInputActions(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

UAbilityTask_WaitInputActions* UAbilityTask_WaitInputActions::WaitInputActions(UGameplayAbility* OwningAbility, TArray<UInputAction*> InputActions)
{
	UAbilityTask_WaitInputActions* Task=NewAbilityTask<UAbilityTask_WaitInputActions>(OwningAbility);
	Task->InputActions = InputActions;
	return Task;
}

void UAbilityTask_WaitInputActions::OnInputActionsTriggerEvent(UInputAction* TriggeredInputAction)
{
	if(OnInputAction.IsBound())
		OnInputAction.Broadcast(TriggeredInputAction);
}

void UAbilityTask_WaitInputActions::Activate()
{
	AActor* Owner=GetOwnerActor();
	UEnhancedInputComponent* InputComponent= Cast<UEnhancedInputComponent>(Owner->InputComponent);

	for (UInputAction* InputAction : InputActions)
	{
		ActionBindingHandles.Add(InputComponent->BindAction(InputAction, ETriggerEvent::Triggered, this, &UAbilityTask_WaitInputActions::OnInputActionsTriggerEvent, InputAction).GetHandle());
	}
}

void UAbilityTask_WaitInputActions::OnDestroy(bool AbilityEnded)
{
	AActor* Owner = GetOwnerActor();
	UEnhancedInputComponent* InputComponent= Cast<UEnhancedInputComponent>(Owner->InputComponent);
	if (InputComponent)
	{
		for (int ActionBindingHandle : ActionBindingHandles)
		{
			InputComponent->RemoveBindingByHandle(ActionBindingHandle);
		}
	}

	Super::OnDestroy(AbilityEnded);
}