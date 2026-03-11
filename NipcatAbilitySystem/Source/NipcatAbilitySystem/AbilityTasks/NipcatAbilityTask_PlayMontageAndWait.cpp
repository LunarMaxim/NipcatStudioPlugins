// Copyright Nipcat Studio All Rights Reserved.

#include "NipcatAbilityTask_PlayMontageAndWait.h"
#include "Animation/AnimMontage.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemLog.h"
#include "AbilitySystemGlobals.h"
#include "NipcatAbilitySystem/NipcatAbilitySystemComponent.h"
#include "NipcatAbilitySystem/Abilities/NipcatGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(NipcatAbilityTask_PlayMontageAndWait)

static bool GUseAggressivePlayMontageAndWaitEndTask = false;
static FAutoConsoleVariableRef CVarAggressivePlayMontageAndWaitEndTask(TEXT("AbilitySystem.PlayMontage.AggressiveEndTask"), GUseAggressivePlayMontageAndWaitEndTask, TEXT("This should be set to true in order to avoid multiple callbacks off an NipcatAbilityTask_PlayMontageAndWait node"));

void UNipcatAbilityTask_PlayMontageAndWait::OnMontageBlendingOut(UAnimMontage* Montage, bool bInterrupted)
{
 	const bool bPlayingThisMontage = (Montage == MontageToPlay) && Ability && Ability->GetCurrentMontage() == MontageToPlay;
	if (bPlayingThisMontage)
	{
		// Reset AnimRootMotionTranslationScale
		ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
		if (Character && (Character->GetLocalRole() == ROLE_Authority ||
							(Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
		{
			Character->SetAnimRootMotionTranslationScale(1.f);
		}
	}

	if (bPlayingThisMontage && (bInterrupted || !bAllowInterruptAfterBlendOut))
	{
		if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
		{
			// ASC->ClearAnimatingAbility(Ability);
		}
	}

	if (ShouldBroadcastAbilityTaskDelegates())
	{
		if (bInterrupted)
		{
			bool bSelfInterrupted = false;
			if (AbilitySystemComponent.Get())
			{
				if (const auto NipcatASC = Cast<UNipcatAbilitySystemComponent>(AbilitySystemComponent.Get()))
				{
					bSelfInterrupted = NipcatASC->NextAnimatingAbility == GetTaskOwner()->_getUObject();
				}	
			}
			if (bSelfInterrupted)
			{
				OnSelfInterrupted.Broadcast(this);
			}
			else
			{
				OnInterrupted.Broadcast(this);
			}

			if (GUseAggressivePlayMontageAndWaitEndTask)
			{
				EndTask();
			}
		}
		else
		{
			OnBlendOut.Broadcast(this);
		}
	}
}

void UNipcatAbilityTask_PlayMontageAndWait::OnGameplayAbilityCancelled()
{
	if (bAllowInterruptAfterBlendOut || (bStopWhenAbilityEnds && StopPlayingMontage(true)))
	{
		// Let the BP handle the interrupt as well
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnInterrupted.Broadcast(this);
		}
	}

	if (GUseAggressivePlayMontageAndWaitEndTask)
	{
		EndTask();
	}
}

void UNipcatAbilityTask_PlayMontageAndWait::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	if (!bInterrupted)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCompleted.Broadcast(this);
		}
	}

	EndTask();
}

void UNipcatAbilityTask_PlayMontageAndWait::OnMontageSectionChanged(UAnimMontage* Montage, FName SectionName,
	bool bLooped)
{
	PrevSectionName = ChangedSectionName;
	PrevSectionStartTime = ChangedSectionStartTime;
	PrevSectionEndTime = ChangedSectionEndTime;
	
	Montage->GetSectionStartAndEndTime(Montage->GetSectionIndex(SectionName), ChangedSectionStartTime, ChangedSectionEndTime);
	ChangedSectionName = SectionName;
	ChangedSectionLooped = bLooped;
	OnSectionChanged.Broadcast(this);
}

