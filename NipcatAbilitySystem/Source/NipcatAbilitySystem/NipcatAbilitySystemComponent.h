// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "Effects/GameplayEffectAdjustment.h"

#include "NipcatAbilitySystemComponent.generated.h"


UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent))
class NIPCATABILITYSYSTEM_API UNipcatAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UNipcatAbilitySystemComponent();

protected:
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	//蒙太奇记录,会记录同时满足
	UPROPERTY()
	TArray<FGameplayAbilityLocalAnimMontage> AbilityMontages;

public:
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAnimMontage> NextAnimatingAbilityMontage;
	
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<UGameplayAbility> NextAnimatingAbility;
	
	UPROPERTY(VisibleAnywhere)
	TWeakObjectPtr<UGameplayAbility> PreActivatingAbility;

	UFUNCTION(BlueprintPure)
	UGameplayAbility* GetPreActivatingAbility() const;
	
	virtual void BeginPlay() override;

	virtual FActiveGameplayEffectHandle ApplyGameplayEffectSpecToSelf(const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey) override;

#pragma region 输入处理
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	/**
	 * 输入处理，应该在PlayerController的PostProcessInput方法中调用
	 * @param DeltaTime 
	 * @param bGamePaused 
	 */
	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
#pragma endregion

	UGameplayAbility* FindMontageAbility(UAnimMontage* Montage);

	virtual float PlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName, float StartTimeSeconds) override;

	virtual void NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability) override;

	virtual void ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags, UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags, bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags) override;
	
	virtual void NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled) override;

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameplayEventReceived, FGameplayTag, EventTag, const FGameplayEventData, Payload);

	UPROPERTY(BlueprintAssignable, Category="Abilities")
	FOnGameplayEventReceived OnGameplayEventReceived;

	virtual int32 HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload) override;

	virtual void OnTagUpdated(const FGameplayTag& Tag, bool TagExists) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FGameplayTagContainer OwnedGameplayTags;

#pragma region EffectAdjustment
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;
	
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, FActiveEffectAdjustmentHandle> SourceActiveEffectAdjustmentContainer;
	
	UPROPERTY()
	TMap<FActiveGameplayEffectHandle, FActiveEffectAdjustmentHandle> TargetActiveEffectAdjustmentContainer;
	
	FDelegateHandle EffectAppliedToSelfHandle;
	FDelegateHandle EffectRemovedFromSelfHandle;
	
	UFUNCTION()
	void HandleGameplayEffectAppliedToSelf(UAbilitySystemComponent* Source, const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle);

	UFUNCTION()
	void HandleGameplayEffectRemovedFromSelf(const FActiveGameplayEffect& ActiveEffect);

	// 遍历角色身上的Effect Adjustments，然后将修改的属性写入字典AdjustmentData中
	static void TryApplyGameplayEffectAdjustment(TMap<FGameplayTag, float>& AdjustmentData,
		FGameplayEffectSpec* EffectSpec, UNipcatAbilitySystemComponent* SourceASC,
		UNipcatAbilitySystemComponent* TargetASC);
#pragma endregion 

};
