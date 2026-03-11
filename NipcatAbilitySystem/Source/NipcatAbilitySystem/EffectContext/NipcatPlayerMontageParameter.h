// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NipcatAbilitySystem/AnimNotify/LinkedAnimNotify/NipcatLinkedAnimNotify.h"
#include "UObject/Object.h"
#include "NipcatPlayerMontageParameter.generated.h"



USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatMontagePlaySetting : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TObjectPtr<UAnimMontage> MontageToPlay;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float PlayRate{1.f};
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName StartSection = NAME_None;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool StopWhenAbilityEnds = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double AnimRootMotionTranslationScale = 1.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double StartTimeSeconds = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float Weight = 100.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FAnimNotifyData> AdditionalNotifies;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bOverrideDefaultInputBlockLevel = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="bOverrideDefaultInputBlockLevel"))
	int32 InputBlockLevelOverride = -1;
	
	bool operator==(const FNipcatMontagePlaySetting& Other) const
	{
		return MontageToPlay == Other.MontageToPlay
		&& PlayRate == Other.PlayRate
		&& StartSection == Other.StartSection
		&& StopWhenAbilityEnds == Other.StopWhenAbilityEnds
		&& AnimRootMotionTranslationScale == Other.AnimRootMotionTranslationScale
		&& StartTimeSeconds == Other.StartTimeSeconds
		&& Weight == Other.Weight
		// && AdditionalNotifies == Other.AdditionalNotifies
		&& bOverrideDefaultInputBlockLevel == Other.bOverrideDefaultInputBlockLevel
		&& InputBlockLevelOverride == Other.InputBlockLevelOverride;
	}
};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatMontagePlaySettingArray : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FNipcatMontagePlaySetting> Array;
};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatMontagePlaySettingMap : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories="Montage"))
	TMap<FGameplayTag, FNipcatMontagePlaySetting> Map;

	bool IsEmpty()
	{
		for (const auto& [Key, Value] : Map)
		{
			if (Value.MontageToPlay.Get())
			{
				return false;
			}
		}
		return true;
	}

	bool operator==(const FNipcatMontagePlaySettingMap& Other) const
	{
		TArray<FGameplayTag> Keys;
		TArray<FNipcatMontagePlaySetting> Values;
		TArray<FGameplayTag> OtherKeys;
		TArray<FNipcatMontagePlaySetting> OtherValues;
		Map.GetKeys(Keys);
		Map.GenerateValueArray(Values);
		Other.Map.GetKeys(OtherKeys);
		Other.Map.GenerateValueArray(OtherValues);
		return Keys == OtherKeys && Values == OtherValues;
	}
};