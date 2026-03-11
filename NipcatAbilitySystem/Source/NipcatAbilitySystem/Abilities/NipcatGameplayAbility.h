// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "NipcatAbilitySystem/EffectContext/NipcatPlayerMontageParameter.h"
#include "NipcatBasicGameplay/Stat/NipcatStatInterface.h"
#include "NipcatGameplayAbility.generated.h"

class UNipcatAbilityTask_PlayMontageAndWait;
/**
 * 
 */
UCLASS(Abstract, HideCategories = Input)
class NIPCATABILITYSYSTEM_API UNipcatGameplayAbility : public UGameplayAbility, public INipcatStatInterface
{
	GENERATED_BODY()
public:

    UNipcatGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "- Config -", meta=(Categories="Montage", ForceInlineRow))
	FNipcatMontagePlaySettingMap MontagesToPlay;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "- Config -")
	bool BlockInputOnPlayMontage = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "- Config -", meta=(EditCondition="BlockInputOnPlayMontage"))
	int32 DefaultInputBlockLevel = 0;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Costs", meta=(Categories="Cost", ForceInlineRow))
	TMap<FGameplayTag, FScalableFloat> Cost;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Costs")
	bool SkipCostCheck = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Costs")
	bool SkipCostCommit = false;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Costs", meta=(EditCondition="!SkipCostCommit && !SkipCostCheck", EditConditionHides))
	bool ForceCost = true;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cooldowns")
	FScalableFloat CooldownTimeDuration;

	UPROPERTY(Transient)
	float LastApplyTime = -1.0f;

	UPROPERTY(Transient)
	mutable bool CheckingCanActivateAbility = false;

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	
	virtual bool CheckCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;

	virtual void CommitExecute(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	virtual float GetCooldownTimeRemaining(const FGameplayAbilityActorInfo* ActorInfo) const override;
	virtual void GetCooldownTimeRemainingAndDuration(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, float& TimeRemaining, float& CooldownDuration) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility,
	                        bool bWasCancelled) override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEndAbility, bool, bWasCancelled);
	UPROPERTY(BlueprintAssignable)
	FOnEndAbility OnEndAbility;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, AdvancedDisplay)
	FNipcatStat AbilityStat;

	virtual FNipcatStat& GetStat() override;
	
};
