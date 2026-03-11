// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "GameplayEffectAdjustment.generated.h"

/**
 * 
 */

struct FGameplayCueTag;
USTRUCT()
struct FActiveEffectAdjustmentHandle
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY()
	TObjectPtr<UGameplayEffectAdjustment> EffectAdjustment;
};

USTRUCT(BlueprintType)
struct FGameplayEffectAdjustmentData
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ExposeFunctionCategories="EffectAdjustment"))
	FGameplayTag Tag = FGameplayTag::EmptyTag;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	float Magnitude = 0.0f;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	TEnumAsByte<EGameplayModOp::Type> MagnitudeOp = EGameplayModOp::Type::Additive;
};

UCLASS(DefaultToInstanced, EditInlineNew, Blueprintable, BlueprintType)
class NIPCATABILITYSYSTEM_API UGameplayEffectAdjustment : public UObject
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	bool CanApplyEffectAdjustment(const FGameplayEffectContextHandle& EffectContextHandle, UAbilitySystemComponent* SourceASC,
	   UAbilitySystemComponent* TargetASC);

	UFUNCTION()
	void TryApplyEffectAdjustment(TMap<FGameplayTag, float>& OutAdjustmentData);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Adjustment, meta=(Categories="EffectAdjustment"))
	TArray<FGameplayEffectAdjustmentData> Adjustments;
};

