// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAttributeSet.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayEffectExtension.h"
#include "NipcatResourceConfig.h"
#include "NipcatAbilitySystem/Settings/NipcatAbilitySystemDeveloperSettings.h"
#include "NipcatBasicGameplay/Stat/NipcatStatFunctionLibrary.h"

UWorld* UNipcatAttributeSet::GetWorld() const
{
	if (const UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}
	return nullptr;
}

void UNipcatAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UNipcatAttributeSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UNipcatAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);

	ClampMaxAttribute(Attribute, OldValue, NewValue);
}

void UNipcatAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	ApplyDamageOrHealing(Data);

	// const FGameplayEffectSpecHandle EffectSpecHandle = FGameplayEffectSpecHandle(&const_cast<FGameplayEffectSpec&>(Data.EffectSpec));
	// K2_PostGameplayEffectExecute(EffectSpecHandle, Data.EvaluatedData, &Data.Target);
}

void UNipcatAttributeSet::ClampAttribute(const FGameplayAttribute& ChangedAttribute, float& NewValue) const
{
	if (UNipcatResourceConfig* AttributeConfig = UNipcatAbilitySystemDeveloperSettings::Get()->ResourceConfigPath.LoadSynchronous())
	{
		for (const auto& [BaseAttribute, Definition] : AttributeConfig->ResourceDefinition)
		{
			if (ChangedAttribute == BaseAttribute)
			{
				NewValue = FMath::Clamp(NewValue, 0.0f, GetEffectiveMaxAttributeValue(GetOwningAbilitySystemComponent(), ChangedAttribute));
				break;
			}
		}
	}
}

void UNipcatAttributeSet::ClampMaxAttribute(const FGameplayAttribute& ChangedAttribute, float OldValue, float NewValue) const
{
	UNipcatResourceConfig* AttributeConfig = UNipcatAbilitySystemDeveloperSettings::Get()->ResourceConfigPath.LoadSynchronous();
	for (const auto& [BaseAttribute, Definition] : AttributeConfig->ResourceDefinition)
	{
		if (ChangedAttribute == Definition.MaxAttribute || ChangedAttribute == Definition.PenaltyAttribute)
		{
			if (const auto ASC = GetOwningAbilitySystemComponent())
			{
				const float EffectiveMaxValue = GetEffectiveMaxAttributeValue(ASC, BaseAttribute);
				if (BaseAttribute.GetNumericValue(this) > EffectiveMaxValue)
				{
					ASC->SetNumericAttributeBase(BaseAttribute, EffectiveMaxValue);
				}
			}
			break;
		}
	}
}

