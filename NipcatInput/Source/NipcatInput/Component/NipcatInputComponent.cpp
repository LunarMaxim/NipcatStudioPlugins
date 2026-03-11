// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInputComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "NipcatInput/Types/NipcatInputTypes.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "GameFramework/InputDeviceLibrary.h"
#include "GameFramework/InputDeviceSubsystem.h"
#include "NipcatInput/Modifier/NipcatInputModifier_TriggerGameplayEvent.h"
#include "NipcatInput/Settings/NipcatInputDeveloperSettings.h"

UNipcatInputComponent::UNipcatInputComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UNipcatInputComponent::BeginPlay()
{
	Super::BeginPlay();
	
	if (APlayerController* Controller = Cast<APlayerController>(GetOwner()))
	{
		PlayerController = Controller;
	}

	if (!PlayerController)
	{
		return;
	}

	if (!PlayerController->GetLocalPlayer())
	{
		return;
	}

	if (PlayerController->GetLocalPlayer()->GetSubsystemArrayCopy<UEnhancedInputLocalPlayerSubsystem>().IsEmpty())
	{
		return;
	}
	
	AddInputMappingContext(UNipcatInputDeveloperSettings::Get()->InputMappingContexts);

	if (UInputDeviceSubsystem* DeviceSubsystem = UInputDeviceSubsystem::Get())
	{
		DeviceSubsystem->OnInputHardwareDeviceChanged.AddDynamic(this, &ThisClass::OnInputHardwareDeviceChanged);
		switch (GetHardwareDeviceType())
		{
		case EHardwareDevicePrimaryType::KeyboardAndMouse:
			AddInputMappingContext(UNipcatInputDeveloperSettings::Get()->InputMappingContexts_Keyboard);
			break;
		case EHardwareDevicePrimaryType::Gamepad:
			AddInputMappingContext(UNipcatInputDeveloperSettings::Get()->InputMappingContexts_Gamepad);
			break;
		default:
			break;
		}
	}
}

UAbilitySystemComponent* UNipcatInputComponent::GetAbilitySystem() const
{
	if (PlayerController)
	{
		return UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(PlayerController->GetPawn());
	}
	return nullptr;
}

void UNipcatInputComponent::OnInputStarted(const UInputAction* InputAction)
{
	OngoingInputs.Add(InputAction);
	if (*InputBlockStack.Find(InputAction) > 0)
	{
		if (InputActionEventMapping.Find(InputAction)->BufferStartedEvent)
		{
			for (auto& Pair : InputBufferStacks)
			{
				if (Pair.Value.InputActionsForBuffer.Contains(InputAction))
				{
					Pair.Value.BufferedInputActions.Insert(const_cast<UInputAction*>(InputAction), 0);
				}
			}
		}
	}
	else
	{
		SendInputTriggerEvent(InputAction, ETriggerEvent::Started);
	}
}

void UNipcatInputComponent::OnInputTriggered(const UInputAction* InputAction)
{
	if (const auto BufferToCancel = InputActionEventMapping.Find(InputAction)->BufferToCancelOnTriggered; !BufferToCancel.IsEmpty())
	{
		for (auto& [Name, Stack] : InputBufferStacks)
		{
			for (auto& InputActionToRemove : BufferToCancel)
			{
				Stack.BufferedInputActions.Remove(InputActionToRemove);
			}
		}
	}
	if (*InputBlockStack.Find(InputAction) > 0)
	{
		if (InputActionEventMapping.Find(InputAction)->BufferTriggeredEvent)
		{
			for (auto& Pair : InputBufferStacks)
			{
				if (Pair.Value.InputActionsForBuffer.Contains(InputAction))
				{
					Pair.Value.BufferedInputActions.Insert(const_cast<UInputAction*>(InputAction), 0);
				}
			}
		}
	}
	else
	{
		SendInputTriggerEvent(InputAction, ETriggerEvent::Triggered);
	}
}

void UNipcatInputComponent::OnInputOngoing(const UInputAction* InputAction)
{
	if (*InputBlockStack.Find(InputAction) <= 0)
	{
		SendInputTriggerEvent(InputAction, ETriggerEvent::Ongoing);
	}
}

void UNipcatInputComponent::OnInputCompleted(const UInputAction* InputAction)
{
	OngoingInputs.Remove(InputAction);
	if (*InputBlockStack.Find(InputAction) <= 0)
	{
		SendInputTriggerEvent(InputAction, ETriggerEvent::Completed);
	}
}

