// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAbilitySystemComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemLog.h"
#include "GameplayCueManager.h"
#include "AnimNotify/NipcatMontageNotifyAbilityInterface.h"
#include "AnimNotify/AnimNotifyState_AbilityMontage.h"
#include "EffectContext/NipcatGameplayEffectContext.h"
#include "Effects/NipcatGameplayEffect.h"
#include "Interface\NipcatAbilitySystemComponentOwnerInterface.h"
#include "Misc/EngineVersionComparison.h"


UNipcatAbilitySystemComponent::UNipcatAbilitySystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


UGameplayAbility* UNipcatAbilitySystemComponent::GetPreActivatingAbility() const
{
	return PreActivatingAbility.Get();
}

void UNipcatAbilitySystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// TODO 应该移到InitializeComponent中，因为Equipment上的Ability会先Give，导致无法正确初始化武器相关属性
	// TODO 但移到InitializeComponent后，ViewModel无法绑定属性
	
	TScriptInterface<INipcatAbilitySystemComponentOwnerInterface> AbilitySystemHolderInterface;
	AbilitySystemHolderInterface.SetObject(GetOwner());
	AbilitySystemHolderInterface.SetInterface(Cast<INipcatAbilitySystemComponentOwnerInterface>(GetOwner()));
	UObject* AbilitySystemHolderInterfaceObject = AbilitySystemHolderInterface.GetObject();
	if (!GetOwner()->GetClass()->ImplementsInterface(UNipcatAbilitySystemComponentOwnerInterface::StaticClass()))
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("Error: Character %s missing IAbilitySystemHolderInterface"), *GetOwner()->GetName());
		return;
	}

	UObject* ActorConfigInterface = INipcatAbilitySystemComponentOwnerInterface::Execute_GetAbilitySystemActorConfig(AbilitySystemHolderInterface.GetObject()).GetObject();

	if (!ActorConfigInterface)
	{
		UE_LOG(LogAbilitySystemComponent, Error, TEXT("Error: Character %s missing IAbilitySystemActorConfigInterface"), *GetOwner()->GetName());
		return;
	}
	
	TArray<UNipcatAbilitySet*> DefaultAbilitySets = INipcatAbilitySystemActorConfigInterface::Execute_GetDefaultAbilitySets(ActorConfigInterface);
	TMap<TSubclassOf<UGameplayAbility>, float> DefaultAbilitiesToGrant = INipcatAbilitySystemActorConfigInterface::Execute_GetDefaultGameplayAbilitiesToGrant(ActorConfigInterface);
	TMap<TSubclassOf<UGameplayAbility>, float> DefaultAbilitiesToActivate = INipcatAbilitySystemActorConfigInterface::Execute_GetDefaultGameplayAbilitiesToActivate(ActorConfigInterface);
	TArray<TSubclassOf<UAttributeSet>> DefaultAttributeSets = INipcatAbilitySystemActorConfigInterface::Execute_GetDefaultAttributeSets(ActorConfigInterface);
	FName AttributeGroupName = INipcatAbilitySystemActorConfigInterface::Execute_GetAttributeGroupName(ActorConfigInterface);
	TMap<TSubclassOf<UGameplayEffect>, float> DefaultGameplayEffects = INipcatAbilitySystemActorConfigInterface::Execute_GetDefaultGameplayEffects(ActorConfigInterface);
	FGameplayTagContainer DefaultGameplayTags = INipcatAbilitySystemActorConfigInterface::Execute_GetDefaultGameplayTags(ActorConfigInterface);

	for (UNipcatAbilitySet* AbilitySet : DefaultAbilitySets)
	{
		if (AbilitySet)
		{
			AbilitySet->GiveToAbilitySystem(this);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Warning: Character %s has invalid Ability Set"), *GetOwner()->GetName())
		}
	}

	for (const auto& Pair : DefaultAbilitiesToGrant)
	{
		K2_GiveAbility(Pair.Key, Pair.Value);
	}
	
	for (const auto& Pair : DefaultAbilitiesToActivate)
	{
		K2_GiveAbilityAndActivateOnce(Pair.Key, Pair.Value);
	}

	for (const auto& AttributeSet : DefaultAttributeSets)
	{
		UAttributeSet* NewSet = NewObject<UAttributeSet>(GetOwner(), AttributeSet);
		AddAttributeSetSubobject(NewSet);
	}

	IGameplayAbilitiesModule::Get().GetAbilitySystemGlobals()->GetAttributeSetInitter()->InitAttributeSetDefaults(this, AttributeGroupName, 1, true);
	
	for (const auto& Pair : DefaultGameplayEffects)
	{
		BP_ApplyGameplayEffectToSelf(Pair.Key, Pair.Value, MakeEffectContext());
	}

