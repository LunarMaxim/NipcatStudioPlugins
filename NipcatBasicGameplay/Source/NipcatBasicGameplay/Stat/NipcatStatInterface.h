// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "NipcatStatInterface.generated.h"

USTRUCT(BlueprintType)
struct NIPCATBASICGAMEPLAY_API FNipcatStat
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories = "Stat.Bool"))
	TMap<FGameplayTag, bool> BoolStat;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories = "Stat.Int"))
	TMap<FGameplayTag, int32> IntStat;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories = "Stat.Float"))
	TMap<FGameplayTag, float> FloatStat;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories = "Stat.Enum"))
	TMap<FGameplayTag, uint8> EnumStat;

};

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable, MinimalAPI)
class UNipcatStatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATBASICGAMEPLAY_API INipcatStatInterface
{
	GENERATED_BODY()

public:
	FNipcatStat CacheStat;
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName = GetStat, Category="Nipcat Stat")
	void K2_GetStat(UPARAM(ref)FNipcatStat& OutStat);

	UFUNCTION(Category="Nipcat Stat")
	virtual FNipcatStat& GetStat();
};
