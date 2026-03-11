// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatIndicatorManagerComponent.generated.h"

#define UE_API NIPCATBASICGAMEPLAY_API

class AController;
class UIndicatorDescriptor;
class UObject;
struct FFrame;

/**
 * @class UNipcatIndicatorManagerComponent
 */
UCLASS(MinimalAPI, BlueprintType, Blueprintable, meta=(BlueprintSpawnableComponent), ClassGroup="Nipcat")
class UNipcatIndicatorManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UE_API UNipcatIndicatorManagerComponent(const FObjectInitializer& ObjectInitializer);

	static UE_API UNipcatIndicatorManagerComponent* GetComponent(AController* Controller);

	UFUNCTION(BlueprintCallable, Category = Indicator)
	UE_API void AddIndicator(UIndicatorDescriptor* IndicatorDescriptor);
	
	UFUNCTION(BlueprintCallable, Category = Indicator)
	UE_API void RemoveIndicator(UIndicatorDescriptor* IndicatorDescriptor);

	DECLARE_EVENT_OneParam(UNipcatIndicatorManagerComponent, FIndicatorEvent, UIndicatorDescriptor* Descriptor)
	FIndicatorEvent OnIndicatorAdded;
	FIndicatorEvent OnIndicatorRemoved;

	const TArray<UIndicatorDescriptor*>& GetIndicators() const { return Indicators; }

private:
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;
};

#undef UE_API