void UNipcatAttributeSet::ApplyDamageOrHealing(const FGameplayEffectModCallbackData& Data)
{
	UNipcatResourceConfig* AttributeConfig = UNipcatAbilitySystemDeveloperSettings::Get()->ResourceConfigPath.LoadSynchronous();
	
	for (const auto& [BaseAttribute, Definition] : AttributeConfig->ResourceDefinition)
	{
		if (Data.EvaluatedData.Attribute == Definition.DamageAttribute)
		{
			const float DamageValue = Definition.DamageAttribute.GetNumericValue(this);
			const float CurrentAttributeValue = BaseAttribute.GetNumericValue(this);
			const bool IsAccumulateResource = Definition.IsAccumulateResource;
			
			if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
			{
				const float MaxAttributeValue = GetEffectiveMaxAttributeValue(ASC, BaseAttribute);
				if (DamageValue > 0.f)
				{
					const float NewAttributeValue = FMath::Clamp(IsAccumulateResource ?
						CurrentAttributeValue + DamageValue :
						CurrentAttributeValue - DamageValue,
						0.f, MaxAttributeValue);
					if ((CurrentAttributeValue > 0.f && !IsAccumulateResource) || (CurrentAttributeValue < MaxAttributeValue && IsAccumulateResource))
					{
						ASC->SetNumericAttributeBase(BaseAttribute, NewAttributeValue);
						SendGameplayEventToOwnerAndInstigator(MakeEventDataWithEffectData(Data),
							Definition.ReceiveDamageEventTag,
							Definition.ApplyDamageEventTag);
						AddStat(Data, Definition.DamageAppliedStatTag);
						if (NewAttributeValue <= 0.f || NewAttributeValue >= MaxAttributeValue)
						{
							SendGameplayEventToOwnerAndInstigator(MakeEventDataWithEffectData(Data),
								Definition.OutOfDamageEventTag,
								Definition.OverKillEventTag);
						}
					}
					
					if (NewAttributeValue <= 0.f || NewAttributeValue >= MaxAttributeValue)
					{
						SendGameplayEventToOwnerAndInstigator(MakeEventDataWithEffectData(Data, DamageValue - CurrentAttributeValue),
							Definition.ReceiveOverflowDamageEventTag,
							Definition.ApplyOverflowDamageEventTag);
					}
					
					SendGameplayEventToOwnerAndInstigator(MakeEventDataWithEffectData(Data),
	Definition.ReceiveRawDamageEventTag,
	Definition.ApplyRawDamageEventTag);
					
					ASC->SetNumericAttributeBase(Definition.DamageAttribute, 0);
				}
			}
			break;
		}
		
		else if (Data.EvaluatedData.Attribute == Definition.HealingAttribute)
		{
			const float HealingValue = Definition.HealingAttribute.GetNumericValue(this);
			const float CurrentAttributeValue = BaseAttribute.GetNumericValue(this);
			const bool IsAccumulateResource = Definition.IsAccumulateResource;
			
			if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
			{
				const float MaxAttributeValue = ASC->GetNumericAttribute(Definition.MaxAttribute);
				const float EffectiveMaxAttributeValue = GetEffectiveMaxAttributeValue(ASC, BaseAttribute);
				
				if (HealingValue > 0.f)
				{
					const float NewAttributeValue = FMath::Clamp(IsAccumulateResource ?
						CurrentAttributeValue - HealingValue :
						CurrentAttributeValue + HealingValue,
						0.f, EffectiveMaxAttributeValue);

					if ((CurrentAttributeValue > 0.f && IsAccumulateResource) || (CurrentAttributeValue < EffectiveMaxAttributeValue && !IsAccumulateResource))
					{
						ASC->SetNumericAttributeBase(BaseAttribute, NewAttributeValue);
					
						SendGameplayEventToOwnerAndInstigator(MakeEventDataWithEffectData(Data),
							Definition.ReceiveHealingEventTag,
							Definition.ApplyHealingEventTag);
						AddStat(Data, Definition.HealingAppliedStatTag);
						
						if (NewAttributeValue <= 0.f || NewAttributeValue >= EffectiveMaxAttributeValue)
						{
							SendGameplayEventToOwnerAndInstigator(MakeEventDataWithEffectData(Data),
								Definition.FullyRestoreEventTag,
								Definition.OverHealingEventTag);
						}

						if (NewAttributeValue <= 0.f || NewAttributeValue >= MaxAttributeValue)
						{
							SendGameplayEventToOwnerAndInstigator(MakeEventDataWithEffectData(Data),
								Definition.FullyRestoreToMaxValueEventTag,
								Definition.OverHealingToMaxValueEventTag);
						}
					}
					
					if (NewAttributeValue <= 0.f || NewAttributeValue >= EffectiveMaxAttributeValue)
                    {
                    	SendGameplayEventToOwnerAndInstigator(MakeEventDataWithEffectData(Data, HealingValue + CurrentAttributeValue - EffectiveMaxAttributeValue),
                    		Definition.ReceiveOverflowHealingEventTag,
                    		Definition.ApplyOverflowHealingEventTag);
                    }
                    					
					SendGameplayEventToOwnerAndInstigator(MakeEventDataWithEffectData(Data),
	Definition.ReceiveRawHealingEventTag,
	Definition.ApplyRawHealingEventTag);
					
					ASC->SetNumericAttributeBase(Definition.HealingAttribute, 0);
				}
				break;
			}
		}
		
		if (Data.EvaluatedData.Attribute == Definition.CostAttribute)
        {
        	const float CostValue = Definition.CostAttribute.GetNumericValue(this);
        	const float CurrentAttributeValue = BaseAttribute.GetNumericValue(this);
			const bool IsAccumulateResource = Definition.IsAccumulateResource;
        	
        	if (UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent())
        	{
        		const float MaxAttributeValue = GetEffectiveMaxAttributeValue(ASC, BaseAttribute);
        		if (CostValue > 0.f)
        		{
        			const float NewAttributeValue = FMath::Clamp(IsAccumulateResource ?
        				CurrentAttributeValue + CostValue :
        				CurrentAttributeValue - CostValue,
        				0.f, MaxAttributeValue);
					if ((CurrentAttributeValue > 0.f && !IsAccumulateResource) || (CurrentAttributeValue < MaxAttributeValue && IsAccumulateResource))
        			{
        				ASC->SetNumericAttributeBase(BaseAttribute, NewAttributeValue);
        				
        				SendGameplayEventToOwner(MakeEventDataWithEffectData(Data),
        					Definition.PostApplyCostEventTag);
        			}

        			if (NewAttributeValue <= 0.f || NewAttributeValue >= MaxAttributeValue)
        			{
        				SendGameplayEventToOwner(MakeEventDataWithEffectData(Data, CostValue - CurrentAttributeValue),
        					Definition.PostApplyOverflowCostEventTag);
        			}
        			
        			SendGameplayEventToOwner(MakeEventDataWithEffectData(Data),
						Definition.PostApplyRawCostEventTag);
        			
        			ASC->SetNumericAttributeBase(Definition.CostAttribute, 0);
        		}
        	}
        	break;
        }
	}
}

