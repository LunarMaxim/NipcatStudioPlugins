// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatCustomMovementMode.h"

#include "NipcatBasicGameplay/Character/NipcatCharacterMovementComponent.h"


void UNipcatCustomMovementMode::AddCustomMovementInput(UNipcatCharacterMovementComponent* CMC, FVector InputVector,
                                                       float DeltaTime)
{
	BP_AddCustomMovementInput(CMC, InputVector, DeltaTime);
}

void UNipcatCustomMovementMode::PreEnter(UNipcatCharacterMovementComponent* CMC)
{
	if (IgnoreMoveInput)
	{
		if (const auto Controller = CMC->GetOwner()->GetInstigatorController())
		{
			Controller->SetIgnoreMoveInput(true);
		}
	}
	BP_PreEnter(CMC);
}

void UNipcatCustomMovementMode::PostEnter(UNipcatCharacterMovementComponent* CMC)
{
	BP_PostEnter(CMC);
}

void UNipcatCustomMovementMode::OnUpdate(UNipcatCharacterMovementComponent* CMC, float DeltaTime)
{
	BP_OnUpdate(CMC, DeltaTime);
}

void UNipcatCustomMovementMode::PreExit(UNipcatCharacterMovementComponent* CMC)
{
	BP_PreExit(CMC);
}

void UNipcatCustomMovementMode::PostExit(UNipcatCharacterMovementComponent* CMC)
{
	BP_PostExit(CMC);
	if (IgnoreMoveInput)
	{
		if (const auto Controller = CMC->GetOwner()->GetInstigatorController())
		{
			Controller->SetIgnoreMoveInput(false);
		}
	}
}

UWorld* UNipcatCustomMovementMode::GetWorld() const
{
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& Context : WorldContexts)
	{
		if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game)) && (Context.World() != NULL))
		{
			return Context.World();
		}
	}
	return nullptr;
}
