// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatEventBroadcastUtils.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "KismetAnimationLibrary.h"
#include "NipcatEventBroadcastFilter.h"
#include "Abilities/GameplayAbilityTypes.h"

void UNipcatEventBroadcastUtils::BroadcastEvent(FGameplayTag EventTag, FGameplayEffectContextHandle ContextHandle, AActor* SourceActor,
	const FVector LocationOffset, const float RotationOffsetYaw, const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
	TArray<TSubclassOf<UNipcatEventBroadcastFilter>> Filters, float Radius, float Direction, float HalfAngle,
	EDrawDebugTrace::Type DrawDebugType, FLinearColor TraceColor, FLinearColor TraceHitColor, float DrawTime)
{
	if (!SourceActor)
	{
		return;
	}
	
	const FVector TraceStartLocation = SourceActor->GetActorLocation() + SourceActor->GetActorRotation().RotateVector(LocationOffset);
	const FRotator TraceForwardRotation = SourceActor->GetActorRotation().Add(0.f, RotationOffsetYaw, 0.f);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Emplace(SourceActor);
	
	TArray<FHitResult> OutHits;
	
	UKismetSystemLibrary::SphereTraceMultiForObjects(
		SourceActor,
		TraceStartLocation,
		TraceStartLocation,
		Radius,
		ObjectTypes,
		false,
		ActorsToIgnore,
		DrawDebugType,
		OutHits,
		true,
		TraceColor,
		TraceHitColor,
		DrawTime);

	TArray<AActor*> SentActors;
	
	for (const FHitResult& HitResult : OutHits)
	{
		if (HitResult.bBlockingHit)
		{
			// Prevent Multi Sending
			AActor* HitActor = HitResult.GetActor();
			if (SentActors.Contains(HitActor))
			{
				continue;
			}
			SentActors.Add(HitActor);
			
			const float TargetDirection = UKismetAnimationLibrary::CalculateDirection(HitActor->GetActorLocation() - TraceStartLocation, TraceForwardRotation);
			FRotator DeltaRotation = FRotator(0.f, Direction, 0.f) - FRotator(0.f, TargetDirection, 0.f);
			DeltaRotation.Normalize();
			if (FMath::Abs(DeltaRotation.Yaw) > HalfAngle)
			{
				continue;
			}
			if (!Filters.IsEmpty())
			{
				bool ShouldBroadcast = true;
				for (TSubclassOf<UNipcatEventBroadcastFilter>& Filter : Filters)
				{
					if (!Filter.GetDefaultObject()->ShouldBroadcastEvent(SourceActor, HitActor))
					{
						ShouldBroadcast = false;
						break;
					}
				}
				if (!ShouldBroadcast)
				{
					continue;
				}
			}
			
			FGameplayEventData Payload;
			Payload.Instigator = SourceActor;
			Payload.Target = HitActor;
			Payload.ContextHandle = ContextHandle;
			// Payload.ContextHandle.AddHitResult(HitResult); Debug Mode Breakpoint
			
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(HitActor, EventTag, Payload);
		}
	}
}