FGameplayEventData UNipcatAttributeSet::MakeEventDataWithEffectData(const FGameplayEffectModCallbackData& Data, const float MagnitudeOverride)
{
	FGameplayEventData Payload;
	Payload.Instigator = Data.EffectSpec.GetEffectContext().GetInstigator();;
	Payload.Target = GetOwningActor();
	Payload.OptionalObject = Data.EffectSpec.Def;
	Payload.ContextHandle = Data.EffectSpec.GetEffectContext();
	Payload.InstigatorTags = *Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
	Payload.TargetTags = *Data.EffectSpec.CapturedTargetTags.GetAggregatedTags();
	Payload.EventMagnitude = MagnitudeOverride >= 0 ? MagnitudeOverride : Data.EvaluatedData.Magnitude;
	return Payload;
}

void UNipcatAttributeSet::SendGameplayEventToOwnerAndInstigator(const FGameplayEventData& Payload,
                                                                const FGameplayTag OwnerReceiveEventTag, const FGameplayTag& InstigatorReceiveEventTag)
{
	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetOwningAbilitySystemComponent();
	FScopedPredictionWindow OwnerScopedWindow(OwnerAbilitySystemComponent, true);
	OwnerAbilitySystemComponent->HandleGameplayEvent(OwnerReceiveEventTag, &Payload);
	
	UAbilitySystemComponent* InstigatorAbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Payload.Instigator.Get());
	FScopedPredictionWindow InstigatorScopedWindow(InstigatorAbilitySystemComponent, true);
	InstigatorAbilitySystemComponent->HandleGameplayEvent(InstigatorReceiveEventTag, &Payload);
}

void UNipcatAttributeSet::SendGameplayEventToOwner(const FGameplayEventData& Payload,
	const FGameplayTag ReceiveEventTag)
{
	UAbilitySystemComponent* OwnerAbilitySystemComponent = GetOwningAbilitySystemComponent();
	FScopedPredictionWindow OwnerScopedWindow(OwnerAbilitySystemComponent, true);
	OwnerAbilitySystemComponent->HandleGameplayEvent(ReceiveEventTag, &Payload);
}

void UNipcatAttributeSet::AddStat(const FGameplayEffectModCallbackData& Data, const FGameplayTag StatTag)
{
	const TScriptInterface<INipcatStatInterface> StatInterface(const_cast<UGameplayAbility*>(Data.EffectSpec.GetEffectContext().GetAbilityInstance_NotReplicated()));
	UNipcatStatFunctionLibrary::AddFloatStat(StatInterface, StatTag, Data.EvaluatedData.Magnitude);
}

float UNipcatAttributeSet::GetEffectiveMaxAttributeValue(UAbilitySystemComponent* ASC, const FGameplayAttribute& Attribute)
{
	if (UNipcatResourceConfig* AttributeConfig = UNipcatAbilitySystemDeveloperSettings::Get()->ResourceConfigPath.LoadSynchronous())
	{
		if (const FResourceDefinition* AttributeConfigData = AttributeConfig->ResourceDefinition.Find(Attribute))
		{
			const float MaxValue = ASC->GetNumericAttribute(AttributeConfigData->MaxAttribute);
			if (AttributeConfigData->PenaltyAttribute.IsValid())
			{
				const float PenaltyValue = ASC->GetNumericAttribute(AttributeConfigData->PenaltyAttribute);
				return MaxValue - PenaltyValue;
			}
			return MaxValue;
		}
	}
	return 0.f;
}
