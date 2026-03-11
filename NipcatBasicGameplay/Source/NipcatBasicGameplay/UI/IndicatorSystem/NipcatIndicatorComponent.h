// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTags.h"
#include "IndicatorDescriptor.h"
#include "NipcatIndicatorComponent.generated.h"



UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent))
class NIPCATBASICGAMEPLAY_API UNipcatIndicatorComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UNipcatIndicatorComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="Indicator"), Category="- Config -")
	FGameplayTag IndicatorType;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(InlineEditConditionToggle), Category="- Config -")
	bool UseCustomIndicatorConfig;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition = "UseCustomIndicatorConfig"), Category="- Config -")
	FNipcatIndicatorConfig CustomIndicatorConfig;

	UFUNCTION(BlueprintCallable)
	void ApplyIndicatorConfig(UIndicatorDescriptor* Indicator);
};
