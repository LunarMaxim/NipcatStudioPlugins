// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatPlayerController.h"

#include "NipcatInput/Component/NipcatInputComponent.h"

void ANipcatPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	BlueprintVisibleInputComponent = InputComponent;
}

FGenericTeamId ANipcatPlayerController::GetGenericTeamId() const
{
	return MyTeamID;
}

ETeamAttitude::Type ANipcatPlayerController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<IGenericTeamAgentInterface>(&Other);
	
	if (OtherTeamAgent)
	{
		FGenericTeamId OtherTeamID = OtherTeamAgent->GetGenericTeamId();

		//Checking Other pawn ID to define Attitude
		if (OtherTeamID == GetGenericTeamId() || FriendlyTeamID.Contains(OtherTeamID))
		{
			//友军
			return ETeamAttitude::Friendly;
		}
		if (HostileTeamID.Contains(OtherTeamID))
		{
			//敌人
			return ETeamAttitude::Hostile;
		}
	}

	return ETeamAttitude::Neutral;
}

bool ANipcatPlayerController::ShouldFlushKeysWhenViewportFocusChanges() const
{
	return false;
}

UNipcatInputComponent* ANipcatPlayerController::GetNipcatInputComponent() const
{
	return Cast<UNipcatInputComponent>(InputComponent);
}

void ANipcatPlayerController::OnSetPawn_Implementation(APawn* PrevPawn, APawn* NewPawn)
{
}

void ANipcatPlayerController::SetPawn(APawn* InPawn)
{
	APawn* PrevPawn = GetPawn();
	Super::SetPawn(InPawn);
	if (InPawn)
	{
		InPawn->Controller = this;
	}
	OnSetPawn(PrevPawn, InPawn);
	if (OnSetPawnDelegate.IsBound())
	{
		OnSetPawnDelegate.Broadcast(PrevPawn, InPawn);
	}
}
