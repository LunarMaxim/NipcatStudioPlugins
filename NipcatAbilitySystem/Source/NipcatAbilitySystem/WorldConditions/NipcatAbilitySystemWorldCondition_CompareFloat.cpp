// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAbilitySystemWorldCondition_CompareFloat.h"
#include "WorldConditionContext.h"


FWorldConditionResult FNipcatAbilitySystemWorldCondition_CompareFloat::IsTrue(const FWorldConditionContext& Context) const
{
	FWorldConditionResult Result(EWorldConditionResultValue::IsTrue, bCanCacheResult);
	if (const UAbilitySystemComponent* ASC = Context.GetContextDataPtr<UAbilitySystemComponent>(ASCRef))
	{
		if (!Comparator.Compare(ASC))
		{
			Result.Value = EWorldConditionResultValue::IsFalse;
		}
	}
	
	return Result;
}