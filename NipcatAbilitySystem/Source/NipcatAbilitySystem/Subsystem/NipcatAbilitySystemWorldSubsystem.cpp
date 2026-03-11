// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAbilitySystemWorldSubsystem.h"

#include "NipcatAbilitySystem/NipcatAbilitySystemComponent.h"

UDamageInstance* UNipcatAbilitySystemWorldSubsystem::AllocDamageInstance(UObject* Outer, const FDamageDefinition& DamageDefinition, const FGameplayEffectSpecHandle InEffectSpec)
{
	UObject* OuterObject = IsValid(Outer) ? Outer : this;
	UDamageInstance* DamageInstance = NewObject<UDamageInstance>(OuterObject);
	DamageInstance->DamageDefinition = DamageDefinition;
	DamageInstance->EffectSpec = InEffectSpec;
	DamageInstances.FindOrAdd(OuterObject).Array.Add(DamageInstance);
	if (const auto ASC = InEffectSpec.Data.Get()->GetEffectContext().GetInstigatorAbilitySystemComponent())
	{
		DamageInstance->InstigatorAbilitySystemComponent = ASC;
		DamageInstance->InstigatorGameplayTagCountChangedDelegate = ASC->RegisterGenericGameplayTagEvent().AddUObject(
			DamageInstance, &UDamageInstance::OnInstigatorGameplayTagCountChanged);
		DamageInstance->InstigatorOwnedTags = ASC->GetOwnedGameplayTags();
	}

	for (auto It = DamageInstances.CreateIterator(); It; ++It)
	{
		if (!IsValid(It.Key()))
		{
			It.RemoveCurrent();
		}
	}
	return DamageInstance;
}