void UNipcatInputComponent::OnInputCanceled(const UInputAction* InputAction)
{
	OngoingInputs.Remove(InputAction);
	if (*InputBlockStack.Find(InputAction) <= 0)
	{
		SendInputTriggerEvent(InputAction, ETriggerEvent::Completed);
	}
}

void UNipcatInputComponent::SendInputTriggerEvent(const UInputAction* InputAction, ETriggerEvent TriggerEvent)
{
	if (const FNipcatInputActionEvent* InputActionEvent = InputActionEventMapping.Find(InputAction))
	{
		FGameplayTag EventTag;
	
		switch (TriggerEvent)
		{
		case ETriggerEvent::Started:
			EventTag = InputActionEvent->StartedEventTag;
			break;
		case ETriggerEvent::Triggered:
			EventTag = InputActionEvent->TriggeredEventTag;
			break;
		case ETriggerEvent::Ongoing:
			EventTag = InputActionEvent->OngoingEventTag;
			break;
		case ETriggerEvent::Completed:
			EventTag = InputActionEvent->CompletedEventTag;
			break;
		default:
			EventTag = InputActionEvent->TriggeredEventTag;
		}
		if (EventTag.IsValid())
		{
			FGameplayEventData Payload;
			Payload.OptionalObject = const_cast<UInputAction*>(InputAction);
			if (const auto ASC = GetAbilitySystem())
			{
				if (!InputActionEvent->TriggerRequirements.IsEmpty() && !InputActionEvent->TriggerRequirements.RequirementsMet(ASC->GetOwnedGameplayTags()))
				{
					return;
				}
				ASC->HandleGameplayEvent(EventTag, &Payload);
			}
		}
	}
}

void UNipcatInputComponent::BlockInput(UInputAction* InputAction, int32 Count, UObject* SourceObject)
{
	if (InputAction)
	{
		if (const auto InputActionEvent = InputActionEventMapping.Find(InputAction))
		{
			if (InputActionEvent->CanBeBlocked)
			{
				InputBlockStack.FindOrAdd(InputAction, 0) += Count;
				if (SourceObject)
				{
					BlockInputObjects.FindOrAdd(InputAction).Map.FindOrAdd(SourceObject) += Count;
				}
			}
		}
	}
}

void UNipcatInputComponent::BlockInputs(const TArray<UInputAction*>& InputActions, int32 Count, UObject* SourceObject)
{
	for (auto& InputAction : InputActions)
	{
		BlockInput(InputAction, Count, SourceObject);
	}
}

void UNipcatInputComponent::BlockAllInputs(const TArray<UInputAction*>& WithoutInputActions, int32 BlockLevel, int32 Count, UObject* SourceObject)
{
	BlockInputs(GetAllInputActionsUnderBlockLevel(WithoutInputActions, BlockLevel), Count, SourceObject);
}

void UNipcatInputComponent::UnlockInput(UInputAction* InputAction, int32 Count, UObject* SourceObject)
{
	if (InputAction)
	{
		if (const auto InputActionEvent = InputActionEventMapping.Find(InputAction))
		{
			if (InputActionEvent->CanBeBlocked)
			{
				int32& Value = InputBlockStack.FindOrAdd(InputAction, 0);
				int32 MaxUnlockCount = INT_MAX;
				if (SourceObject)
				{
					auto& BlockInputCountMap = BlockInputObjects.FindOrAdd(InputAction).Map;
					auto& FoundCount = BlockInputCountMap.FindOrAdd(SourceObject);
					MaxUnlockCount = FoundCount;
					if (Count < 0 || Count >= MaxUnlockCount)
					{
						BlockInputCountMap.Remove(SourceObject);
					}
					else
					{
						FoundCount -=  Count;
						if (FoundCount <= 0)
						{
							BlockInputCountMap.Remove(SourceObject);
						}
					}
				}
		
				if (Count < 0 || Count >= MaxUnlockCount)
				{
					Value -= MaxUnlockCount;
				}
				else
				{
					Value -= Count;
				}
			}
		}
	}
}

void UNipcatInputComponent::UnlockInputs(const TArray<UInputAction*>& InputActions, int32 Count, UObject* SourceObject)
{
	for (const auto& InputAction : InputActions)
	{
		UnlockInput(InputAction, Count, SourceObject);
	}
}

