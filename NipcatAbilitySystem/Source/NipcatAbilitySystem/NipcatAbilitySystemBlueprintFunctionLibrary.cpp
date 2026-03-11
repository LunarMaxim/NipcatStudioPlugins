// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAbilitySystemBlueprintFunctionLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "NipcatAbilitySystemGlobals.h"
#include "Abilities/NipcatGameplayAbility.h"
#include "AnimNotify/NipcatAnimNotify.h"
#include "AnimNotify/NipcatAnimNotifyState.h"
#include "Attributes/NipcatAttributeSet.h"
#include "Attributes/NipcatResourceConfig.h"
#include "EffectContext/NipcatGameplayEffectContext.h"
#include "EffectContext/NipcatPlayerMontageParameter.h"
#include "GameFramework/Character.h"
#include "NipcatAbilitySystem/Types/NipcatAbilitySystemTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Settings/NipcatAbilitySystemDeveloperSettings.h"
#include "Subsystem/NipcatAbilitySystemWorldSubsystem.h"


UObject* UNipcatAbilitySystemBlueprintFunctionLibrary::GetAnimNotifyObject(const FAnimNotifyEvent& AnimNotifyEvent)
{
	return AnimNotifyEvent.Notify.Get()? Cast<UObject>(AnimNotifyEvent.Notify.Get()) : Cast<UObject>(AnimNotifyEvent.NotifyStateClass.Get());
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::DoesAnimNotifyEventImplementInterface(
	const FAnimNotifyEvent& AnimNotifyEvent, const TSubclassOf<UInterface> Interface)
{
	if (const UObject* NotifyObject = GetAnimNotifyObject(AnimNotifyEvent))
	{
		return NotifyObject->GetClass()->ImplementsInterface(Interface);
	}
	return false;
}


void UNipcatAbilitySystemBlueprintFunctionLibrary::GetAnimNotifies(UAnimMontage* AnimMontage, const FNipcatMontagePlaySetting& MontagePlaySetting, 
                                                                   TArray<FAnimNotifyData>& OutNotifies)
{
	if (!AnimMontage)
	{
		return;
	}
	if (!MontagePlaySetting.AdditionalNotifies.IsEmpty())
	{
		OutNotifies.Append(MontagePlaySetting.AdditionalNotifies);
	}
	for (FAnimNotifyEvent& AnimNotify : AnimMontage->Notifies)
	{
		const FAnimNotifyData AnimNotifyData(AnimNotify);
		OutNotifies.Add(AnimNotifyData);

		UObject* NotifyObject = GetAnimNotifyObject(AnimNotify);
		if (DoesAnimNotifyEventImplementInterface(AnimNotify, UNipcatLinkableAnimNotifyInterface::StaticClass()))
		{
			TArray<FNipcatLinkedAnimNotify> LinkedAnimNotifies = INipcatLinkableAnimNotifyInterface::Execute_GetLinkedAnimNotifies(NotifyObject);
			if (!LinkedAnimNotifies.IsEmpty())
			{
				OutNotifies.Append(FNipcatLinkedAnimNotify::ConvertLinkedNotifyData(LinkedAnimNotifies, AnimNotifyData.StartTime, AnimNotifyData.EndTime, AnimMontage));
			}
		}
	}
	OutNotifies.StableSort([](const FAnimNotifyData& A, const FAnimNotifyData& B)
	{
		return A.StartTime <= B.StartTime;
	});
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::GetAnimNotifiesInSection(UAnimMontage* AnimMontage,
	const FNipcatMontagePlaySetting& MontagePlaySetting, FName SectionName, TArray<FAnimNotifyData>& OutNotifies)
{
	if (!AnimMontage)
	{
		return;
	}
	if (!MontagePlaySetting.AdditionalNotifies.IsEmpty())
	{
		OutNotifies.Append(MontagePlaySetting.AdditionalNotifies);
	}

	float SectionStartTime;
	float SectionEndTime;
	AnimMontage->GetSectionStartAndEndTime(AnimMontage->GetSectionIndex(SectionName), SectionStartTime, SectionEndTime);
	
	for (FAnimNotifyEvent& AnimNotify : AnimMontage->Notifies)
	{
		if (AnimNotify.NotifyStateClass)
		{
			if (AnimNotify.GetEndTriggerTime() < SectionStartTime || AnimNotify.GetTriggerTime() > SectionEndTime)
			{
				continue;
			}
		}
		else if (AnimNotify.Notify)
		{
			if (!(AnimNotify.GetTriggerTime() > SectionStartTime && AnimNotify.GetTriggerTime() < SectionEndTime))
			{
				continue;
			}
		}
		const FAnimNotifyData AnimNotifyData(AnimNotify);
		OutNotifies.Add(AnimNotifyData);

		UObject* NotifyObject = GetAnimNotifyObject(AnimNotify);
		if (DoesAnimNotifyEventImplementInterface(AnimNotify, UNipcatLinkableAnimNotifyInterface::StaticClass()))
		{
			TArray<FNipcatLinkedAnimNotify> LinkedAnimNotifies = INipcatLinkableAnimNotifyInterface::Execute_GetLinkedAnimNotifies(NotifyObject);
			if (!LinkedAnimNotifies.IsEmpty())
			{
				OutNotifies.Append(FNipcatLinkedAnimNotify::ConvertLinkedNotifyData(LinkedAnimNotifies, AnimNotifyData.StartTime, AnimNotifyData.EndTime, AnimMontage));
			}
		}
	}
	OutNotifies.StableSort([](const FAnimNotifyData& A, const FAnimNotifyData& B)
	{
		return A.StartTime <= B.StartTime;
	});
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::SortNotifies(TArray<FAnimNotifyData>& Notifies)
{
	Notifies.StableSort([](const FAnimNotifyData& A, const FAnimNotifyData& B)
	{
		return A.StartTime <= B.StartTime;
	});
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::GetAllAnimNotifiesByInterface(UAnimMontage* AnimMontage,
                                                                                 const TSubclassOf<UInterface> Interface, TArray<FAnimNotifyData>& OutNotifies, bool IncludeLinkedNotify)
{
	if (!AnimMontage)
	{
		return;
	}
	for (FAnimNotifyEvent& AnimNotify : AnimMontage->Notifies)
	{
		if (DoesAnimNotifyEventImplementInterface(AnimNotify, Interface))
		{
			const FAnimNotifyData AnimNotifyData(AnimNotify);
			OutNotifies.Add(AnimNotifyData);

			if (IncludeLinkedNotify)
			{
				UObject* NotifyObject = GetAnimNotifyObject(AnimNotify);
				if (DoesAnimNotifyEventImplementInterface(AnimNotify, UNipcatLinkableAnimNotifyInterface::StaticClass()))
				{
					TArray<FNipcatLinkedAnimNotify> LinkedAnimNotifies = INipcatLinkableAnimNotifyInterface::Execute_GetLinkedAnimNotifies(NotifyObject);
					if (!LinkedAnimNotifies.IsEmpty())
					{
						OutNotifies.Append(FNipcatLinkedAnimNotify::ConvertLinkedNotifyData(LinkedAnimNotifies, AnimNotifyData.StartTime, AnimNotifyData.EndTime, AnimMontage));
					}
				}
			}
		}
	}
	OutNotifies.StableSort([](const FAnimNotifyData& A, const FAnimNotifyData& B)
	{
		return A.StartTime <= B.StartTime;
	});
}


void UNipcatAbilitySystemBlueprintFunctionLibrary::GetAnimNotifyStatesByClass(
	UAnimMontage* AnimMontage, const TSubclassOf<UAnimNotifyState> Class, TArray<FAnimNotifyData>& OutNotifies, bool IncludeLinkedNotify)
{
	if (!AnimMontage)
	{
		return;
	}
	
	for (FAnimNotifyEvent& AnimNotify : AnimMontage->Notifies)
	{
		if (UAnimNotifyState* NotifyState = AnimNotify.NotifyStateClass)
		{
			if (NotifyState->IsA(Class))
			{
				const FAnimNotifyData AnimNotifyData(AnimNotify);
				OutNotifies.Add(AnimNotifyData);

				if (IncludeLinkedNotify && NotifyState->GetClass()->ImplementsInterface(UNipcatLinkableAnimNotifyInterface::StaticClass()))
				{
					TArray<FAnimNotifyData> LinkedNotifies = FNipcatLinkedAnimNotify::ConvertLinkedNotifyData(
						INipcatLinkableAnimNotifyInterface::Execute_GetLinkedAnimNotifies(NotifyState),
						AnimNotifyData.StartTime,
						AnimNotifyData.EndTime,
						AnimMontage);
					
					for (const auto LinkedNotify : LinkedNotifies)
					{
						if (LinkedNotify.NotifyState && LinkedNotify.NotifyState->IsA(Class))
						{
							OutNotifies.Emplace(LinkedNotify);
						}
					}
				}
			}
		}
	}
	OutNotifies.StableSort([](const FAnimNotifyData& A, const FAnimNotifyData& B)
	{
		return A.StartTime <= B.StartTime;
	});
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::GetAnimNotifyStatesByInterface(UAnimMontage* AnimMontage,
	const TSubclassOf<UInterface> Interface, TArray<FAnimNotifyData>& OutNotifies, bool IncludeLinkedNotify)
{
	if (!AnimMontage)
	{
		return;
	}
	
	for (FAnimNotifyEvent& AnimNotify : AnimMontage->Notifies)
	{
		if (UAnimNotifyState* NotifyState = AnimNotify.NotifyStateClass)
		{
			if (NotifyState->GetClass()->ImplementsInterface(Interface))
			{
				const FAnimNotifyData AnimNotifyData(AnimNotify);
				OutNotifies.Add(AnimNotifyData);

				if (IncludeLinkedNotify && NotifyState->GetClass()->ImplementsInterface(UNipcatLinkableAnimNotifyInterface::StaticClass()))
				{
					TArray<FAnimNotifyData> LinkedNotifies = FNipcatLinkedAnimNotify::ConvertLinkedNotifyData(
						INipcatLinkableAnimNotifyInterface::Execute_GetLinkedAnimNotifies(NotifyState),
						AnimNotifyData.StartTime,
						AnimNotifyData.EndTime,
						AnimMontage);
					
					for (const auto LinkedNotify : LinkedNotifies)
					{
						if (LinkedNotify.NotifyState && LinkedNotify.NotifyState->GetClass()->ImplementsInterface(Interface))
						{
							OutNotifies.Emplace(LinkedNotify);
						}
					}
				}
			}
		}
	}
	OutNotifies.StableSort([](const FAnimNotifyData& A, const FAnimNotifyData& B)
	{
		return A.StartTime <= B.StartTime;
	});
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::RemoveInvalidNotifies(UAnimSequenceBase* AnimationSequenceBase)
{
	if (AnimationSequenceBase)
	{
		const int32 RemovedCount = AnimationSequenceBase->Notifies.RemoveAll(
			[&](const FAnimNotifyEvent& Event)
		{
			return !Event.Notify && !Event.NotifyStateClass;
		});

		if (RemovedCount > 0)
		{
			// Refresh all cached data
			AnimationSequenceBase->RefreshCacheData();
			return true;
		}
		return false;
	}
	return false;
}

float UNipcatAbilitySystemBlueprintFunctionLibrary::GetFloatCurveValue(UAnimSequenceBase* Animation, FName CurveName, float Time, float DefaultValue)
{
	if (!Animation)
	{
		return DefaultValue;
	}
	for (const FFloatCurve& FloatCurve : Animation->GetCurveData().FloatCurves)
	{
		if (FloatCurve.GetName() == CurveName)
		{
			return FloatCurve.Evaluate(Time);
		}
	}
	return DefaultValue;
}

float UNipcatAbilitySystemBlueprintFunctionLibrary::EvaluateRuntimeCurve(float InEvaluatePos,
                                                                         const FRuntimeFloatCurve& InCurve)
{
	return InCurve.GetRichCurveConst()->Eval(InEvaluatePos);
}

double UNipcatAbilitySystemBlueprintFunctionLibrary::CalculateCurveQuadrature(UCurveFloat* Curve, int SubSteps,
																		 double StartTime, double EndTime)
{
	if (EndTime == 0.f)
	{
		double MinTime;
		double MaxTime;
		Curve->GetTimeRange(MinTime, MaxTime);
		EndTime = MaxTime;
	}
	
	const double TimeLength = EndTime - StartTime;
	const double StepLength = UKismetMathLibrary::SafeDivide(TimeLength, SubSteps);
	double CurrentTime = StepLength * 0.5 + StartTime;
	double Sum = 0.f;
	for (int i = StartTime; i < SubSteps; i++)
	{
		const double CurrentValue = Curve->GetFloatValue(CurrentTime);
		Sum += CurrentValue * StepLength;
		CurrentTime += StepLength;
	}
	return Sum;
}

FNipcatMontagePlaySetting UNipcatAbilitySystemBlueprintFunctionLibrary::GetOverriddenMontagePlaySetting(
	const FNipcatMontagePlaySetting& InMontagePlaySetting, const FNipcatMontagePlaySettingMap& InOverrideMap,
	const FGameplayTag& MontageTag)
{
	FNipcatMontagePlaySetting Result = InMontagePlaySetting;
	if (const FNipcatMontagePlaySetting* OverrideSetting = InOverrideMap.Map.Find(MontageTag))
	{
		if (OverrideSetting->MontageToPlay)
		{
			Result.MontageToPlay = OverrideSetting->MontageToPlay;
		}
		if (OverrideSetting->PlayRate > 0)
		{
			Result.PlayRate = OverrideSetting->PlayRate;
		}
		if (OverrideSetting->StartSection != NAME_None)
		{
			Result.StartSection = OverrideSetting->StartSection;
		}
		Result.StopWhenAbilityEnds = OverrideSetting->StopWhenAbilityEnds;
		if (OverrideSetting->AnimRootMotionTranslationScale >= 0)
		{
			Result.AnimRootMotionTranslationScale = OverrideSetting->AnimRootMotionTranslationScale;
		}
		if (OverrideSetting->StartTimeSeconds >= 0)
		{
			Result.StartTimeSeconds = OverrideSetting->StartTimeSeconds;
		}
	}
	return Result;
}

ACharacter* UNipcatAbilitySystemBlueprintFunctionLibrary::GetAvatarCharacterFromAbility(UGameplayAbility* Ability)
{
	return Cast<ACharacter>(Ability->GetAvatarActorFromActorInfo());
}

FGameplayAbilitySpecHandle UNipcatAbilitySystemBlueprintFunctionLibrary::GetCurrentAbilitySpecHandle(
	UGameplayAbility* Ability)
{
	return Ability->GetCurrentAbilitySpecHandle();
}

FGameplayEffectSpecHandle UNipcatAbilitySystemBlueprintFunctionLibrary::MakeOutgoingGameplayEffectSpecByDamageDefinition(UGameplayAbility* Ability,
                                                                                                                                 TSubclassOf<UGameplayEffect> GameplayEffectClass, const FDamageDefinition& DamageDefinition, float Level)
{
	const FGameplayEffectSpecHandle EffectSpec = Ability->MakeOutgoingGameplayEffectSpec(GameplayEffectClass, Level);
	const FGameplayEffectContextHandle EffectContext = EffectSpec.Data.Get()->GetContext();

	// Construct Damage Instance
	UDamageInstance* DamageInstance = Ability->GetWorld()->GetSubsystem<UNipcatAbilitySystemWorldSubsystem>()->AllocDamageInstance(Ability->GetAvatarActorFromActorInfo(), DamageDefinition, EffectSpec);
	SetDamageInstance(EffectContext, DamageInstance);

	// Assign Set By Caller Magnitudes
	for (const TPair<FGameplayTag, float>& Pair : DamageDefinition.SetByCallerMagnitudes)
	{
		EffectSpec.Data.Get()->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
	}
	
	return EffectSpec;
}

FNipcatGameplayAbilityActorInfo UNipcatAbilitySystemBlueprintFunctionLibrary::GetNipcatAbilityActorInfoFromAbility(
	UGameplayAbility* GameplayAbility)
{
	if (const auto& ActorInfo = GameplayAbility->GetCurrentActorInfo())
	{
		return *FNipcatGameplayAbilityActorInfo::ExtractActorInfo(const_cast<FGameplayAbilityActorInfo*>(ActorInfo));
	}
	return FNipcatGameplayAbilityActorInfo();
}

FNipcatMontagePlaySettingMap UNipcatAbilitySystemBlueprintFunctionLibrary::
GetMontagePlaySettingOverrideMapFromActorInfo(UGameplayAbility* GameplayAbility)
{
	return GetNipcatAbilityActorInfoFromAbility(GameplayAbility).MontagePlaySettingOverride.FindOrAdd(GameplayAbility->GetClass());
}

const UGameplayAbility* UNipcatAbilitySystemBlueprintFunctionLibrary::GetSourceAbilityInstanceNotReplicatedFromEffectContext(
	const FGameplayEffectContextHandle& EffectContext)
{
	return EffectContext.GetAbilityInstance_NotReplicated();
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::AddActor(FGameplayEffectContextHandle ContextHandle,
                                                            AActor* Actor, bool bReset)
{
	TArray<TWeakObjectPtr<AActor>> ActorToAdd;
	ActorToAdd.Emplace(Actor);
	ContextHandle.AddActors(ActorToAdd, bReset);
}

const UPhysicalMaterial* UNipcatAbilitySystemBlueprintFunctionLibrary::GetPhysicalMaterial(
	const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FHitResult* HitResultPtr = ContextHandle.GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::SetDamageInstance(
	FGameplayEffectContextHandle ContextHandle, UDamageInstance* DamageInstance)
{
	static_cast<FNipcatGameplayEffectContext*>(ContextHandle.Get())->DamageInstance = DamageInstance;
}

UDamageInstance* UNipcatAbilitySystemBlueprintFunctionLibrary::GetDamageInstance(
	const FGameplayEffectContextHandle& ContextHandle)
{
	if (!ContextHandle.IsValid())
	{
		return nullptr;
	}

	const FNipcatGameplayEffectContext* TypedContext = FNipcatGameplayEffectContext::ExtractEffectContext(ContextHandle);
	check(TypedContext)
	if (UDamageInstance* DamageInstance = TypedContext->DamageInstance.Get())
	{
		return DamageInstance;
	}
	return nullptr;
}

FDamageDefinition UNipcatAbilitySystemBlueprintFunctionLibrary::GetDamageDefinition(
	const FGameplayEffectContextHandle& ContextHandle)
{
	if (UDamageInstance* DamageInstance = GetDamageInstance(ContextHandle))
	{
		return DamageInstance->DamageDefinition;
	}
	return FDamageDefinition();
}

FGameplayEffectContextHandle UNipcatAbilitySystemBlueprintFunctionLibrary::GetEffectContextHandleFromEffectSpec(const FGameplayEffectSpec& Spec)
{
	return Spec.GetContext();
}

const UGameplayEffect* UNipcatAbilitySystemBlueprintFunctionLibrary::GetGameplayEffectDefFromEffectSpec(
	const FGameplayEffectSpecHandle& Spec)
{
	return Spec.Data->Def.Get();
}

float UNipcatAbilitySystemBlueprintFunctionLibrary::GetSetByCallerMagnitudeByTag(
	const FGameplayEffectSpecHandle& EffectSpec, const FGameplayTag& Tag, const bool TryParentTagIfNotFound, const bool WarnIfNotFound, const float DefaultIfNotFound)
{
	if (EffectSpec.IsValid())
	{
		FGameplayTag ActualTag = Tag;
		while (ActualTag.IsValid())
		{
			if (const auto ValuePtr = EffectSpec.Data->SetByCallerTagMagnitudes.Find(ActualTag))
			{
				return *ValuePtr;
			}
			if (TryParentTagIfNotFound)
			{
				ActualTag = ActualTag.RequestDirectParent();
			}
			else
			{
				break;
			}
		}
		if (WarnIfNotFound)
		{
			UE_LOG(LogGameplayEffects, Error, TEXT("FGameplayEffectSpec::GetMagnitude called for Data %s on Def %s when magnitude had not yet been set by caller."), *Tag.ToString(), *EffectSpec.Data->Def->GetName());
		}
	}
	return DefaultIfNotFound;
}

float UNipcatAbilitySystemBlueprintFunctionLibrary::GetSetByCallerMagnitudeByName(
	const FGameplayEffectSpecHandle& EffectSpec, const FName& MagnitudeName, const bool WarnIfNotFound, const float DefaultIfNotFound)
{
	if (EffectSpec.IsValid())
	{
		return EffectSpec.Data->GetSetByCallerMagnitude(MagnitudeName, WarnIfNotFound, DefaultIfNotFound);
	}
	return DefaultIfNotFound;
}

FNipcatGameplayAbilityActorInfo UNipcatAbilitySystemBlueprintFunctionLibrary::GetNipcatAbilityActorInfo(
	UAbilitySystemComponent* ASC)
{
	
	if (const auto& ActorInfo = ASC->AbilityActorInfo.Get())
	{
		return *FNipcatGameplayAbilityActorInfo::ExtractActorInfo(ActorInfo);
	}
	return FNipcatGameplayAbilityActorInfo();
}


FGameplayAbilitySpec UNipcatAbilitySystemBlueprintFunctionLibrary::GetHandleAbilitySpec(UAbilitySystemComponent* ASC, FGameplayAbilitySpecHandle AbilitySpecHandle)
{
	return *(ASC->FindAbilitySpecFromHandle(AbilitySpecHandle));
}


void UNipcatAbilitySystemBlueprintFunctionLibrary::TryActivateAbilityByClassAndReturnInstance(UAbilitySystemComponent* ASC,
                                                                                              TSubclassOf<UGameplayAbility> InAbilityToActivate, bool& bSuccess, UGameplayAbility*& Instance, bool bAllowRemoteActivation)
{
	bSuccess = false;

	const UGameplayAbility* const InAbilityCDO = InAbilityToActivate.GetDefaultObject();

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability == InAbilityCDO)
		{
			bSuccess |= ASC->TryActivateAbility(Spec.Handle, bAllowRemoteActivation);
			if (bSuccess)
			{
				if (!Spec.GetAbilityInstances().IsEmpty())
				{
					Instance = Spec.GetAbilityInstances()[0];
				}
				else
				{
					bSuccess = false;
				}
			}
			break;
		}
	}
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::GetActivatableAbilitySpecHandleByClass(UAbilitySystemComponent* ASC, TSubclassOf<UGameplayAbility> InAbilityClass, FGameplayAbilitySpecHandle& Handle)
{
	if (!ASC || !InAbilityClass)
	{
		return false;
	}
	
	const UGameplayAbility* const InAbilityCDO = InAbilityClass.GetDefaultObject();

	for (const FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
	{
		if (Spec.Ability == InAbilityCDO)
		{
			Handle = Spec.Handle;
			return true;
		}
	}

	return false;
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::GetCooldownRemainingByClass(UAbilitySystemComponent* ASC,
                                                                               TSubclassOf<UGameplayAbility> InAbilityClass, float& CooldownRemaining, float& CooldownDuration)
{
	FGameplayAbilitySpecHandle AbilitySpecHandle;
	if (GetActivatableAbilitySpecHandleByClass(ASC, InAbilityClass, AbilitySpecHandle))
	{
		bool bIsInstance;
		if (const UGameplayAbility* Ability = UAbilitySystemBlueprintLibrary::GetGameplayAbilityFromSpecHandle(ASC, AbilitySpecHandle, bIsInstance))
		{
			Ability->GetCooldownTimeRemainingAndDuration(AbilitySpecHandle, Ability->GetCurrentActorInfo(), CooldownRemaining, CooldownDuration);
			return true;
		}
	}
	return false;
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::SetCooldownDuration(UGameplayAbility* Ability, float NewDuration)
{
	if (UNipcatGameplayAbility* NipcatGameplayAbility = Cast<UNipcatGameplayAbility>(Ability))
	{
		if (UWorld* World = NipcatGameplayAbility->GetWorld())
		{
			const float CurrentTime = World->GetTimeSeconds();
			const float CooldownDuration = NipcatGameplayAbility->CooldownTimeDuration.GetValueAtLevel(NipcatGameplayAbility->GetAbilityLevel());
			NipcatGameplayAbility->LastApplyTime = CurrentTime - CooldownDuration + NewDuration;
			return true;
		}
	}
	return false;
}


bool UNipcatAbilitySystemBlueprintFunctionLibrary::SetGameplayEffectDurationHandle(UAbilitySystemComponent* ASC, FActiveGameplayEffectHandle Handle, float NewDuration)
{
	if (!Handle.IsValid())
	{
		return false;
	}

	const FActiveGameplayEffect* ActiveGameplayEffect = ASC->GetActiveGameplayEffect(Handle);
	if (!ActiveGameplayEffect)
	{
		return false;
	}

	FActiveGameplayEffect* AGE = const_cast<FActiveGameplayEffect*>(ActiveGameplayEffect);
	if (NewDuration > 0)
	{
		AGE->Spec.Duration = NewDuration;
	}
	else
	{
		AGE->Spec.Duration = 0.01f;
	}

	const FActiveGameplayEffectsContainer& ConstActiveGameplayEffects = ASC->GetActiveGameplayEffects();
	FActiveGameplayEffectsContainer& ActiveGameplayEffects = const_cast<FActiveGameplayEffectsContainer&>(ConstActiveGameplayEffects);
	AGE->StartServerWorldTime = ActiveGameplayEffects.GetServerWorldTime();
	AGE->CachedStartServerWorldTime = AGE->StartServerWorldTime;
	AGE->StartWorldTime = ActiveGameplayEffects.GetWorldTime();
	ActiveGameplayEffects.MarkItemDirty(*AGE);
	ActiveGameplayEffects.CheckDuration(Handle);

	AGE->EventSet.OnTimeChanged.Broadcast(AGE->Handle, AGE->StartWorldTime, AGE->GetDuration());
	ASC->OnGameplayEffectDurationChange(*AGE);

	return true;
}

FGameplayEffectContextHandle UNipcatAbilitySystemBlueprintFunctionLibrary::GetEffectContextFromActiveGEHandle(
	const FActiveGameplayEffectHandle& ActiveGameplayEffectHandle)
{
	if (const auto ASC = ActiveGameplayEffectHandle.GetOwningAbilitySystemComponent())
	{
		return ASC->GetEffectContextFromActiveGEHandle(ActiveGameplayEffectHandle);
	}
	return FGameplayEffectContextHandle();
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::CancelAbility(UAbilitySystemComponent* ASC,
                                                                 UGameplayAbility* Ability)
{
	if (ASC)
	{
		ASC->CancelAbility(Ability);
	}
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::CancelAbilityByClass(UAbilitySystemComponent* ASC,
                                                                        TSubclassOf<UGameplayAbility> AbilityClass)
{
	if (ASC && AbilityClass)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = ASC->FindAbilitySpecFromClass(AbilityClass))
		{
			ASC->CancelAbilityHandle(AbilitySpec->Handle);
		}
	}
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::CancelAbilities(UAbilitySystemComponent* ASC,
                                                                   const FGameplayTagContainer WithTags, const FGameplayTagContainer WithoutTags, UGameplayAbility* Ignore)
{
	if (ASC)
	{
		ASC->CancelAbilities(&WithTags, &WithoutTags, Ignore);
	}
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::CancelAllAbilities(UAbilitySystemComponent* ASC,
                                                                      UGameplayAbility* Ignore)
{
	if (ASC)
	{
		ASC->CancelAllAbilities(Ignore);
	}
}

FGameplayAbilityActorInfo UNipcatAbilitySystemBlueprintFunctionLibrary::GetAbilityActorInfo(
	const UAbilitySystemComponent* ASC)
{
	if (ASC)
	{
		if (FGameplayAbilityActorInfo* ActorInfo = ASC->AbilityActorInfo.Get())
		{
			return *ActorInfo;
		}
	}
	return FGameplayAbilityActorInfo();
}

AActor* UNipcatAbilitySystemBlueprintFunctionLibrary::GetAvatarActorFromAbilityActorInfo(
	const UAbilitySystemComponent* ASC)
{
	if (ASC)
	{
		return ASC->GetAvatarActor();
	}
	return nullptr;
}

FActiveGameplayEffect UNipcatAbilitySystemBlueprintFunctionLibrary::GetActiveGameplayEffect(
	const UAbilitySystemComponent* ASC, const FActiveGameplayEffectHandle Handle)
{
	if (ASC)
	{
		if (const FActiveGameplayEffect* ActiveGE = ASC->GetActiveGameplayEffect(Handle))
		{
			return *ActiveGE;
		}
	}
	return FActiveGameplayEffect();
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::CanApplyAttributeModifiers(const UAbilitySystemComponent* ASC,
	const UGameplayEffect* GameplayEffect, float Level, const FGameplayEffectContextHandle& EffectContext)
{
	const auto& ResourceConfig = UNipcatAbilitySystemDeveloperSettings::Get()->ResourceConfigPath.LoadSynchronous();

	if (!ResourceConfig)
	{
		return const_cast<FActiveGameplayEffectsContainer&>(ASC->GetActiveGameplayEffects()).CanApplyAttributeModifiers(GameplayEffect, Level, EffectContext);
	}
			
	FGameplayEffectSpec	Spec(GameplayEffect, EffectContext, Level);

	Spec.CalculateModifierMagnitudes();
	
	for(int32 ModIdx = 0; ModIdx < Spec.Modifiers.Num(); ++ModIdx)
	{
		const FGameplayModifierInfo& ModDef = Spec.Def->Modifiers[ModIdx];
		const FModifierSpec& ModSpec = Spec.Modifiers[ModIdx];
	
		// It only makes sense to check additive operators
		if (ModDef.ModifierOp == EGameplayModOp::Additive)
		{
			if (!ModDef.Attribute.IsValid())
			{
				continue;
			}
			const UAttributeSet* Set = nullptr;
			for (const auto AttributeSet : ASC->GetSpawnedAttributes())
			{
				if (AttributeSet && AttributeSet->IsA(ModDef.Attribute.GetAttributeSetClass()))
				{
					Set = AttributeSet;
				}
			}
			
			float CurrentValue = ModDef.Attribute.GetNumericValueChecked(Set);
			float CostValue = ModSpec.GetEvaluatedMagnitude();

			for (const auto& [ResourceAttribute, ResourceDefinition] : ResourceConfig->ResourceDefinition)
			{
				if (ModDef.Attribute == ResourceDefinition.DamageAttribute || ModDef.Attribute == ResourceDefinition.CostAttribute)
				{
					CostValue *= -1;
					CurrentValue = ResourceAttribute.GetNumericValue(Set);
					if (ResourceDefinition.IsAccumulateResource)
					{
						const float MaxValue = UNipcatAttributeSet::GetEffectiveMaxAttributeValue(const_cast<UAbilitySystemComponent*>(ASC), ResourceAttribute);
						CurrentValue = MaxValue - CurrentValue;						
					}
					break;
				}
			}

			if (CurrentValue + CostValue < 0.f)
			{
				return false;
			}
		}
	}
	return true;
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::SendGameplayEventsToActor(AActor* Actor,
                                                                             FGameplayTagContainer EventTags, FGameplayEventData Payload)
{
	for (const FGameplayTag EventTag : EventTags)
	{
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Actor, EventTag, Payload);
	}
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::BreakGameplayModifierEvaluatedData(
	const FGameplayModifierEvaluatedData& Data, FGameplayAttribute& Attribute,
	TEnumAsByte<EGameplayModOp::Type>& ModifierOp, float& Magnitude, FActiveGameplayEffectHandle& Handle, bool& IsValid)
{
	Attribute = Data.Attribute;
	ModifierOp = Data.ModifierOp;
	Magnitude = Data.Magnitude;
	Handle = Data.Handle;
	IsValid = Data.IsValid;
}


void UNipcatAbilitySystemBlueprintFunctionLibrary::GetActiveAbilitiesWithTags(UAbilitySystemComponent* ASC, const FGameplayTagContainer& GameplayTagContainer, TArray<UGameplayAbility*>& ActiveAbilities)
{
	TArray<FGameplayAbilitySpec*> AbilitiesToActivate;
	ASC->GetActivatableGameplayAbilitySpecsByAllMatchingTags(GameplayTagContainer, AbilitiesToActivate, false);
	// Iterate the list of all ability specs
	for (FGameplayAbilitySpec* Spec : AbilitiesToActivate)
	{
		// Iterate all instances on this ability spec
		TArray<UGameplayAbility*> AbilityInstances = Spec->GetAbilityInstances();

		for (UGameplayAbility* ActiveAbility : AbilityInstances)
		{
			if (ActiveAbility->IsActive())
			{
				ActiveAbilities.Add(ActiveAbility);
			}
		}
	}
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::SetAnimNotifyEventTriggerTime(FAnimNotifyEvent& NotifyEvent, float NewTime)
{
	NotifyEvent.SetTime(NewTime);
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::SetAnimNotifyEventDuration(FAnimNotifyEvent& NotifyEvent,
	float NewTime)
{
	NotifyEvent.SetDuration(NewTime);
}

UAnimSequenceBase* UNipcatAbilitySystemBlueprintFunctionLibrary::GetLinkedAnimSequenceBase(
	const FAnimNotifyEvent& NotifyEvent)
{
	return const_cast<UAnimSequenceBase*>(NotifyEvent.GetLinkedMontage()? NotifyEvent.GetLinkedMontage() : NotifyEvent.GetLinkedSequence());
}

FDamageDefinition UNipcatAbilitySystemBlueprintFunctionLibrary::GetDamageDefinitionByHandle(
	FDamageDefinitionHandle Handle)
{
	return Handle.IsValid() ? *Handle.Get() : FDamageDefinition();
}

void UNipcatAbilitySystemBlueprintFunctionLibrary::SetDamageDefinitionByHandle(FDamageDefinitionHandle Handle,
	const FDamageDefinition& Value)
{
	if (Handle.IsValid())
	{
		*Handle.Get() = Value;
	}
}

FDamageDefinitionHandle UNipcatAbilitySystemBlueprintFunctionLibrary::MakeDamageDefinitionHandle(
	const FDamageDefinition& DamageDefinition)
{
	return FDamageDefinitionHandle(&const_cast<FDamageDefinition&>(DamageDefinition));
}

TArray<float> UNipcatAbilitySystemBlueprintFunctionLibrary::GetMontageSettingRandomWeights(
	const FNipcatMontagePlaySettingArray& MontagePlaySettingArray)
{
	TArray<float> Result;
	for (const FNipcatMontagePlaySetting& Setting : MontagePlaySettingArray.Array)
	{
		Result.Add(Setting.Weight);
	}
	return Result;
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::RequirementsMet(const FGameplayTagRequirements TagRequirements,
	const FGameplayTagContainer Container)
{
	return TagRequirements.IsEmpty() || TagRequirements.RequirementsMet(Container);
}

UAbilitySystemComponent* UNipcatAbilitySystemBlueprintFunctionLibrary::GetOwningAbilitySystemComponent(
	const FActiveGameplayEffectHandle& ActiveGameplayEffectHandle)
{
	return ActiveGameplayEffectHandle.GetOwningAbilitySystemComponent();
}

FActiveGameplayEffect UNipcatAbilitySystemBlueprintFunctionLibrary::GetActiveGameplayEffectFromHandle(
	const FActiveGameplayEffectHandle& ActiveGameplayEffectHandle)
{
	if (const UAbilitySystemComponent* ASC = ActiveGameplayEffectHandle.GetOwningAbilitySystemComponent())
	{
		if (const FActiveGameplayEffect* ActiveGameplayEffect = ASC->GetActiveGameplayEffect(ActiveGameplayEffectHandle))
		{
			return *ActiveGameplayEffect;
		}
	}
	return FActiveGameplayEffect();
}

FActiveGameplayEffectHandle UNipcatAbilitySystemBlueprintFunctionLibrary::GetHandle(
	const FActiveGameplayEffect& ActiveGameplayEffect)
{
	return ActiveGameplayEffect.Handle;
}

float UNipcatAbilitySystemBlueprintFunctionLibrary::GetTimeRemaining(const FActiveGameplayEffect& ActiveGameplayEffect, float WorldTime)
{
	return ActiveGameplayEffect.GetTimeRemaining(WorldTime);
}

float UNipcatAbilitySystemBlueprintFunctionLibrary::GetDuration(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	return ActiveGameplayEffect.GetDuration();
}

float UNipcatAbilitySystemBlueprintFunctionLibrary::GetPeriod(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	return ActiveGameplayEffect.GetPeriod();
}

float UNipcatAbilitySystemBlueprintFunctionLibrary::GetEndTime(const FActiveGameplayEffect& ActiveGameplayEffect)
{
	return ActiveGameplayEffect.GetEndTime();
}

FGameplayEffectSpec UNipcatAbilitySystemBlueprintFunctionLibrary::GetEffectSpec(
	const FActiveGameplayEffect& ActiveGameplayEffect)
{
	return ActiveGameplayEffect.Spec;
}

double UNipcatAbilitySystemBlueprintFunctionLibrary::GetFloatParameterValue(const FNipcatFloatParameter& FloatParameter, const UAbilitySystemComponent* ASC, const UGameplayAbility* GameplayAbility)
{
	return FloatParameter.GetFloatParameterValue(ASC, GameplayAbility);
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::Compare(const FNipcatFloatComparator& FloatComparator, const UAbilitySystemComponent* ASC,
	const UGameplayAbility* GameplayAbility)
{
	return FloatComparator.Compare(ASC, GameplayAbility);
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::NameRequirementsMet(const FNipcatNameRequirements& NameRequirements,
	const FName& Name)
{
	return NameRequirements.RequirementsMet(Name);
}

bool UNipcatAbilitySystemBlueprintFunctionLibrary::NameRequirementsMets(const FNipcatNameRequirements& NameRequirements,
	const TArray<FName>& Names)
{
	return NameRequirements.RequirementsMet(Names);
}

TArray<FString> UNipcatAbilitySystemBlueprintFunctionLibrary::GetAttributeGroupNames()
{
	TSet<FString> ResultSet;
	if (const auto NipcatAbilitySystemGlobals = Cast<UNipcatAbilitySystemGlobals>(&UAbilitySystemGlobals::Get()))
	{
		for (const FSoftObjectPath& AttribDefaultTableName : NipcatAbilitySystemGlobals->GetGlobalAttributeSetDefaultsTableNames())
		{
			if (AttribDefaultTableName.IsValid())
			{
				const UCurveTable* CurveTable = Cast<UCurveTable>(AttribDefaultTableName.TryLoad());
				if (CurveTable)
				{
					TArray<FName> Keys;
					CurveTable->GetRowMap().GenerateKeyArray(Keys);

					for (auto& Key : Keys)
					{
						FString RowName = Key.ToString();
						FString GroupName;
						RowName.Split(TEXT("."), &GroupName, nullptr);
						ResultSet.Add(GroupName);
					}
				}
			}
		}
	}
	TArray<FString> ResultArray = ResultSet.Array();
	ResultArray.Sort();
	ResultArray.Insert(TEXT("None"), 0);
	return ResultArray;
}

const TArray<FGameplayTag>& UNipcatAbilitySystemBlueprintFunctionLibrary::GetGameplayTagArrayFromQuery(
	const FGameplayTagQuery& Query)
{
	return Query.GetGameplayTagArray();
}
