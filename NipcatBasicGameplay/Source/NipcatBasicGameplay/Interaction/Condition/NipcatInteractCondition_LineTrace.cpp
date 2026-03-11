// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition_LineTrace.h"

#include "Kismet/GameplayStatics.h"
#include "NipcatBasicGameplay/Interaction/NipcatInteractableComponent.h"

bool UNipcatInteractCondition_LineTrace::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
                                                                    UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	FVector TraceEnd;
	if (UseCameraLocation)
	{
		FRotator Rotation;
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPlayerViewPoint(TraceEnd, Rotation);
	}
	else
	{
		TraceEnd = InteractComponent->GetOwner()->GetActorLocation();
	}
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	FHitResult HitResult;
	const bool bHit = UKismetSystemLibrary::LineTraceSingle(Interactable, Interactable->GetInteractPointLocation(),
	                                                        TraceEnd,
	                                                        TraceChannel, true, ActorsToIgnore,
	                                                        DrawDebug
		                                                        ? EDrawDebugTrace::Type::ForOneFrame
		                                                        : EDrawDebugTrace::Type::None
	                                                        , HitResult,
	                                                        IgnoreSelf, FLinearColor::Red, FLinearColor::Green);
	if (DrawDebug && bHit)
	{
		UKismetSystemLibrary::PrintString(GetWorld(), FString::Printf(TEXT("%s Hit Component %s"), *Interactable->GetName(), *HitResult.Component->GetName()));
	}
	if (!bHit)
	{
		return true;
	}
	OutFailedReason = GetFailedReason(Interactable);
	return false;
}
