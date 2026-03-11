// Copyright Nipcat Studio All Rights Reserved.


#include "AbilityTask_WaitInputAction.h"

#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"

UAbilityTask_WaitInputAction::UAbilityTask_WaitInputAction(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{}

UAbilityTask_WaitInputAction* UAbilityTask_WaitInputAction::WaitInputAction(UGameplayAbility* OwningAbility,UInputAction* InputAction, ETriggerEvent TriggerEvent)
{
	UAbilityTask_WaitInputAction* Task=NewAbilityTask<UAbilityTask_WaitInputAction>(OwningAbility);
	Task->InputAction = InputAction;
	Task->TriggerEvent = TriggerEvent;
	return Task;
}

void UAbilityTask_WaitInputAction::OnInputActionTriggerEvent(UInputAction* TriggeredInputAction)
{
	if(OnInputAction.IsBound())
		OnInputAction.Broadcast(TriggeredInputAction);
}

void UAbilityTask_WaitInputAction::Activate()
{
	const AActor* Owner = GetOwnerActor();
	if (UEnhancedInputComponent* InputComponent= Cast<UEnhancedInputComponent>(Owner->InputComponent))
	{
		ActionBindingHandle = InputComponent->BindAction(InputAction, TriggerEvent, this, &UAbilityTask_WaitInputAction::OnInputActionTriggerEvent, InputAction).GetHandle();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::Activate);
	}
}

void UAbilityTask_WaitInputAction::OnDestroy(bool AbilityEnded)
{
	AActor* Owner = GetOwnerActor();
	UEnhancedInputComponent* InputComponent= Cast<UEnhancedInputComponent>(Owner->InputComponent);
	if (InputComponent)
	{
		InputComponent->RemoveBindingByHandle(ActionBindingHandle);
	}

	Super::OnDestroy(AbilityEnded);
}