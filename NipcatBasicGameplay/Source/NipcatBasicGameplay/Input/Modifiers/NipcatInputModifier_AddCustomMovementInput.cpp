// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInputModifier_AddCustomMovementInput.h"

#include "EnhancedPlayerInput.h"
#include "GameFramework/Character.h"
#include "NipcatBasicGameplay/Character/NipcatCharacterMovementComponent.h"

FInputActionValue UNipcatInputModifier_AddCustomMovementInput::ModifyRaw_Implementation(
	const UEnhancedPlayerInput* PlayerInput, FInputActionValue CurrentValue, float DeltaTime)
{
	if (const auto PC = PlayerInput->GetOuterAPlayerController())
	{
		if (const auto Character = PC->GetCharacter())
		{
			if (const auto NipcatCMC = Cast<UNipcatCharacterMovementComponent>(Character->GetCharacterMovement()))
			{
				NipcatCMC->AddCustomMovementInput(CurrentValue.Get<FVector>(), DeltaTime);
			}
		}
	}
	return CurrentValue;
}
