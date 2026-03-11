// Copyright Nipcat Studio All Rights Reserved.


#include "GameplayEffectAdjustment.h"

bool UGameplayEffectAdjustment::CanApplyEffectAdjustment_Implementation(
	const FGameplayEffectContextHandle& EffectContextHandle, UAbilitySystemComponent* SourceASC,
	UAbilitySystemComponent* TargetASC)
{
	return true;
}

void UGameplayEffectAdjustment::TryApplyEffectAdjustment(TMap<FGameplayTag, float>& OutAdjustmentData)
{
	for (auto& Data : Adjustments)
	{
		const float OldMagnitude = OutAdjustmentData.FindOrAdd(Data.Tag);
		
		switch (Data.MagnitudeOp)
		{
		case EGameplayModOp::Additive:
			OutAdjustmentData[Data.Tag] = OldMagnitude + Data.Magnitude;
			break;
		case EGameplayModOp::Multiplicitive:
			OutAdjustmentData[Data.Tag] = OldMagnitude * Data.Magnitude;
			break;
		case EGameplayModOp::Division:
			OutAdjustmentData[Data.Tag] = OldMagnitude / Data.Magnitude;
			break;
		case EGameplayModOp::Override:
			OutAdjustmentData[Data.Tag] = Data.Magnitude;
			break;
		}
	}
}
