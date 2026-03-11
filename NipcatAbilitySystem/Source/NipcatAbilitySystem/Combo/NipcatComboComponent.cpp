// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatComboComponent.h"

#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbilitySpecHandle.h"
#include "Interface/NipcatComboActorConfigInterface.h"
#include "Interface/NipcatComboComponentOwnerInterface.h"
#include "NipcatAbilitySystem/NipcatAbilitySystemComponent.h"
#include "NipcatAbilitySystem/EffectContext/NipcatGameplayEffectContext.h"
#include "NipcatBasicGameplay/Gameplay/NipcatPlayerController.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_GameplayEvent_InputAction, "GameplayEvent.InputAction")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Combo, "Combo")
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Combo_Default, "Combo.Default")

void UNipcatComboComponent::AddComboGraph(UNipcatComboGraph* InComboGraph, const TMap<FGameplayTag, FGameplayTag>& InputEventRedirect)
{
	if (!InComboGraph)
	{
		return;
	}
	if (!ComboGraphs.Contains(InComboGraph))
	{
		ComboGraphs.Add(InComboGraph);
		
		for (const auto& TransitionPriorityMap : InComboGraph->TransitionPriorityMaps)
		{
			auto RedirectedTransitionPriorityMap = TransitionPriorityMap;
			if (!InputEventRedirect.IsEmpty())
			{
				for (auto It = RedirectedTransitionPriorityMap.Map.CreateIterator(); It; ++It)
				{
					if (const auto& NewTag = InputEventRedirect.Find(It.Key()))
					{
						RedirectedTransitionPriorityMap.Map.Add(*NewTag, It.Value());
						It.RemoveCurrent();
					}
				}
			}
			const int32 MapPriority = RedirectedTransitionPriorityMap.Priority;
			int32 MapIndex;
			if (!UNipcatComboGraph::FindTransitionMapIndexByPriority(CombinedTransitionPriorityMaps, MapPriority, MapIndex))
			{
				CombinedTransitionPriorityMaps.Insert(RedirectedTransitionPriorityMap, MapIndex);
			}
			else
			{
				FNipcatComboTransitionPriorityMap& ExistingPriorityMap = CombinedTransitionPriorityMaps[MapIndex];
				for (auto& [InputTag, TransitionArray] : RedirectedTransitionPriorityMap.Map)
				{
					FNipcatComboTransitionArray& ExistingTransitionArray = ExistingPriorityMap.Map.FindOrAdd(InputTag);
					ExistingTransitionArray.Array.Append(TransitionArray.Array);
				}
			}
		}
	}
}

void UNipcatComboComponent::AddComboGraphs(const TArray<UNipcatComboGraph*>& InComboGraphs, const TMap<FGameplayTag, FGameplayTag>& InputEventRedirect)
{
	for (const auto& ComboGraph : InComboGraphs)
	{
		AddComboGraph(ComboGraph, InputEventRedirect);
	}
}

void UNipcatComboComponent::RemoveComboGraph(UNipcatComboGraph* InComboGraph)
{
	if (!InComboGraph)
	{
		return;
	}
	if (ComboGraphs.Contains(InComboGraph))
	{
		ComboGraphs.Remove(InComboGraph);
		
		for (const auto& TransitionPriorityMap : InComboGraph->TransitionPriorityMaps)
		{
			const int32 MapPriority = TransitionPriorityMap.Priority;
			int32 MapIndex;
			UNipcatComboGraph::FindTransitionMapIndexByPriority(CombinedTransitionPriorityMaps, MapPriority, MapIndex);
			
			FNipcatComboTransitionPriorityMap& ExistingPriorityMap = CombinedTransitionPriorityMaps[MapIndex];
			for (auto& [InputTag, TransitionArray] : TransitionPriorityMap.Map)
			{
				FNipcatComboTransitionArray* ExistingTransitionArray = ExistingPriorityMap.Map.Find(InputTag);
				for (auto& ItemToRemove : TransitionArray.Array)
				{
					ExistingTransitionArray->Array.Remove(ItemToRemove);
				}
			}
		}
	}
}

void UNipcatComboComponent::RemoveComboGraphs(const TArray<UNipcatComboGraph*>& InComboGraphs)
{
	const TArray<UNipcatComboGraph*> ComboGraphsToRemove = InComboGraphs;
	for (const auto& ComboGraph : ComboGraphsToRemove)
	{
		RemoveComboGraph(ComboGraph);
	}
}

