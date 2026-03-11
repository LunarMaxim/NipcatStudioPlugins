// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatCharacter.h"

#include "GenericTeamAgentInterface.h"
#include "NipcatCapsuleComponent.h"
#include "NipcatCharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "NipcatBasicGameplay/Team/NipcatTeamComponent.h"
#include "NipcatBasicGameplay/Team/NipcatTeamComponentOwnerInterface.h"

ANipcatCharacter::ANipcatCharacter(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	bUseControllerRotationYaw = false;

	GetMesh()->bUpdateOverlapsOnAnimationFinalize = false;
	
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->InitCapsuleSize(50.f, 90.f);
	}
	if (DisableCollision)
	{
		SetActorEnableCollision(false);
	}
	if (DisableMovement)
	{
		if (GetCharacterMovement())
		{
			GetCharacterMovement()->SetMovementMode(MOVE_Flying);
		}
	}
}

void ANipcatCharacter::SetDisableCollision(bool Value)
{
	SetActorEnableCollision(!Value);
}

void ANipcatCharacter::SetDisableMovement(bool Value)
{
	GetCharacterMovement()->SetMovementMode(Value ? MOVE_Flying : MOVE_Walking);
}

UNipcatInputComponent* ANipcatCharacter::GetNipcatInputComponent() const
{
	if (GetController())
	{
		if (const auto Interface = Cast<INipcatInputComponentOwnerInterface>(GetController()))
		{
			return Interface->GetNipcatInputComponent();
		}
	}
	return nullptr;
}

void ANipcatCharacter::FaceRotation(FRotator NewControlRotation, float DeltaTime)
{
	// 用于解决AIController Rotation突变导致的不平滑问题
	const FRotator CurrentRotation = FMath::RInterpTo(GetActorRotation(), NewControlRotation, DeltaTime, FaceRotationInterpSpeed);
	Super::FaceRotation(CurrentRotation, DeltaTime);
}

bool ANipcatCharacter::CanJumpInternal_Implementation() const
{
	// same as ACharacter's implementation but without the crouch check
	return JumpIsAllowedInternal();
}


void ANipcatCharacter::SetGenericTeamId(const FGenericTeamId& TeamID)
{
	if (UNipcatTeamComponent* NipcatTeamComponent = INipcatTeamComponentOwnerInterface::Execute_GetNipcatTeamComponent(this))
	{
		return NipcatTeamComponent->OverrideCurrentTeamTagByGenericTeamId(TeamID);
	}
}

FGenericTeamId ANipcatCharacter::GetGenericTeamId() const
{
	if (UNipcatTeamComponent* NipcatTeamComponent = INipcatTeamComponentOwnerInterface::Execute_GetNipcatTeamComponent(this))
	{
		return NipcatTeamComponent->GetCurrentGenericTeamId();
	}
	if (Controller)
	{
		return Cast<IGenericTeamAgentInterface>(Controller)->GetGenericTeamId();
	}
	return FGenericTeamId::NoTeam;
}

ETeamAttitude::Type ANipcatCharacter::GetTeamAttitudeTowards(const AActor& Other) const
{
	if (UNipcatTeamComponent* NipcatTeamComponent = INipcatTeamComponentOwnerInterface::Execute_GetNipcatTeamComponent(this))
	{
		return NipcatTeamComponent->GetTeamAttitudeTowards(&Other);
	}
	if (Controller)
	{
		return Cast<IGenericTeamAgentInterface>(Controller)->GetTeamAttitudeTowards(Other);
	}
	return ETeamAttitude::Neutral;
}
