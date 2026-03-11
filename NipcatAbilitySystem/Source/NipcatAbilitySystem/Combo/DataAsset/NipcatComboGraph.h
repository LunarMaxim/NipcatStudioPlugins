// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "Engine/DataAsset.h"
#include "NipcatAbilitySystem/EffectContext/NipcatPlayerMontageParameter.h"
#include "WorldConditionQuery.h"
#include "WorldConditionContext.h"
#include "NipcatComboGraph.generated.h"

class UNipcatComboComponent;

UENUM(BlueprintType)
enum class ENipcatComboOffsetRule : uint8
{
	Clear,
	Hold,
	Write
};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatComboTransition
{
	GENERATED_BODY()

	FNipcatComboTransition();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Priority = 0;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, meta=(MultiLine))
	FString Comment;

	UPROPERTY()
	bool bCommentModified = false;
#endif
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="GameplayEvent.InputAction"))
	FGameplayTagContainer InputActions;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="Combo"))
	FGameplayTag NextComboTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow))
	FNipcatMontagePlaySettingMap MontagePlaySettingOverride;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(Categories="Combo"))
	FGameplayTagRequirements CurrentComboRequirements;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagRequirements OwningTagRequirements;
	
	// FWorldConditionContextData ContextData;
	
	UPROPERTY(EditAnywhere)
	FWorldConditionQuery Conditions;

	UPROPERTY(EditAnywhere)
	ENipcatComboOffsetRule ComboOffsetRule;
	
	bool CanEnterTransition(const UNipcatComboComponent* ComboComponent, const FGameplayTag& InputEventTag, bool& IsOffset);

	bool IsValid() const
	{
		return NextComboTag.IsValid();
	}

	bool operator==(const FNipcatComboTransition& Other) const
	{

		return Priority == Other.Priority
		&& InputActions == Other.InputActions
		&& NextComboTag == Other.NextComboTag
		&& MontagePlaySettingOverride == Other.MontagePlaySettingOverride
		&& CurrentComboRequirements == Other.CurrentComboRequirements
		&& OwningTagRequirements == Other.OwningTagRequirements;
	}
};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatComboTransitionArray
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(TitleProperty="Comment"))
	TArray<FNipcatComboTransition> Array;
};
	
USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatComboTransitionPriorityMap
{
	GENERATED_BODY()

	FNipcatComboTransitionPriorityMap() = default;

	FNipcatComboTransitionPriorityMap(int32 InPriority)
	{
		Priority = InPriority;
	}
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 Priority = 0;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(Categories="GameplayEvent.InputAction", ForceInlineRow, TitleProperty="Comment"))
	TMap<FGameplayTag, FNipcatComboTransitionArray> Map;
};
	
/**
 * 
 */
UCLASS(BlueprintType)
class NIPCATABILITYSYSTEM_API UNipcatComboGraph : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(TitleProperty="Comment"))
	TArray<FNipcatComboTransition> Transitions;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(TitleProperty="Priority"))
	TArray<FNipcatComboTransitionPriorityMap> TransitionPriorityMaps;
	
	static bool FindTransitionMapIndexByPriority(TArray<FNipcatComboTransitionPriorityMap>& InTransitionPriorityMaps,
		const int32& InPriority, int32& OutIndex);
	
	UFUNCTION(BlueprintCallable)
	virtual void BuildTransitionPriorityMap();
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
	virtual void PostLoad() override;
#endif
	
};
