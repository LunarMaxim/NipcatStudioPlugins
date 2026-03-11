// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatStatInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NipcatStatFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatStatFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Bool"))
	static void SetBoolStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag, bool NewValue);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Int"))
	static void SetIntStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag, int32 NewValue);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Float"))
	static void SetFloatStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag, float NewValue);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Enum"))
	static void SetEnumStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag, uint8 NewValue);

	UFUNCTION(BlueprintPure, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Bool"))
	static bool GetBoolStat(const TScriptInterface<INipcatStatInterface>& StatOwner, FGameplayTag Tag);

	UFUNCTION(BlueprintPure, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Int"))
	static int32 GetIntStat(const TScriptInterface<INipcatStatInterface>& StatOwner, FGameplayTag Tag);

	UFUNCTION(BlueprintPure, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Float"))
	static float GetFloatStat(const TScriptInterface<INipcatStatInterface>& StatOwner, FGameplayTag Tag);

	UFUNCTION(BlueprintPure, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Enum"))
	static uint8 GetEnumStat(const TScriptInterface<INipcatStatInterface>& StatOwner, FGameplayTag Tag);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Int"))
	static void AddIntStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag, int32 AddValue);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Stat", meta=(GameplayTagFilter="Stat.Float"))
	static void AddFloatStat(TScriptInterface<INipcatStatInterface> StatOwner, FGameplayTag Tag, float AddValue);

};
