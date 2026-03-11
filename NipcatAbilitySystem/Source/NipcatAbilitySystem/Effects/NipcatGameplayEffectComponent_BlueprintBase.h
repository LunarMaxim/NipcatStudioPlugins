// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"

#include "NipcatGameplayEffectComponent_BlueprintBase.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType, EditInlineNew, CollapseCategories, meta=(ShowWorldContextPin))
class NIPCATABILITYSYSTEM_API UNipcatGameplayEffectComponent_BlueprintBase : public UObject
{
	GENERATED_BODY()
		
public:
	UFUNCTION(BlueprintImplementableEvent)
	void PreGameplayEffectApply(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpecHandle& GESpec) const;
	
	/** 
	 * Can the GameplayEffectSpec apply to the passed-in ASC?  All Components of the GE must return true, or a single one can return false to prohibit the application.
	 * Note: Application and Inhibition are two separate things.  If a GE can apply, we then either Add it (if it has duration/prediction) or Execute it (if it's instant).
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool CanGameplayEffectApply(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpecHandle& GESpec) const;

	/**
	 * Called when a Gameplay Effect is Added to the ActiveGameplayEffectsContainer.  GE's are added to that container when they have duration (or are predicting locally).
	 * Note: This also occurs as a result of replication (e.g. when the server replicates a GE to the owning client -- including the 'duplicate' GE after a prediction).
	 * Return if the effect should remain active, or false to inhibit.  Note: Inhibit does not remove the effect (it remains added but dormant, waiting to uninhibit).
	 */
	UFUNCTION(BlueprintNativeEvent)
	bool OnActiveGameplayEffectAdded(UAbilitySystemComponent* AbilitySystemComponent, const FActiveGameplayEffect& ActiveGE) const;
	
	/** 
	 * Called when a Gameplay Effect is executed.  GE's can only Execute on ROLE_Authority.  GE's only Execute when they're applying an instant effect (otherwise they're added to the ActiveGameplayEffectsContainer).
	 * Note: Periodic effects Execute every period (and are also added to ActiveGameplayEffectsContainer).  One may think of this as periodically executing an instant effect (and thus can only happen on the server).
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameplayEffectExecuted(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpecHandle& GESpec) const;
	
	/**
	 * Called when a Gameplay Effect is initially applied, or stacked.  GE's are 'applied' in both cases of duration or instant execution.  This call does not happen periodically, nor through replication.
	 * One should favor this function over OnActiveGameplayEffectAdded & OnGameplayEffectExecuted (but all multiple may be used depending on the case).
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameplayEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpecHandle& GESpec) const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnGameplayEffectStackChange(FActiveGameplayEffectHandle Handle, int32 NewCount, int32 OldCount);

	UFUNCTION(BlueprintImplementableEvent)
	void OnActiveGameplayEffectRemoved(const FGameplayEffectRemovalInfo& RemovalInfo, UAbilitySystemComponent* AbilitySystemComponent) const;

	virtual UWorld* GetWorld() const override;
};