void UNipcatInputComponent::UnlockAllInputs(const TArray<UInputAction*>& WithoutInputActions, int32 BlockLevel, int32 Count, UObject* SourceObject)
{
	UnlockInputs(GetAllInputActionsUnderBlockLevel(WithoutInputActions, BlockLevel), Count, SourceObject);
}

void UNipcatInputComponent::StartBuffer(const TArray<UInputAction*>& InputActions, const TArray<UInputAction*>& InitialBufferedInputActions, FName ID)
{
	const auto& InputActionsForBuffer = FilterInputActionsForBuffer(InputActions);
	FNipcatInputBufferStack& InputBufferStack = InputBufferStacks.FindOrAdd(ID);
	InputBufferStack.InputActionsForBuffer = InputActionsForBuffer;
	InputBufferStack.BufferedInputActions.Append(InitialBufferedInputActions);
}

void UNipcatInputComponent::StartBufferForAllInputs(FName ID, const TArray<UInputAction*>& WithOutInputActions, const TArray<UInputAction*>& InitialBufferedInputActions, int32 BlockLevel)
{
	const auto& InputActionsForBuffer = FilterInputActionsForBuffer(GetAllInputActionsUnderBlockLevel(WithOutInputActions, BlockLevel));
	StartBuffer(InputActionsForBuffer, InitialBufferedInputActions, ID);
}

TArray<UInputAction*> UNipcatInputComponent::GetAllInputActionsUnderBlockLevel(const TArray<UInputAction*>& WithoutInputActions, int32 BlockLevel) const
{
	TArray<UInputAction*> AllInputActions;
	if (BlockLevel < 0)
	{
		InputActionEventMapping.GetKeys(AllInputActions);
	}
	else
	{
		for (const auto& [InputAction, EventMapping] : InputActionEventMapping)
		{
			if (EventMapping.BlockLevel <= BlockLevel)
			{
				AllInputActions.Add(InputAction);
			}
		}
	}
	for (auto& WithoutInputAction : WithoutInputActions)
	{
		AllInputActions.Remove(WithoutInputAction);
	}
	return AllInputActions;
}

TArray<UInputAction*> UNipcatInputComponent::FilterInputActionsForBuffer(
	const TArray<UInputAction*>& InputActions) const
{
	TArray<UInputAction*> Result;
	for (auto& InputAction : InputActions)
	{
		if (InputAction)
		{
			if (const auto& Config = InputActionEventMapping.Find(InputAction))
			{
				if (Config->CanBeBuffered && Config->CanBeBlocked)
				{
					Result.Add(InputAction);
				}
			}
		}
	}
	return Result;
}

void UNipcatInputComponent::EndBuffer(FName ID, bool TryTrigger, bool ShouldUnlockInputs, UObject* SourceObject)
{
	FNipcatInputBufferStack InputBufferStack = InputBufferStacks.FindOrAdd(ID);
	InputBufferStacks.Remove(ID);

	if (ShouldUnlockInputs)
	{
		UnlockInputs(InputBufferStack.InputActionsForBuffer, 1, SourceObject);
	}
	
	if (TryTrigger)
	{
		if (!InputBufferStack.BufferedInputActions.IsEmpty())
		{
			InputBufferStack.BufferedInputActions.StableSort([&](const UInputAction& A, const UInputAction& B)
			{
				const int32 PriorityA = InputActionEventMapping.Find(&A)->BufferPriority;
				const int32 PriorityB = InputActionEventMapping.Find(&B)->BufferPriority;
				return PriorityA > PriorityB;
			});
			UInputAction*& ResultInputAction = InputBufferStack.BufferedInputActions[0];
			if (const auto InputActionEvent = InputActionEventMapping.Find(ResultInputAction))
			{
				if (InputActionEvent->CanTriggerBufferedInput && InputBlockStack.FindOrAdd(ResultInputAction) <= 0)
				{
					if (InputActionEvent->BufferStartedEvent)
					{
						SendInputTriggerEvent(ResultInputAction, ETriggerEvent::Started);
					}
					
					if (InputActionEvent->BufferTriggeredEvent)
					{
						SendInputTriggerEvent(ResultInputAction, ETriggerEvent::Triggered);
					}
				}
			}
		}
	}
}

bool UNipcatInputComponent::CheckInputCompleted(UInputAction* InputAction, bool TryTrigger, bool ShouldUnlockInputs, UObject* SourceObject)
{
	if (InputAction)
	{
		if (ShouldUnlockInputs)
		{
			UnlockInput(InputAction, 1, SourceObject);
		}
		
		if (!OngoingInputs.Contains(InputAction))
		{
			if (TryTrigger)
			{
				SendInputTriggerEvent(InputAction, ETriggerEvent::Completed);
				return true;
			}
		}
		return false;
	}
	return true;
}

