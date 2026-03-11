// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAbilitySystemWorldConditionSchema.h"
#include "NipcatAbilitySystem/Types/NipcatAbilitySystemTypes.h"

#include "NipcatAbilitySystemWorldCondition_CompareFloat.generated.h"

/**
 * 
 */
USTRUCT()
struct NIPCATABILITYSYSTEM_API FNipcatAbilitySystemWorldCondition_CompareFloat : public FNipcatAbilitySystemWorldConditionBase
{
	GENERATED_BODY()

	FNipcatAbilitySystemWorldCondition_CompareFloat() = default;

	UPROPERTY(EditAnywhere)
	FNipcatFloatComparator Comparator;
	
	virtual FWorldConditionResult IsTrue(const FWorldConditionContext& Context) const override;
	
};