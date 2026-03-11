// Copyright Nipcat Studio All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Animation/AnimInstance.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "NipcatAbilitySystem/EffectContext/NipcatPlayerMontageParameter.h"
#include "NipcatAbilityTask_PlayMontageAndWait.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNipcatMontageWaitSimpleDelegate, UNipcatAbilityTask_PlayMontageAndWait*, TaskThatTriggeredDelegate);
/** Ability task to simply play a montage. Many games will want to make a modified version of this task that looks for game-specific events */
UCLASS()
class NIPCATABILITYSYSTEM_API UNipcatAbilityTask_PlayMontageAndWait : public UAbilityTask
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FNipcatMontageWaitSimpleDelegate	OnCompleted;

	UPROPERTY(BlueprintAssignable)
	FNipcatMontageWaitSimpleDelegate	OnBlendOut;

	UPROPERTY(BlueprintAssignable)
	FNipcatMontageWaitSimpleDelegate	OnInterrupted;

	UPROPERTY(BlueprintAssignable)
	FNipcatMontageWaitSimpleDelegate	OnSelfInterrupted;

	UPROPERTY(BlueprintAssignable)
	FNipcatMontageWaitSimpleDelegate	OnCancelled;

	UPROPERTY(BlueprintAssignable)
	FNipcatMontageWaitSimpleDelegate	OnSectionChanged;

	UFUNCTION()
	void OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted);

	/** Callback function for when the owning Gameplay Ability is cancelled */
	UFUNCTION()
	void OnGameplayAbilityCancelled();

	UFUNCTION()
	void OnMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnMontageSectionChanged(UAnimMontage* Montage, FName SectionName, bool bLooped);

	/** 
	 * Start playing an animation montage on the avatar actor and wait for it to finish
	 * If StopWhenAbilityEnds is true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled.
	 * On normal execution, OnBlendOut is called when the montage is blending out, and OnCompleted when it is completely done playing
	 * OnInterrupted is called if another montage overwrites this, and OnCancelled is called if the ability or task is cancelled
	 *
	 * @param TaskInstanceName Set to override the name of this task, for later querying
	 * @param MontageToPlay The montage to play on the character
	 * @param Rate Change to play the montage faster or slower
	 * @param StartSection If not empty, named montage section to start from
	 * @param bStopWhenAbilityEnds If true, this montage will be aborted if the ability ends normally. It is always stopped when the ability is explicitly cancelled
	 * @param AnimRootMotionTranslationScale Change to modify size of root motion or set to 0 to block it entirely
	 * @param StartTimeSeconds Starting time offset in montage, this will be overridden by StartSection if that is also set
	 * @param bAllowInterruptAfterBlendOut If true, you can receive OnInterrupted after an OnBlendOut started (otherwise OnInterrupted will not fire when interrupted, but you will not get OnComplete).
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName="NipcatPlayMontageAndWait",
		HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE", AutoCreateRefTerm="MontagePlaySetting"))
	static UNipcatAbilityTask_PlayMontageAndWait* CreateNipcatPlayMontageAndWaitProxy(UGameplayAbility* OwningAbility,
		FName TaskInstanceName, UPARAM(Ref)FNipcatMontagePlaySetting MontagePlaySetting, int32 DefaultInputBlockLevel = 0);

	virtual void Activate() override;

	/** Called when the ability is asked to cancel from an outside node. What this means depends on the individual task. By default, this does nothing other than ending the task. */
	virtual void ExternalCancel() override;

	virtual FString GetDebugString() const override;

protected:

	virtual void OnDestroy(bool AbilityEnded) override;

	/** Checks if the ability is playing a montage and stops that montage, returns true if a montage was stopped, false if not. */
	bool StopPlayingMontage(bool bInterrupted);

	FOnMontageBlendingOutStarted BlendingOutDelegate;
	FOnMontageEnded MontageEndedDelegate;
	FDelegateHandle InterruptedHandle;
	FOnMontageSectionChanged SectionChangedDelegate;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UAnimMontage> MontageToPlay;

	UPROPERTY(BlueprintReadOnly)
	float Rate;

	UPROPERTY(BlueprintReadOnly)
	FName StartSection;

	UPROPERTY(BlueprintReadOnly)
	float AnimRootMotionTranslationScale;

	UPROPERTY(BlueprintReadOnly)
	float StartTimeSeconds;

	UPROPERTY(BlueprintReadOnly)
	bool bStopWhenAbilityEnds;

	UPROPERTY(BlueprintReadOnly)
	FName PrevSectionName;

	UPROPERTY(BlueprintReadOnly)
	float PrevSectionStartTime;

	UPROPERTY(BlueprintReadOnly)
	float PrevSectionEndTime;

	UPROPERTY(BlueprintReadOnly)
	FName ChangedSectionName;

	UPROPERTY(BlueprintReadOnly)
	float ChangedSectionStartTime;

	UPROPERTY(BlueprintReadOnly)
	float ChangedSectionEndTime;

	UPROPERTY(BlueprintReadOnly)
	bool ChangedSectionLooped;

	UPROPERTY()
	bool bAllowInterruptAfterBlendOut;
	
	UPROPERTY(BlueprintReadOnly)
	FNipcatMontagePlaySetting MontagePlaySetting;

	UPROPERTY(BlueprintReadOnly)
	int32 BlockLevel = 0;
};