UNipcatAbilityTask_PlayMontageAndWait* UNipcatAbilityTask_PlayMontageAndWait::CreateNipcatPlayMontageAndWaitProxy(UGameplayAbility* OwningAbility,
                                                                                                                  FName TaskInstanceName, FNipcatMontagePlaySetting MontagePlaySetting, int32 DefaultInputBlockLevel)
{
	if (!OwningAbility->IsActive())
	{
		return nullptr;
	}
	UAbilitySystemGlobals::NonShipping_ApplyGlobalAbilityScaler_Rate(MontagePlaySetting.PlayRate);

	const auto& MontageRef = MontagePlaySetting.MontageToPlay;
	if (!MontageRef)
	{
		return nullptr;
	}
	
	UNipcatAbilityTask_PlayMontageAndWait* MyObj = NewAbilityTask<UNipcatAbilityTask_PlayMontageAndWait>(OwningAbility, TaskInstanceName);
	MyObj->MontagePlaySetting = MontagePlaySetting;
	MyObj->MontageToPlay = MontageRef;
	MyObj->Rate = MontagePlaySetting.PlayRate;
	MyObj->StartSection = MontagePlaySetting.StartSection;
	MyObj->StartTimeSeconds = MontagePlaySetting.StartTimeSeconds;
	MyObj->PrevSectionName = MontagePlaySetting.StartSection;
	if (MyObj->PrevSectionName == NAME_None)
	{
		MyObj->PrevSectionName = MontageRef->GetSectionName(MontageRef->GetSectionIndexFromPosition(MyObj->StartTimeSeconds));
	}
	MontageRef->GetSectionStartAndEndTime(MontageRef->GetSectionIndex(MyObj->PrevSectionName), MyObj->PrevSectionStartTime, MyObj->PrevSectionEndTime);
	MyObj->ChangedSectionName = MyObj->PrevSectionName;
	MyObj->ChangedSectionStartTime = MyObj->PrevSectionStartTime;
	MyObj->ChangedSectionEndTime = MyObj->PrevSectionEndTime;
	MyObj->AnimRootMotionTranslationScale = MontagePlaySetting.AnimRootMotionTranslationScale;
	MyObj->bStopWhenAbilityEnds = MontagePlaySetting.StopWhenAbilityEnds;
	MyObj->bAllowInterruptAfterBlendOut = true;
	if (MontagePlaySetting.bOverrideDefaultInputBlockLevel)
	{
		MyObj->BlockLevel = MontagePlaySetting.InputBlockLevelOverride;
	}
	else
	{
		MyObj->BlockLevel = DefaultInputBlockLevel;
	}
	
	return MyObj;
}

void UNipcatAbilityTask_PlayMontageAndWait::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}

	bool bPlayedMontage = false;

	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			if (ASC->PlayMontage(Ability, Ability->GetCurrentActivationInfo(), MontageToPlay, Rate, StartSection, StartTimeSeconds) > 0.f)
			{
				// Playing a montage could potentially fire off a callback into game code which could kill this ability! Early out if we are  pending kill.
				if (ShouldBroadcastAbilityTaskDelegates() == false)
				{
					return;
				}

				InterruptedHandle = Ability->OnGameplayAbilityCancelled.AddUObject(this, &UNipcatAbilityTask_PlayMontageAndWait::OnGameplayAbilityCancelled);

				BlendingOutDelegate.BindUObject(this, &UNipcatAbilityTask_PlayMontageAndWait::OnMontageBlendingOut);
				AnimInstance->Montage_SetBlendingOutDelegate(BlendingOutDelegate, MontageToPlay);

				MontageEndedDelegate.BindUObject(this, &UNipcatAbilityTask_PlayMontageAndWait::OnMontageEnded);
				AnimInstance->Montage_SetEndDelegate(MontageEndedDelegate, MontageToPlay);

				SectionChangedDelegate.BindUObject(this, &ThisClass::OnMontageSectionChanged);
				AnimInstance->Montage_SetSectionChangedDelegate(SectionChangedDelegate, MontageToPlay);
				
				ACharacter* Character = Cast<ACharacter>(GetAvatarActor());
				if (Character && (Character->GetLocalRole() == ROLE_Authority ||
								  (Character->GetLocalRole() == ROLE_AutonomousProxy && Ability->GetNetExecutionPolicy() == EGameplayAbilityNetExecutionPolicy::LocalPredicted)))
				{
					Character->SetAnimRootMotionTranslationScale(AnimRootMotionTranslationScale);
				}

				bPlayedMontage = true;
			}
		}
		else
		{
			ABILITY_LOG(Warning, TEXT("UNipcatAbilityTask_PlayMontageAndWait call to PlayMontage failed!"));
		}
	}
	else
	{
		ABILITY_LOG(Warning, TEXT("UNipcatAbilityTask_PlayMontageAndWait called on invalid AbilitySystemComponent"));
	}

	if (!bPlayedMontage)
	{
		ABILITY_LOG(Warning, TEXT("UNipcatAbilityTask_PlayMontageAndWait called in Ability %s failed to play montage %s; Task Instance Name %s."), *Ability->GetName(), *GetNameSafe(MontageToPlay),*InstanceName.ToString());
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnCancelled.Broadcast(this);
		}
	}

	SetWaitingOnAvatar();
}