#if UE_VERSION_OLDER_THAN(5,7,0)
	AddLooseGameplayTags(DefaultGameplayTags);
	AddReplicatedLooseGameplayTags(DefaultGameplayTags);
#else
	AddLooseGameplayTags(DefaultGameplayTags, 1, EGameplayTagReplicationState::CountToOwner);
#endif
	
}

FActiveGameplayEffectHandle UNipcatAbilitySystemComponent::ApplyGameplayEffectSpecToSelf(
	const FGameplayEffectSpec& Spec, FPredictionKey PredictionKey)
{
	// Scope lock the container after the addition has taken place to prevent the new effect from potentially getting mangled during the remainder
	// of the add operation
	FScopedActiveGameplayEffectLock ScopeLock(ActiveGameplayEffects);

	FScopeCurrentGameplayEffectBeingApplied ScopedGEApplication(&Spec, this);

	const bool bIsNetAuthority = IsOwnerActorAuthoritative();

	// Check Network Authority
	if (!HasNetworkAuthorityToApplyGameplayEffect(PredictionKey))
	{
		return FActiveGameplayEffectHandle();
	}

	// Don't allow prediction of periodic effects
	if (PredictionKey.IsValidKey() && Spec.GetPeriod() > 0.f)
	{
		if (IsOwnerActorAuthoritative())
		{
			// Server continue with invalid prediction key
			PredictionKey = FPredictionKey();
		}
		else
		{
			// Client just return now
			return FActiveGameplayEffectHandle();
		}
	}
	
	/**************************  PreGameplayEffectApply *****************************************/
	if (const UNipcatGameplayEffect* NipcatGameplayEffect = Cast<UNipcatGameplayEffect>(Spec.Def))
	{
		NipcatGameplayEffect->PreGameplayEffectApply(ActiveGameplayEffects, Spec, PredictionKey);
	}

		// Check if there is a registered "application" query that can block the application
	for (const FGameplayEffectApplicationQuery& ApplicationQuery : GameplayEffectApplicationQueries)
	{
		const bool bAllowed = ApplicationQuery.Execute(ActiveGameplayEffects, Spec);
		if (!bAllowed)
		{
			return FActiveGameplayEffectHandle();
		}
	}

	// check if the effect being applied actually succeeds
	if (!Spec.Def->CanApply(ActiveGameplayEffects, Spec))
	{
		return FActiveGameplayEffectHandle();
	}

	// Check AttributeSet requirements: make sure all attributes are valid
	// We may want to cache this off in some way to make the runtime check quicker.
	// We also need to handle things in the execution list
	for (const FGameplayModifierInfo& Mod : Spec.Def->Modifiers)
	{
		if (!Mod.Attribute.IsValid())
		{
			ABILITY_LOG(Warning, TEXT("%s has a null modifier attribute."), *Spec.Def->GetPathName());
			return FActiveGameplayEffectHandle();
		}
	}


	// Clients should treat predicted instant effects as if they have infinite duration. The effects will be cleaned up later.
	bool bTreatAsInfiniteDuration = GetOwnerRole() != ROLE_Authority && PredictionKey.IsLocalClientKey() && Spec.Def->DurationPolicy == EGameplayEffectDurationType::Instant;

	// Make sure we create our copy of the spec in the right place
	// We initialize the FActiveGameplayEffectHandle here with INDEX_NONE to handle the case of instant GE
	// Initializing it like this will set the bPassedFiltersAndWasExecuted on the FActiveGameplayEffectHandle to true so we can know that we applied a GE
	FActiveGameplayEffectHandle	MyHandle(INDEX_NONE);
	bool bInvokeGameplayCueApplied = Spec.Def->DurationPolicy != EGameplayEffectDurationType::Instant; // Cache this now before possibly modifying predictive instant effect to infinite duration effect.
	bool bFoundExistingStackableGE = false;

	FActiveGameplayEffect* AppliedEffect = nullptr;
	FGameplayEffectSpec* OurCopyOfSpec = nullptr;
	TUniquePtr<FGameplayEffectSpec> StackSpec;
	{
		if (Spec.Def->DurationPolicy != EGameplayEffectDurationType::Instant || bTreatAsInfiniteDuration)
		{
			AppliedEffect = ActiveGameplayEffects.ApplyGameplayEffectSpec(Spec, PredictionKey, bFoundExistingStackableGE);
			if (!AppliedEffect)
			{
				return FActiveGameplayEffectHandle();
			}

			MyHandle = AppliedEffect->Handle;
			OurCopyOfSpec = &(AppliedEffect->Spec);

			// Log results of applied GE spec
			if (UE_LOG_ACTIVE(VLogAbilitySystem, Log))
			{
				UE_VLOG(GetOwnerActor(), VLogAbilitySystem, Log, TEXT("Applied %s"), *OurCopyOfSpec->Def->GetFName().ToString());

				for (const FGameplayModifierInfo& Modifier : Spec.Def->Modifiers)
				{
					float Magnitude = 0.f;
					Modifier.ModifierMagnitude.AttemptCalculateMagnitude(Spec, Magnitude);
					UE_VLOG(GetOwnerActor(), VLogAbilitySystem, Log, TEXT("         %s: %s %f"), *Modifier.Attribute.GetName(), *EGameplayModOpToString(Modifier.ModifierOp), Magnitude);
				}
			}
		}

		if (!OurCopyOfSpec)
		{
			StackSpec = MakeUnique<FGameplayEffectSpec>(Spec);
			OurCopyOfSpec = StackSpec.Get();

			UAbilitySystemGlobals::Get().GlobalPreGameplayEffectSpecApply(*OurCopyOfSpec, this);
			OurCopyOfSpec->CaptureAttributeDataFromTarget(this);
		}

		// if necessary add a modifier to OurCopyOfSpec to force it to have an infinite duration
		if (bTreatAsInfiniteDuration)
		{
			// This should just be a straight set of the duration float now
			OurCopyOfSpec->SetDuration(UGameplayEffect::INFINITE_DURATION, true);
		}
	}

	// Update (not push) the global spec being applied [we want to switch it to our copy, from the const input copy)
	UAbilitySystemGlobals::Get().SetCurrentAppliedGE(OurCopyOfSpec);

	// UE5.4: We are following the same previous implementation that there is a special case for Gameplay Cues here (caveat: may not be true):
	// We are Stacking an existing Gameplay Effect.  That means the GameplayCues should already be Added/WhileActive and we do not have a proper
	// way to replicate the fact that it's been retriggered, hence the RPC here.  I say this may not be true because any number of things could have
	// removed the GameplayCue by the time we getting a Stacking GE (e.g. RemoveGameplayCue).
	if (!bSuppressGameplayCues && !Spec.Def->bSuppressStackingCues && bFoundExistingStackableGE && AppliedEffect && !AppliedEffect->bIsInhibited)
	{
		ensureMsgf(OurCopyOfSpec, TEXT("OurCopyOfSpec will always be valid if bFoundExistingStackableGE"));
		if (OurCopyOfSpec && OurCopyOfSpec->GetStackCount() > Spec.GetStackCount())
		{
			// Because PostReplicatedChange will get called from modifying the stack count
			// (and not PostReplicatedAdd) we won't know which GE was modified.
			// So instead we need to explicitly RPC the client so it knows the GC needs updating
			UAbilitySystemGlobals::Get().GetGameplayCueManager()->InvokeGameplayCueAddedAndWhileActive_FromSpec(this, *OurCopyOfSpec, PredictionKey);
		}
	}
	
	// Execute the GE at least once (if instant, this will execute once and be done. If persistent, it was added to ActiveGameplayEffects in ApplyGameplayEffectSpec)
	
	// Execute if this is an instant application effect
	if (bTreatAsInfiniteDuration)
	{
		// This is an instant application but we are treating it as an infinite duration for prediction. We should still predict the execute GameplayCUE.
		// (in non predictive case, this will happen inside ::ExecuteGameplayEffect)

		if (!bSuppressGameplayCues)
		{
			UAbilitySystemGlobals::Get().GetGameplayCueManager()->InvokeGameplayCueExecuted_FromSpec(this, *OurCopyOfSpec, PredictionKey);
		}
	}
	else if (Spec.Def->DurationPolicy == EGameplayEffectDurationType::Instant)
	{
		// This is a non-predicted instant effect (it never gets added to ActiveGameplayEffects)
		ExecuteGameplayEffect(*OurCopyOfSpec, PredictionKey);
	}

	// Notify the Gameplay Effect (and its Components) that it has been successfully applied
	Spec.Def->OnApplied(ActiveGameplayEffects, *OurCopyOfSpec, PredictionKey);

	UAbilitySystemComponent* InstigatorASC = Spec.GetContext().GetInstigatorAbilitySystemComponent();

	// Send ourselves a callback	
	OnGameplayEffectAppliedToSelf(InstigatorASC, *OurCopyOfSpec, MyHandle);

	// Send the instigator a callback
	if (InstigatorASC)
	{
		InstigatorASC->OnGameplayEffectAppliedToTarget(this, *OurCopyOfSpec, MyHandle);
	}

	return MyHandle;
}