void UNipcatInputComponent::AddInputMappingContext(TMap<TSoftObjectPtr<UInputMappingContext>, FNipcatInputMappingContextSetting> InputMappingContexts)
{
	EnhancedInputSubsystem = PlayerController->GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	
	for (const auto& [IMCPtr, Setting] : InputMappingContexts)
	{
		if (const UInputMappingContext* IMC = IMCPtr.LoadSynchronous())
		{
			EnhancedInputSubsystem->AddMappingContext(IMC, Setting.Priority, Setting.Options);
			for (const auto& KeyMapping : IMC->GetMappings())
			{
				if (const UInputAction* InputAction = KeyMapping.Action.Get())
				{
					if (!InputActionEventMapping.Contains(InputAction))
					{
						for (const auto& Modifier : InputAction->Modifiers)
						{
							if (const auto& NipcatModifier = Cast<UNipcatInputModifier_TriggerGameplayEvent>(Modifier))
							{
								InputActionEventMapping.Add(const_cast<UInputAction*>(InputAction), NipcatModifier->TriggerEvents);
								InputBlockStack.FindOrAdd(const_cast<UInputAction*>(InputAction), 0);
								const FEnhancedInputActionEventBinding* StartHandle = &BindAction(InputAction, ETriggerEvent::Started, this, &ThisClass::OnInputStarted, InputAction);
								const FEnhancedInputActionEventBinding* TriggerHandle = &BindAction(InputAction, ETriggerEvent::Triggered, this, &ThisClass::OnInputTriggered, InputAction);
								if (NipcatModifier->TriggerEvents.OngoingEventTag.IsValid())
								{
									const FEnhancedInputActionEventBinding* OngoingHandle = &BindAction(InputAction, ETriggerEvent::Completed, this, &ThisClass::OnInputOngoing, InputAction);
								}
								const FEnhancedInputActionEventBinding* CompleteHandle = &BindAction(InputAction, ETriggerEvent::Completed, this, &ThisClass::OnInputCompleted, InputAction);
								const FEnhancedInputActionEventBinding* CanceledHandle = &BindAction(InputAction, ETriggerEvent::Canceled, this, &ThisClass::OnInputCanceled, InputAction);
								break;
							}
						}
					}
				}
			}
		}
	}
}

void UNipcatInputComponent::RemoveInputMappingContext(TMap<TSoftObjectPtr<UInputMappingContext>, FNipcatInputMappingContextSetting> InputMappingContexts)
{
	for (const auto& [IMCPtr, Setting] : InputMappingContexts)
	{
		if (const UInputMappingContext* IMC = IMCPtr.LoadSynchronous())
		{
			EnhancedInputSubsystem->RemoveMappingContext(IMC, Setting.Options);
		}
	}
}

void UNipcatInputComponent::OnInputHardwareDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId)
{
	switch (GetHardwareDeviceType(DeviceId))
	{
	case EHardwareDevicePrimaryType::KeyboardAndMouse:
		RemoveInputMappingContext(UNipcatInputDeveloperSettings::Get()->InputMappingContexts_Gamepad);
		AddInputMappingContext(UNipcatInputDeveloperSettings::Get()->InputMappingContexts_Keyboard);
		break;
	case EHardwareDevicePrimaryType::Gamepad:
		RemoveInputMappingContext(UNipcatInputDeveloperSettings::Get()->InputMappingContexts_Keyboard);
		AddInputMappingContext(UNipcatInputDeveloperSettings::Get()->InputMappingContexts_Gamepad);
		break;
	default:
		break;
	}
}

EHardwareDevicePrimaryType UNipcatInputComponent::GetHardwareDeviceType(const FInputDeviceId DeviceId)
{
	if (const UInputDeviceSubsystem* DeviceSubsystem = UInputDeviceSubsystem::Get())
	{
		if (DeviceId.IsValid())
		{
			return DeviceSubsystem->GetInputDeviceHardwareIdentifier(DeviceId).PrimaryDeviceType;
		}
		return DeviceSubsystem->GetInputDeviceHardwareIdentifier(UInputDeviceLibrary::GetDefaultInputDevice()).PrimaryDeviceType;
	}
	return EHardwareDevicePrimaryType::Unspecified;
}
