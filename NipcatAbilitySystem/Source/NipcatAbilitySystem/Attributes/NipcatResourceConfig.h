// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "GameplayTagContainer.h"
#include "NipcatResourceConfig.generated.h"

USTRUCT(BlueprintType)
struct FResourceDefinition : public FTableRowBase
{
	GENERATED_BODY()
		
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsAccumulateResource;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayAttribute MaxAttribute;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayAttribute DamageAttribute;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayAttribute HealingAttribute;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayAttribute CostAttribute;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayAttribute PenaltyAttribute;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostApplyDamage"))
	FGameplayTag ApplyDamageEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostApplyRawDamage"))
	FGameplayTag ApplyRawDamageEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostApplyOverflowDamage"))
	FGameplayTag ApplyOverflowDamageEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.OverKill"))
	FGameplayTag OverKillEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostReceiveDamage"))
	FGameplayTag ReceiveDamageEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostReceiveRawDamage"))
	FGameplayTag ReceiveRawDamageEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostReceiveOverflowDamage"))
	FGameplayTag ReceiveOverflowDamageEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.OutOfDamage"))
	FGameplayTag OutOfDamageEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostApplyHealing"))
	FGameplayTag ApplyHealingEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostApplyRawHealing"))
	FGameplayTag ApplyRawHealingEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostApplyOverflowHealing"))
	FGameplayTag ApplyOverflowHealingEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.OverHealing"))
	FGameplayTag OverHealingEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.OverHealingToMaxValue"))
	FGameplayTag OverHealingToMaxValueEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostReceiveHealing"))
	FGameplayTag ReceiveHealingEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostReceiveRawHealing"))
	FGameplayTag ReceiveRawHealingEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostReceiveOverflowHealing"))
	FGameplayTag ReceiveOverflowHealingEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.FullyRestore"))
	FGameplayTag FullyRestoreEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.FullyRestoreToMaxValue"))
	FGameplayTag FullyRestoreToMaxValueEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostApplyCost"))
	FGameplayTag PostApplyCostEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostApplyRawCost"))
	FGameplayTag PostApplyRawCostEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.PostApplyOverflowCost"))
	FGameplayTag PostApplyOverflowCostEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="Stat.Float"))
	FGameplayTag DamageAppliedStatTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="Stat.Float"))
	FGameplayTag HealingAppliedStatTag;
};
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class NIPCATABILITYSYSTEM_API UNipcatResourceConfig : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, meta=(ForceInlineRow))
	TMap<FGameplayAttribute, FResourceDefinition> ResourceDefinition;
};