#pragma region 输入处理
void UNipcatAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				// InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void UNipcatAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				// InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void UNipcatAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{
	/*if (HasMatchingGameplayTag(TAG_Gameplay_AbilityInputBlocked))
	{
		ClearAbilityInput();
		return;
	}*/

	static TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate;
	AbilitiesToActivate.Reset();

	//@TODO: See if we can use FScopedServerAbilityRPCBatcher ScopedRPCBatcher in some of these loops

	//
	// Process all abilities that activate when the input is held.
	//
	/*for (const FGameplayAbilitySpecHandle& SpecHandle : InputHeldSpecHandles)
	{
		if (const FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability && !AbilitySpec->IsActive())
			{
				if (AbilitySpec->Ability.GetClass()->IsChildOf(UCommonGameplayAbility::StaticClass()))
				{
					const UCommonGameplayAbility* AbilityCDO = CastChecked<UCommonGameplayAbility>(AbilitySpec->Ability);

					if (AbilityCDO && AbilityCDO->GetActivationPolicy() == ECommonAbilityActivationPolicy::WhileInputActive)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
				}
				else
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}*/

	//
	// Process all abilities that had their input pressed this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;
				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
				}
			}
		}
	}

	//
	// Try to activate all the abilities that are from presses and holds.
	// We do it all at once so that held inputs don't activate the ability
	// and then also send a input event to the ability because of the press.
	//
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		TryActivateAbility(AbilitySpecHandle);
	}

	//
	// Process all abilities that had their input released this frame.
	//
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	//
	// Clear the cached ability handles.
	//
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

