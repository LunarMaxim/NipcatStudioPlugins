// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotifyTypes.h"
#include "UObject/Object.h"
#include "NipcatFXDefinition_DurationGameplayCue.generated.h"

struct FGameplayCueParameters;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType, EditInlineNew, Abstract, meta=(ShowWorldContextPin), CollapseCategories)
class NIPCATABILITYSYSTEM_API UNipcatFXScript_DurationGameplayCue : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(WorldContext = "MyTarget"))
	void OnExecute(AActor* MyTarget, const FGameplayCueParameters& Parameters);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(WorldContext = "MyTarget"))
	void WhileActive(AActor* MyTarget, const FGameplayCueParameters& Parameters);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, meta=(WorldContext = "MyTarget"))
	void OnRemove(AActor* MyTarget, const FGameplayCueParameters& Parameters);
	
};
