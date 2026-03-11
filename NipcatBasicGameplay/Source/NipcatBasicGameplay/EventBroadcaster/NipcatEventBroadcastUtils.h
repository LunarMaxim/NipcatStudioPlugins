// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "NipcatEventBroadcastUtils.generated.h"

class UNipcatEventBroadcastFilter;
struct FGameplayEffectContextHandle;
struct FGameplayEventData;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatEventBroadcastUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Broadcast")
	static void BroadcastEvent(
		UPARAM(meta=(Categories = "GameplayEvent"))FGameplayTag EventTag,
		FGameplayEffectContextHandle ContextHandle,
		AActor* SourceActor,
		const FVector LocationOffset,
		const float RotationOffsetYaw,
		const TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes,
		TArray<TSubclassOf<UNipcatEventBroadcastFilter>> Filters,
		float Radius = 500.f,
		float Direction = 0.f,
		float HalfAngle = 180.f,
		EDrawDebugTrace::Type DrawDebugType = EDrawDebugTrace::Type::None,
		FLinearColor TraceColor = FLinearColor::Red,
		FLinearColor TraceHitColor = FLinearColor::Green,
		float DrawTime = 2.f);
};