UGameplayAbility* UNipcatAbilitySystemComponent::FindMontageAbility(UAnimMontage* Montage)
{
	if (Montage)
	{
		for (FGameplayAbilityLocalAnimMontage& AbilityMontage : AbilityMontages)
		{
			if (AbilityMontage.AnimMontage == Montage)
			{
				return AbilityMontage.AnimatingAbility.Get();
			}
		}
	}
	return nullptr;
}

float UNipcatAbilitySystemComponent::PlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName, float StartTimeSeconds)
{
	NextAnimatingAbilityMontage = Montage;
	NextAnimatingAbility = AnimatingAbility;
	float Duration = Super::PlayMontage(AnimatingAbility, ActivationInfo, Montage, InPlayRate, StartSectionName, StartTimeSeconds);
	NextAnimatingAbilityMontage = nullptr;
	NextAnimatingAbility = nullptr;
	/*
	if (Montage && AnimatingAbility && Duration > 0.f)
	{
		if (AnimatingAbility->Implements<UNipcatMontageNotifyAbilityInterface>())
		{
			bool bRecordMontage = false;
			for (FAnimNotifyEvent& NotifyEvent : Montage->Notifies)
			{
				if (NotifyEvent.NotifyStateClass && NotifyEvent.NotifyStateClass.GetClass()->IsChildOf(UAnimNotifyState_AbilityMontage::StaticClass()))
				{
					bRecordMontage = true;
					break;
				}
			}
			if (bRecordMontage)
			{
				// AnimatingAbility->OnGameplayAbilityEnded.RemoveAll(this);
				// AnimatingAbility->OnGameplayAbilityEnded.AddUObject(this, &UNipcatAbilitySystemComponent::OnMontageAbilityEnded);
				for (FGameplayAbilityLocalAnimMontage& MontageInfo : AbilityMontages)
				{
					if (Montage == MontageInfo.AnimMontage)
					{
						MontageInfo.AnimatingAbility = AnimatingAbility;
						MontageInfo.PlayInstanceId = LocalAnimMontageInfo.PlayInstanceId;
						return Duration;
					}
				}
				AbilityMontages.Add(LocalAnimMontageInfo);
			}
		}
	}*/
	return Duration;
}