void UNipcatAbilityTask_PlayMontageAndWait::ExternalCancel()
{
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		OnCancelled.Broadcast(this);
	}
	Super::ExternalCancel();
}

void UNipcatAbilityTask_PlayMontageAndWait::OnDestroy(bool AbilityEnded)
{
	// Note: Clearing montage end delegate isn't necessary since its not a multicast and will be cleared when the next montage plays.
	// (If we are destroyed, it will detect this and not do anything)

	// This delegate, however, should be cleared as it is a multicast
	if (Ability)
	{
		Ability->OnGameplayAbilityCancelled.Remove(InterruptedHandle);
		if (AbilityEnded && bStopWhenAbilityEnds)
		{
			StopPlayingMontage(false);
		}
	}

	Super::OnDestroy(AbilityEnded);

}

// GA被Cancel或Node Destroy时触发
bool UNipcatAbilityTask_PlayMontageAndWait::StopPlayingMontage(bool bInterrupted)
{
	if (Ability == nullptr)
	{
		return false;
	}

	const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
	if (ActorInfo == nullptr)
	{
		return false;
	}

	UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();
	if (AnimInstance == nullptr)
	{
		return false;
	}

	if (const auto ASC = Cast<UNipcatAbilitySystemComponent>(AbilitySystemComponent))
	{
		if (ASC->PreActivatingAbility.IsValid())
		{
			if (const auto ActivatingAbility = Cast<UNipcatGameplayAbility>(ASC->PreActivatingAbility.Get()))
			{
				if (!ActivatingAbility->MontagesToPlay.IsEmpty())
				{
					// Cancelled By Montage, Don't Stop
					return false;
				}
			}
		}
	}

	// Check if the montage is still playing
	// The ability would have been interrupted, in which case we should automatically stop the montage
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (ASC && Ability)
	{
		if (ASC->GetAnimatingAbility() == Ability
			&& ASC->GetCurrentMontage() == MontageToPlay)
		{
			// Unbind delegates so they don't get called as well
			FAnimMontageInstance* MontageInstance = AnimInstance->GetActiveInstanceForMontage(MontageToPlay);
			if (MontageInstance)
			{
				if (!bInterrupted)
				{
					OnBlendOut.Broadcast(this);
				}
				MontageInstance->OnMontageBlendingOutStarted.Unbind();
				MontageInstance->OnMontageEnded.Unbind();
			}

			ASC->CurrentMontageStop();
			return true;
		}
	}

	return false;
}

FString UNipcatAbilityTask_PlayMontageAndWait::GetDebugString() const
{
	UAnimMontage* PlayingMontage = nullptr;
	if (Ability)
	{
		const FGameplayAbilityActorInfo* ActorInfo = Ability->GetCurrentActorInfo();
		UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance();

		if (AnimInstance != nullptr)
		{
			PlayingMontage = AnimInstance->Montage_IsActive(MontageToPlay) ? ToRawPtr(MontageToPlay) : AnimInstance->GetCurrentActiveMontage();
		}
	}

	return FString::Printf(TEXT("PlayMontageAndWait. MontageToPlay: %s  (Currently Playing): %s"), *GetNameSafe(MontageToPlay), *GetNameSafe(PlayingMontage));
}

