// Copyright Nipcat Studio All Rights Reserved.


#include "CharacterRotationModifier.h"

void UCharacterRotationModifier::Initialize_Implementation(ACharacter* Character)
{
}


FRotator UCharacterRotationModifier::GetDesiredRotation(ACharacter* Character, FRotator CurrentRotation,
	float DeltaTime)
{
	return BlueprintGetDesiredRotation(Character, CurrentRotation, DeltaTime) + FRotator(0, GetYawOffset(Character, CurrentRotation, DeltaTime), 0);
}

FRotator UCharacterRotationModifier::BlueprintGetDesiredRotation_Implementation(ACharacter* Character,
	FRotator CurrentRotation, float DeltaTime)
{
	return CurrentRotation;
}

float UCharacterRotationModifier::GetRotationRateScale_Implementation(ACharacter* Character, FRotator CurrentRotation, float DeltaTime) const
{
	return RotationRateScale;
}

float UCharacterRotationModifier::GetYawOffset_Implementation(ACharacter* Character, FRotator CurrentRotation, float DeltaTime) const
{
	return YawOffset;
}

UWorld* UCharacterRotationModifier::GetWorld() const
{
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& Context : WorldContexts)
	{
		if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game)) && (Context.World() != NULL))
		{
			return Context.World();
		}
	}
	return Super::GetWorld();
}
