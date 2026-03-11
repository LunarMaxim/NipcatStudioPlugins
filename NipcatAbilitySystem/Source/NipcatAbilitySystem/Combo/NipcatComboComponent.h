// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/NipcatComboDefinition.h"
#include "DataAsset/NipcatComboGraph.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "NipcatComboComponent.generated.h"

struct FScopedComboTransitionLock
{
	NIPCATABILITYSYSTEM_API FScopedComboTransitionLock(UNipcatComboComponent& InComboComponent);
	NIPCATABILITYSYSTEM_API ~FScopedComboTransitionLock();

private:
	UNipcatComboComponent& ComboComponent;
};

UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class NIPCATABILITYSYSTEM_API UNipcatComboComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="- Config -")
	FGameplayTagRequirements ComboOffsetRequirement;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="- Runtime -")
	TArray<UNipcatComboGraph*> ComboGraphs;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="- Runtime -", meta=(TitleProperty="Priority"))
	TArray<FNipcatComboTransitionPriorityMap> CombinedTransitionPriorityMaps;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="- Runtime -", meta=(ForceInlineRow))
	TArray<FNipcatComboDefinitionHandle> ComboDefinitionHandles;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="- Runtime -", meta=(ForceInlineRow))
	bool AbilityEndedByCombo = false;
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Tag", Categories="Combo"))
	void SetCurrentComboTag(FGameplayTag InComboTag, ENipcatComboOffsetRule ComboOffsetRule = ENipcatComboOffsetRule::Hold, bool ClearComboOffsetIfDefaultCombo = true);

	UFUNCTION(BlueprintPure)
	FGameplayTag GetCurrentComboTag() const;

	UFUNCTION(BlueprintPure)
	FGameplayTag GetComboOffsetTag() const;

	UFUNCTION(BlueprintPure)
	bool IsComboOffsetValid() const;

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="InputEventRedirect"))
	void AddComboGraph(UNipcatComboGraph* InComboGraph,
		UPARAM(meta=(GameplayTagFilter="GameplayEvent.InputAction"))
		const TMap<FGameplayTag, FGameplayTag>& InputEventRedirect);
	
	FORCEINLINE void AddComboGraph(UNipcatComboGraph* InComboGraph)
	{
		AddComboGraph(InComboGraph, TMap<FGameplayTag, FGameplayTag>());
	}
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="InputEventRedirect"))
	void AddComboGraphs(const TArray<UNipcatComboGraph*>& InComboGraphs,
		UPARAM(meta=(GameplayTagFilter="GameplayEvent.InputAction"))
		const TMap<FGameplayTag, FGameplayTag>& InputEventRedirect);
	
	FORCEINLINE void AddComboGraphs(const TArray<UNipcatComboGraph*>& InComboGraphs)
	{
		AddComboGraphs(InComboGraphs, TMap<FGameplayTag, FGameplayTag>());
	}
	
	UFUNCTION(BlueprintCallable)
	void RemoveComboGraph(UNipcatComboGraph* InComboGraph);
	
	UFUNCTION(BlueprintCallable)
	void RemoveComboGraphs(const TArray<UNipcatComboGraph*>& InComboGraphs);
	
	UFUNCTION(BlueprintCallable)
	void RemoveAllComboGraphs();
	
	UFUNCTION(BlueprintCallable)
	void AddComboDefinition(const UNipcatComboDefinitionConfig* ComboDefinitionConfig);
	
	UFUNCTION(BlueprintCallable)
	void AddComboDefinitions(const TArray<UNipcatComboDefinitionConfig*>& ComboDefinitionConfigs);
	
	UFUNCTION(BlueprintCallable)
	void RemoveComboDefinition(const UNipcatComboDefinitionConfig* ComboDefinitionConfig);
	
	UFUNCTION(BlueprintCallable)
	void RemoveComboDefinitions(const TArray<UNipcatComboDefinitionConfig*>& ComboDefinitionConfigs);
	
	UFUNCTION(BlueprintCallable)
	void RemoveAllComboDefinitions();
	
	UFUNCTION(BlueprintCallable)
	int32 FindComboDefinitionHandleIndex(const UNipcatComboDefinitionConfig* ComboDefinitionConfig) const;
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="InputEventTag", Categories="GameplayEvent.InputAction"))
	TArray<FNipcatComboTransition> GetValidComboTransitions(FGameplayTag& InputEventTag);
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Tag", Categories="Combo"))
	bool TryActivateComboAbility(const FNipcatComboTransition& ComboTransition, bool& OutConsumeInput);
	
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

protected:
	UPROPERTY(VisibleAnywhere, Category="- Runtime -", meta=(Categories="Combo"))
	FGameplayTag CurrentComboTag;
	
	UPROPERTY(VisibleAnywhere, Category="- Runtime -", meta=(Categories="Combo"))
	FGameplayTag LastActivatedAbilityComboTag;
	
	UPROPERTY(VisibleAnywhere, Category="- Runtime -", meta=(Categories="Combo"))
	FGameplayTag ComboOffsetTag;

	FGameplayAbilitySpecHandle LastActivatedAbilitySpecHandle;
	
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void InitASC();
	virtual void DeinitASC();
	
	UFUNCTION()
	virtual void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	FDelegateHandle OnAbilityEndedHandle;
	
	UFUNCTION()
	void PreInputEventReceived(const FGameplayTag& MatchingTag, const FGameplayEventData& Payload);

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPreInputEventReceivedDelegate, const FGameplayTag&, MatchingTag, const FGameplayEventData&, Payload);
	UPROPERTY(BlueprintAssignable)
	FPreInputEventReceivedDelegate PreInputEventReceivedDelegate;
	
	UFUNCTION()
	virtual void OnInputEventReceived(FGameplayTag MatchingTag, const FGameplayEventData Payload);
	void OnAbilityEnded(const FAbilityEndedData& AbilityEndedData);
};
