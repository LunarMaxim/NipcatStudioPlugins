// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAbilitySystemWorldConditionSchema.h"
#include "AbilitySystemComponent.h"

bool FNipcatAbilitySystemWorldConditionBase::Initialize(const UWorldConditionSchema& Schema)
{
	const UNipcatAbilitySystemWorldConditionSchema* AbilitySchema = Cast<UNipcatAbilitySystemWorldConditionSchema>(&Schema);
	if (AbilitySchema == nullptr)
	{
		return false;
	}
	ASCRef = AbilitySchema->GetASCRef();
	
	return true;
}


UNipcatAbilitySystemWorldConditionSchema::UNipcatAbilitySystemWorldConditionSchema(const FObjectInitializer& ObjectInitializer): Super(ObjectInitializer)
{
	ASCRef = AddContextDataDesc(TEXT("ASC"), UAbilitySystemComponent::StaticClass(), EWorldConditionContextDataType::Persistent);
}
