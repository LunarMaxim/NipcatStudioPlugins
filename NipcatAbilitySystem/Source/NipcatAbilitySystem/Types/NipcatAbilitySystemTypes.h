// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "UObject/Object.h"
#include "GameplayEffectTypes.h"
#include "Abilities/GameplayAbility.h"
#include "NipcatAbilitySystem/EffectContext/NipcatPlayerMontageParameter.h"
#include "NipcatAbilitySystemTypes.generated.h"


class UProxyTable;

UENUM(BlueprintType)
enum class ENipcatComparisonMethod : uint8
{
	Equal_To,
	Not_Equal_To,
	Greater_Than_Or_Equal_To,
	Less_Than_Or_Equal_To,
	Greater_Than,
	Less_Than
};

UENUM(BlueprintType)
enum class ENipcatFloatValueSource : uint8
{
	DirectSet,
	ScalableFloat,
	BackingAttribute,
	GameplayEffectModifier
};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatFloatParameter
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENipcatFloatValueSource ValueSource = ENipcatFloatValueSource::DirectSet;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="ValueSource==ENipcatFloatValueSource::DirectSet", EditConditionHides))
	double Value = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="ValueSource==ENipcatFloatValueSource::ScalableFloat", EditConditionHides))
	FScalableFloat ScalableValue = FScalableFloat();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="ValueSource==ENipcatFloatValueSource::BackingAttribute", EditConditionHides))
	FGameplayAttribute BackingAttribute = FGameplayAttribute();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="ValueSource==ENipcatFloatValueSource::GameplayEffectModifier", EditConditionHides))
	TSoftClassPtr<UGameplayEffect> GameplayEffect = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="ValueSource==ENipcatFloatValueSource::GameplayEffectModifier", EditConditionHides))
	int32 ModifierIndex = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	double Coefficient = 1.f;

	double GetFloatParameterValue(const UAbilitySystemComponent* ASC, const UGameplayAbility* GameplayAbility = nullptr) const
	{
		switch (ValueSource)
		{
		case ENipcatFloatValueSource::DirectSet:
			return Value * Coefficient;
		case ENipcatFloatValueSource::ScalableFloat:
			return ScalableValue.GetValueAtLevel(GameplayAbility ? GameplayAbility->GetAbilityLevel() : 0, nullptr) * Coefficient;
		case ENipcatFloatValueSource::BackingAttribute:
			if (ASC && BackingAttribute.IsValid())
			{
				return ASC->GetNumericAttribute(BackingAttribute) * Coefficient;
			}
			return 0.f;
		case ENipcatFloatValueSource::GameplayEffectModifier:
			if (ASC && !GameplayEffect.IsNull() && ModifierIndex >= 0)
			{
				FGameplayEffectSpec	Spec(Cast<UGameplayEffect>(GameplayEffect.LoadSynchronous()->GetDefaultObject()), ASC->MakeEffectContext(), GameplayAbility ? GameplayAbility->GetAbilityLevel() : 0);
				Spec.CalculateModifierMagnitudes();
				return Spec.GetModifierMagnitude(ModifierIndex) * Coefficient;
			}
		}
		return 0.f;
	};
};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatFloatComparator
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNipcatFloatParameter A = FNipcatFloatParameter();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ENipcatComparisonMethod ComparisonMethod = ENipcatComparisonMethod::Greater_Than;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNipcatFloatParameter B = FNipcatFloatParameter();

	bool Compare(const UAbilitySystemComponent* ASC, const UGameplayAbility* GameplayAbility = nullptr) const
	{
		const double ValueA = A.GetFloatParameterValue(ASC, GameplayAbility);
		const double ValueB = A.GetFloatParameterValue(ASC, GameplayAbility);
		switch (ComparisonMethod)
		{
		case ENipcatComparisonMethod::Equal_To:
			return ValueA == ValueB;
		case ENipcatComparisonMethod::Not_Equal_To:
			return ValueA != ValueB;
		case ENipcatComparisonMethod::Greater_Than_Or_Equal_To:
			return ValueA >= ValueB;
		case ENipcatComparisonMethod::Less_Than_Or_Equal_To:
			return ValueA <= ValueB;
		case ENipcatComparisonMethod::Greater_Than:
			return ValueA > ValueB;
		case ENipcatComparisonMethod::Less_Than:
			return ValueA < ValueB;
		}
		return false;
	}
};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatNameRequirements
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequireNames_Any;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> RequireNames_All;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FName> IgnoreNames;

	bool RequirementsMet(const FName& Name) const;
	bool RequirementsMet(const TArray<FName>& Names) const;

	bool IsEmpty() const;

	bool operator==(const FNipcatNameRequirements& Other) const;
	bool operator!=(const FNipcatNameRequirements& Other) const;

};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatPoiseData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FNipcatFloatParameter MaxPoiseValue = FNipcatFloatParameter();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	double RegenPercentagePerSecond = 0.1f;

	// Negative Value Means Never Regen
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(Units="s"))
	double RegenDelay = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(InlineEditConditionToggle))
	bool bUseChooser = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="!bUseChooser", ShowOnlyInnerProperties))
	FNipcatMontagePlaySetting MontageToPlay;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="bUseChooser"))
	UProxyTable* MontageToPlayProxyTable;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<TSoftClassPtr<UGameplayEffect>> EffectsApplyToSelf;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FNipcatNameRequirements HitComponentTagRequirements;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FNipcatNameRequirements HitBoneNameRequirements;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagRequirements TagRequirements;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTagRequirements LockPoiseTagRequirements;
	
};