void UNipcatAbilitySystemComponent::NotifyAbilityActivated(const FGameplayAbilitySpecHandle Handle,
	UGameplayAbility* Ability)
{
	PreActivatingAbility = Ability;
	Super::NotifyAbilityActivated(Handle, Ability);
}

void UNipcatAbilitySystemComponent::ApplyAbilityBlockAndCancelTags(const FGameplayTagContainer& AbilityTags,
	UGameplayAbility* RequestingAbility, bool bEnableBlockTags, const FGameplayTagContainer& BlockTags,
	bool bExecuteCancelTags, const FGameplayTagContainer& CancelTags)
{
	Super::ApplyAbilityBlockAndCancelTags(AbilityTags, RequestingAbility, bEnableBlockTags, BlockTags,
	                                      bExecuteCancelTags, CancelTags);
	if (bExecuteCancelTags)
	{
		PreActivatingAbility.Reset();
	}
}

void UNipcatAbilitySystemComponent::NotifyAbilityEnded(FGameplayAbilitySpecHandle Handle, UGameplayAbility* Ability, bool bWasCancelled)
{
	Super::NotifyAbilityEnded(Handle, Ability, bWasCancelled);
	
	if (auto NipcatAbilityActorInfo = FNipcatGameplayAbilityActorInfo::ExtractActorInfo(AbilityActorInfo.Get()))
	{
		NipcatAbilityActorInfo->MontagePlaySettingOverride.Remove(Ability->GetClass());
	}
	
	for (auto EntryIt = AbilityMontages.CreateIterator(); EntryIt; ++EntryIt)
	{
		FGameplayAbilityLocalAnimMontage& Entry = *EntryIt;
		if (Entry.AnimatingAbility == Ability)
		{
			EntryIt.RemoveCurrent();
		}
	}
}


#pragma endregion


int32 UNipcatAbilitySystemComponent::HandleGameplayEvent(FGameplayTag EventTag, const FGameplayEventData* Payload)
{
	OnGameplayEventReceived.Broadcast(EventTag, *Payload);
	return Super::HandleGameplayEvent(EventTag, Payload);
}

void UNipcatAbilitySystemComponent::OnTagUpdated(const FGameplayTag& Tag, bool TagExists)
{
	OwnedGameplayTags = GetOwnedGameplayTags();
}

#pragma region EffectAdjustment

void UNipcatAbilitySystemComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 解除监听Effect Applied和removed
	if(EffectAppliedToSelfHandle.IsValid())
	{
		OnGameplayEffectAppliedDelegateToSelf.Remove(EffectAppliedToSelfHandle);
	}
	if(EffectRemovedFromSelfHandle.IsValid())
	{
		OnAnyGameplayEffectRemovedDelegate().Remove(EffectRemovedFromSelfHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void UNipcatAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	// 监听Effect Applied，用于注册Effect Adjustment
	if(!EffectAppliedToSelfHandle.IsValid())
	{
		EffectAppliedToSelfHandle = OnActiveGameplayEffectAddedDelegateToSelf.AddUObject(this, &UNipcatAbilitySystemComponent::HandleGameplayEffectAppliedToSelf);
	}
	if(!EffectRemovedFromSelfHandle.IsValid())
	{
		EffectRemovedFromSelfHandle = OnAnyGameplayEffectRemovedDelegate().AddUObject(this, &UNipcatAbilitySystemComponent::HandleGameplayEffectRemovedFromSelf);
	}
}

void UNipcatAbilitySystemComponent::HandleGameplayEffectAppliedToSelf(UAbilitySystemComponent* Source,
	const FGameplayEffectSpec& SpecApplied, FActiveGameplayEffectHandle ActiveHandle)
{
	if(!IsValid(SpecApplied.Def)) return;

	const UNipcatGameplayEffect* EffectCDO = Cast<UNipcatGameplayEffect>(SpecApplied.Def);
	if(!IsValid(EffectCDO)) return;

	if(!EffectCDO->SourceGrantEffectAdjustments.IsEmpty())
	{
		for(UGameplayEffectAdjustment* Adjustment: EffectCDO->SourceGrantEffectAdjustments)
		{
			FActiveEffectAdjustmentHandle AdjustmentHandle;
			AdjustmentHandle.EffectAdjustment = Adjustment;
			SourceActiveEffectAdjustmentContainer.Add(ActiveHandle, AdjustmentHandle);
		}
	}

	if(!EffectCDO->TargetGrantEffectAdjustments.IsEmpty())
	{
		for(UGameplayEffectAdjustment* Adjustment: EffectCDO->TargetGrantEffectAdjustments)
		{
			FActiveEffectAdjustmentHandle AdjustmentHandle;
			AdjustmentHandle.EffectAdjustment = Adjustment;
			TargetActiveEffectAdjustmentContainer.Add(ActiveHandle, AdjustmentHandle);
		}
	}
}

void UNipcatAbilitySystemComponent::HandleGameplayEffectRemovedFromSelf(const FActiveGameplayEffect& ActiveEffect)
{
	const FActiveGameplayEffectHandle& Handle = ActiveEffect.Handle;
	SourceActiveEffectAdjustmentContainer.Remove(Handle);
	TargetActiveEffectAdjustmentContainer.Remove(Handle);
}

void UNipcatAbilitySystemComponent:: TryApplyGameplayEffectAdjustment(TMap<FGameplayTag, float>& AdjustmentData,
	FGameplayEffectSpec* EffectSpec, UNipcatAbilitySystemComponent* SourceASC,
	UNipcatAbilitySystemComponent* TargetASC)
{
	
	for(auto& Pair: SourceASC->SourceActiveEffectAdjustmentContainer)
	{
		FActiveEffectAdjustmentHandle AdjustmentHandle = Pair.Value;
		if(AdjustmentHandle.EffectAdjustment)
		{
			if(AdjustmentHandle.EffectAdjustment->CanApplyEffectAdjustment(EffectSpec->GetContext(), SourceASC, TargetASC))
			{
				for(int i = 0; i < SourceASC->GetActiveGameplayEffect(Pair.Key)->Spec.GetStackCount(); i++)
				{
					AdjustmentHandle.EffectAdjustment->TryApplyEffectAdjustment(AdjustmentData);
				}
			}
		}
	}
	for(auto& Pair: TargetASC->TargetActiveEffectAdjustmentContainer)
	{
		FActiveEffectAdjustmentHandle AdjustmentHandle = Pair.Value;
		if(AdjustmentHandle.EffectAdjustment)
		{
			if(AdjustmentHandle.EffectAdjustment->CanApplyEffectAdjustment(EffectSpec->GetContext(), SourceASC, TargetASC))
			{
				for(int i = 0; i < TargetASC->GetActiveGameplayEffect(Pair.Key)->Spec.GetStackCount(); i++)
				{
					AdjustmentHandle.EffectAdjustment->TryApplyEffectAdjustment(AdjustmentData);
				}
			}
		}
	}
}

#pragma endregion 