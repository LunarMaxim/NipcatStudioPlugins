// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "NipcatAbilitySystem/NipcatAbilitySystemBlueprintFunctionLibrary.h"

UNipcatGameplayAbility::UNipcatGameplayAbility(const FObjectInitializer& ObjectInitializer)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::Type::InstancedPerActor;
}

bool UNipcatGameplayAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	CheckingCanActivateAbility = true;
	const bool Result = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
	CheckingCanActivateAbility = false;
	return Result;
}

bool UNipcatGameplayAbility::CheckCooldown(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (LastApplyTime < 0.f)
	{
		return true;
	}
	if (UWorld* World = GetWorld())
	{
		float CurrentTime = World->GetTimeSeconds();
		return CurrentTime - LastApplyTime > CooldownTimeDuration.GetValueAtLevel(GetAbilityLevel());
	}
	return true;
}

bool UNipcatGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (SkipCostCheck)
	{
		return true;
	}
	if (const UGameplayEffect* CostGE = GetCostGameplayEffect())
	{
		if (const auto ASC = ActorInfo->AbilitySystemComponent.Get())
		{
			if (!CheckingCanActivateAbility)
			{
				if (ForceCost)
				{
					return true;
				}
			}
			if (!UNipcatAbilitySystemBlueprintFunctionLibrary::CanApplyAttributeModifiers(ASC, CostGE, GetAbilityLevel(Handle, ActorInfo), MakeEffectContext(Handle, ActorInfo)))
			{
				const FGameplayTag& CostTag = UAbilitySystemGlobals::Get().ActivateFailCostTag;

				if (OptionalRelevantTags && CostTag.IsValid())
				{
					OptionalRelevantTags->AddTag(CostTag);
				}
				return false;
			}
		}
	}
	return true;
}

void UNipcatGameplayAbility::CommitExecute(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (UWorld* World = GetWorld())
	{
		LastApplyTime = World->GetTimeSeconds();
	}
	Super::CommitExecute(Handle, ActorInfo, ActivationInfo);
}

void UNipcatGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (UWorld* World = GetWorld())
	{
		float* pLastApplyTime = const_cast<float*>(&LastApplyTime);
		*pLastApplyTime = World->GetTimeSeconds();
	}
}

void UNipcatGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (SkipCostCommit)
	{
		return;
	}
	Super::ApplyCost(Handle, ActorInfo, ActivationInfo);
}

float UNipcatGameplayAbility::GetCooldownTimeRemaining(const FGameplayAbilityActorInfo* ActorInfo) const
{
	if (LastApplyTime < 0.f)
	{
		return 0.f;
	}
	
	if (UWorld* World = GetWorld())
	{
		const float CurrentTime = World->GetTimeSeconds();
		return FMath::Max(0, CooldownTimeDuration.GetValueAtLevel(GetAbilityLevel()) - (CurrentTime - LastApplyTime));
	}
	
	return 0.f;
}

void UNipcatGameplayAbility::GetCooldownTimeRemainingAndDuration(FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, float& TimeRemaining, float& CooldownDuration) const
{
	CooldownDuration = CooldownTimeDuration.GetValueAtLevel(GetAbilityLevel());
	
	if (LastApplyTime < 0.f)
	{
		return;
	}
	
	if (UWorld* World = GetWorld())
	{
		const float CurrentTime = World->GetTimeSeconds();
		TimeRemaining = FMath::Max(0, CooldownDuration - (CurrentTime - LastApplyTime));
	}
}

void UNipcatGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	
	if (TriggerEventData && bHasBlueprintActivateFromEvent)
	{
		// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
		K2_ActivateAbilityFromEvent(*TriggerEventData);
	}
	if (bHasBlueprintActivate)
	{
		// A Blueprinted ActivateAbility function must call CommitAbility somewhere in its execution chain.
		K2_ActivateAbility();
	}
	else
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{			
			constexpr bool bReplicateEndAbility = true;
			constexpr bool bWasCancelled = true;
			EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
		}
	}
}

void UNipcatGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ScopeLockCount > 0)
		{
			return;
		}

		if	(OnEndAbility.IsBound())
		{
			OnEndAbility.Broadcast(bWasCancelled);
		}
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FNipcatStat& UNipcatGameplayAbility::GetStat()
{
	return AbilityStat;
}

