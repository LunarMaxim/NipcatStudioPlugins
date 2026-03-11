// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffectExtension.h"
#include "UObject/Object.h"
#include "NipcatAttributeSet.generated.h"

// Uses macros from AttributeSet.h
// Getter宏获取的是CurrentValue，Setter宏设置的是BaseValue，Initter宏同时修改二者

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS(BlueprintType)
class NIPCATABILITYSYSTEM_API UNipcatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	virtual UWorld* GetWorld() const override;

	UFUNCTION(BlueprintImplementableEvent)
	void K2_PostGameplayEffectExecute(const FGameplayEffectSpecHandle& EffectSpec,
		const FGameplayModifierEvaluatedData& EvaluatedData, UAbilitySystemComponent* Target);
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	
	virtual void ClampAttribute(const FGameplayAttribute& ChangedAttribute, float& NewValue) const;
	virtual void ClampMaxAttribute(const FGameplayAttribute& ChangedAttribute, float OldValue, float NewValue) const;
	virtual void ApplyDamageOrHealing(const FGameplayEffectModCallbackData& Data);

	virtual FGameplayEventData MakeEventDataWithEffectData(const FGameplayEffectModCallbackData& Data, const float MagnitudeOverride = -1.f);
	virtual void SendGameplayEventToOwnerAndInstigator(const FGameplayEventData& Payload, const FGameplayTag OwnerReceiveEventTag, const FGameplayTag& InstigatorReceiveEventTag);
	virtual void SendGameplayEventToOwner(const FGameplayEventData& Payload, const FGameplayTag ReceiveEventTag);
	virtual void AddStat(const FGameplayEffectModCallbackData& Data, const FGameplayTag StatTag);

	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="MaxAttribute", ScriptMethod))
	static float GetEffectiveMaxAttributeValue(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute);
};
