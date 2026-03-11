// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNodeReference.h"
#include "NipcatLocomotionSystemFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATLOCOMOTIONSYSTEM_API UNipcatLocomotionSystemFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Locomotion System Function Library", meta=(BlueprintThreadSafe))
	static bool IsAnimNodeEquals(FAnimNodeReference NodeA, FAnimNodeReference NodeB);
	
	/**
 * Calculates the actor's relative stop location.
 * This function calculates a stop location by using the friction and braking deceleration
 * variables from the Character Movement Component and applying them to a velocity vector, using
 * the tick's DeltaTime to simulate the deceleration that will be applied for every frame.
 * The predicted stop location is then incremented by the newly calculated velocity vector and
 * DeltaTime. This calculation is looped until the velocity vector has reached zero.
 * @param Velocity:						The current velocity.
 * @param bUseSeparateBrakingFriction:	Indicates whether to use BrakingFriction * BrakingFrictionFactor (TRUE) or GroundFriction * BrakingFrictionFactor (FALSE).
 * @param BrakingFriction:				The drag coefficient applied when braking.
 * @param GroundFriction:				The opposing ground "force" that scales with velocity.
 * @param BrakingFrictionFactor:		The coefficient used to multiply the value of friction used (either BrakingFriction or GroundFriction).
 * @param BrakingDecelerationWalking:	The constant opposing "force" that directly lowers velocity by a constant amount.
 * @param DeltaTime:					The time between ticks of the Engine.
 * @return Returns the location where the owning actor will reach zero velocity.
 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Locomotion System Function Library", meta = (BlueprintThreadSafe))
	static FVector CalculateStopLocation(const FVector& Velocity, const bool bUseSeparateBrakingFriction, const float BrakingFriction,
			const float GroundFriction, const float BrakingFrictionFactor, const float BrakingDecelerationWalking, const float DeltaTime);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Nipcat Locomotion System Function Library", meta = (BlueprintThreadSafe))
	static FVector CalculateStopSpeed(const float& StopDistance, const bool bUseSeparateBrakingFriction, const float BrakingFriction,
			const float GroundFriction, const float BrakingFrictionFactor, const float BrakingDecelerationWalking, const float DeltaTime);
};