void UNipcatComboComponent::RemoveAllComboGraphs()
{
	RemoveComboGraphs(ComboGraphs);
}


void UNipcatComboComponent::AddComboDefinition(const UNipcatComboDefinitionConfig* ComboDefinitionConfig)
{
	if (!ComboDefinitionConfig)
	{
		return;
	}
	
	if (FindComboDefinitionHandleIndex(ComboDefinitionConfig) == INDEX_NONE)
	{
		int32 Index = 0;
		bool Succeed = false;
		for (const auto& Handle : ComboDefinitionHandles)
		{
			// 同优先级时, 先加的在前
			if (ComboDefinitionConfig->Priority > Handle.ComboDefinitionConfig->Priority)
			{
				ComboDefinitionHandles.EmplaceAt(Index, ComboDefinitionConfig);
				Succeed = true;
				break;
			}
			++Index;
		}
		if (!Succeed)
		{
			ComboDefinitionHandles.Emplace(ComboDefinitionConfig);
		}
	}
}

void UNipcatComboComponent::AddComboDefinitions(const TArray<UNipcatComboDefinitionConfig*>& ComboDefinitionConfigs)
{
	for (const auto& ComboDefinitionConfig : ComboDefinitionConfigs)
	{
		AddComboDefinition(ComboDefinitionConfig);
	}
}

void UNipcatComboComponent::RemoveComboDefinition(const UNipcatComboDefinitionConfig* ComboDefinitionConfig)
{
	if (!ComboDefinitionConfig)
	{
		return;
	}
	
	if (const int32 Index = FindComboDefinitionHandleIndex(ComboDefinitionConfig) ; Index != INDEX_NONE)
	{
		FNipcatComboDefinitionHandle& Handle = ComboDefinitionHandles[Index];
		for (auto& AbilityHandle : Handle.AbilityHandles)
		{
			AbilitySystemComponent->ClearAbility(AbilityHandle);
		}
		ComboDefinitionHandles.RemoveAt(Index);
	}
}

void UNipcatComboComponent::RemoveComboDefinitions(const TArray<UNipcatComboDefinitionConfig*>& ComboDefinitionConfigs)
{
	for (const auto& ComboDefinitionConfig : ComboDefinitionConfigs)
	{
		RemoveComboDefinition(ComboDefinitionConfig);
	}
}

void UNipcatComboComponent::RemoveAllComboDefinitions()
{
	for (auto It = ComboDefinitionHandles.CreateIterator(); It; ++It)
	{
		for (auto& AbilityHandle : It->AbilityHandles)
		{
			AbilitySystemComponent->ClearAbility(AbilityHandle);
		}
		It.RemoveCurrent();
	}
}

int32 UNipcatComboComponent::FindComboDefinitionHandleIndex(
	const UNipcatComboDefinitionConfig* ComboDefinitionConfig) const
{
	if (ComboDefinitionConfig)
	{
		return ComboDefinitionHandles.IndexOfByPredicate([ComboDefinitionConfig](const FNipcatComboDefinitionHandle& Handle)
		{
			return Handle.ComboDefinitionConfig == ComboDefinitionConfig;
		});
	}
	return INDEX_NONE;
}

TArray<FNipcatComboTransition> UNipcatComboComponent::GetValidComboTransitions(FGameplayTag& InputEventTag)
{
	TArray<FNipcatComboTransition> OffsetResult;
	TArray<FNipcatComboTransition> NormalResult;
	if (InputEventTag.IsValid() && AbilitySystemComponent.IsValid())
	{
		for (auto& TransitionPriorityMap : CombinedTransitionPriorityMaps)
		{
			if (const auto& ArrayPtr = TransitionPriorityMap.Map.Find(InputEventTag))
			{
				for (FNipcatComboTransition& Transition : ArrayPtr->Array)
				{
					bool IsOffset = false;
					if (Transition.IsValid() && Transition.CanEnterTransition(this, InputEventTag, IsOffset))
					{
						if (IsOffset)
						{
							OffsetResult.Add(Transition);
						}
						else
						{
							NormalResult.Add(Transition);
						}
					}
				}
			}
		}
	}
	OffsetResult.Append(NormalResult);
	return OffsetResult;
}


bool UNipcatComboComponent::TryActivateComboAbility(const FNipcatComboTransition& ComboTransition, bool& OutConsumeInput)
{
	for (auto& Handle : ComboDefinitionHandles)
	{
		if (Handle.IsValid())
		{
			const FGameplayTag& ComboTag = ComboTransition.NextComboTag;
			if (const auto& ComboDefinition = Handle.ComboDefinitionConfig->ComboDefinitions.Find(ComboTag))
			{
				const TSubclassOf<UGameplayAbility> Ability = ComboDefinition->Ability.LoadSynchronous();
				
				FGameplayAbilitySpecHandle AbilitySpecHandle;
				
				if (const auto& AbilitySpec = AbilitySystemComponent->FindAbilitySpecFromClass(Ability))
				{
					AbilitySpecHandle = AbilitySpec->Handle;
				}
				else
				{
					AbilitySpecHandle = AbilitySystemComponent->GiveAbility(Ability);
					Handle.AbilityHandles.Add(AbilitySpecHandle);
				}

				if (auto NipcatAbilityActorInfo = FNipcatGameplayAbilityActorInfo::ExtractActorInfo(AbilitySystemComponent->AbilityActorInfo.Get()))
				{
					NipcatAbilityActorInfo->MontagePlaySettingOverride.Add(Ability, ComboTransition.MontagePlaySettingOverride);
				}
				
				FScopedComboTransitionLock ScopedComboTransitionLock(*this);
				if (AbilitySystemComponent->TryActivateAbility(AbilitySpecHandle))
				{
					LastActivatedAbilitySpecHandle = AbilitySpecHandle;
					LastActivatedAbilityComboTag = ComboTag;
					SetCurrentComboTag(ComboTag, ComboTransition.ComboOffsetRule);
					// 激活第一帧就End的情况
					if (!AbilitySystemComponent->FindAbilitySpecFromHandle(LastActivatedAbilitySpecHandle)->IsActive())
					{
						SetCurrentComboTag(TAG_Combo_Default);
					}
					return true;
				}
				else
				{
					if (auto NipcatAbilityActorInfo = FNipcatGameplayAbilityActorInfo::ExtractActorInfo(AbilitySystemComponent->AbilityActorInfo.Get()))
					{
						NipcatAbilityActorInfo->MontagePlaySettingOverride.Remove(Ability);
					}
					if (ComboDefinition->bConsumeInput)
					{
						OutConsumeInput = true;
						return false;
					}
				}
			}
		}
	}
	return false;
}

FScopedComboTransitionLock::FScopedComboTransitionLock(UNipcatComboComponent& InComboComponent): ComboComponent(InComboComponent)
{
	ComboComponent.AbilityEndedByCombo = true;
}

FScopedComboTransitionLock::~FScopedComboTransitionLock()
{
	ComboComponent.AbilityEndedByCombo = false;
}

void UNipcatComboComponent::SetCurrentComboTag(FGameplayTag InComboTag, ENipcatComboOffsetRule ComboOffsetRule, bool ClearComboOffsetIfDefaultCombo)
{
	if (InComboTag == TAG_Combo_Default && ClearComboOffsetIfDefaultCombo)
	{
		ComboOffsetTag = FGameplayTag::EmptyTag;
	}
	else
	{
		switch (ComboOffsetRule)
		{
		case ENipcatComboOffsetRule::Clear:
			ComboOffsetTag = FGameplayTag::EmptyTag;
			break;
		case ENipcatComboOffsetRule::Hold:
			break;
		case ENipcatComboOffsetRule::Write:
			ComboOffsetTag = InComboTag;
			break;
		}
	}
	
	if (AbilitySystemComponent.IsValid())
	{
		CurrentComboTag = InComboTag;
		AbilitySystemComponent->RemoveLooseGameplayTags(UGameplayTagsManager::Get().RequestGameplayTagChildren(TAG_Combo));
		AbilitySystemComponent->AddLooseGameplayTag(InComboTag);
	}
}

FGameplayTag UNipcatComboComponent::GetCurrentComboTag() const
{
	return CurrentComboTag;
}

FGameplayTag UNipcatComboComponent::GetComboOffsetTag() const
{
	return ComboOffsetTag;
}

bool UNipcatComboComponent::IsComboOffsetValid() const
{
	if (!ComboOffsetTag.IsValid())
	{
		return false;
	}
	if (ComboOffsetTag == CurrentComboTag)
	{
		return false;
	}
	if (!ComboOffsetRequirement.IsEmpty())
	{
		if (!ComboOffsetRequirement.RequirementsMet(AbilitySystemComponent->GetOwnedGameplayTags()))
		{
			return false;
		}
	}
	return true;
}


void UNipcatComboComponent::BeginPlay()
{
	Super::BeginPlay();
	if (ANipcatPlayerController* Controller = Cast<ANipcatPlayerController>(GetOwner()))
	{
		Controller->OnSetPawnDelegate.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	}
	InitASC();
	if (GetOwner()->GetClass()->ImplementsInterface(UNipcatComboComponentOwnerInterface::StaticClass()))
	{
		const auto& ComboActorConfig = INipcatComboComponentOwnerInterface::Execute_GetNipcatComboActorConfig(GetOwner());
		if (IsValid(ComboActorConfig.GetObject()))
		{
			const auto& DefaultComboGraphs = INipcatComboActorConfigInterface::Execute_GetDefaultComboGraphs(ComboActorConfig.GetObject());
			const auto& DefaultComboDefinitions = INipcatComboActorConfigInterface::Execute_GetDefaultComboDefinitions(ComboActorConfig.GetObject());
			AddComboGraphs(DefaultComboGraphs);
			AddComboDefinitions(DefaultComboDefinitions);
		}
	}
}

void UNipcatComboComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DeinitASC();
	if (ANipcatPlayerController* Controller = Cast<ANipcatPlayerController>(GetOwner()))
	{
		Controller->OnSetPawnDelegate.RemoveDynamic(this, &ThisClass::OnPossessedPawnChanged);
	}
	Super::EndPlay(EndPlayReason);
}

void UNipcatComboComponent::InitASC()
{
	if (AController* Controller = Cast<AController>(GetOwner()))
	{
		AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Controller->GetPawn());
	}
	else
	{
		AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	}
	if (AbilitySystemComponent.IsValid())
	{
		if (const auto NipcatASC = Cast<UNipcatAbilitySystemComponent>(AbilitySystemComponent))
		{
			NipcatASC->OnGameplayEventReceived.AddDynamic(this, &ThisClass::OnInputEventReceived);
		}
		// 不用AddGameplayEventTagContainerDelegate, 避免ScopeLock
		// WaitInputEventHandle = AbilitySystemComponent->AddGameplayEventTagContainerDelegate(FGameplayTagContainer(TAG_GameplayEvent_InputAction), FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &UNipcatComboComponent::OnInputEventReceived));
		OnAbilityEndedHandle = AbilitySystemComponent->OnAbilityEnded.AddUObject(this, &ThisClass::OnAbilityEnded);
		SetCurrentComboTag(TAG_Combo_Default);
	}
}

void UNipcatComboComponent::DeinitASC()
{
	if (AbilitySystemComponent.IsValid())
	{
		if (const auto NipcatASC = Cast<UNipcatAbilitySystemComponent>(AbilitySystemComponent))
		{
			NipcatASC->OnGameplayEventReceived.RemoveDynamic(this, &ThisClass::OnInputEventReceived);
		}
		if (OnAbilityEndedHandle.IsValid())
		{
			AbilitySystemComponent->OnAbilityEnded.Remove(OnAbilityEndedHandle);
		}
	}
	AbilitySystemComponent.Reset();
}

void UNipcatComboComponent::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	DeinitASC();
	InitASC();
}

void UNipcatComboComponent::PreInputEventReceived(const FGameplayTag& MatchingTag, const FGameplayEventData& Payload)
{
	PreInputEventReceivedDelegate.Broadcast(MatchingTag, Payload);
}

void UNipcatComboComponent::OnInputEventReceived(FGameplayTag MatchingTag, const FGameplayEventData Payload)
{
	PreInputEventReceived(MatchingTag, Payload);
	auto Transitions = GetValidComboTransitions(MatchingTag);
	for (auto& Transition : Transitions)
	{
		bool bConsumeInput = false;
		if (TryActivateComboAbility(Transition, bConsumeInput))
		{
			return;
		}
		else if (bConsumeInput)
		{
			return;
		}
	}
}

void UNipcatComboComponent::OnAbilityEnded(const FAbilityEndedData& AbilityEndedData)
{
	if (AbilityEndedData.AbilitySpecHandle == LastActivatedAbilitySpecHandle)
	{
		if (CurrentComboTag == LastActivatedAbilityComboTag)
		{
			SetCurrentComboTag(TAG_Combo_Default, ENipcatComboOffsetRule::Hold, !AbilityEndedByCombo);
		}
	}
}
