// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatComboGraph.h"

#include "AbilitySystemComponent.h"
#include "NipcatAbilitySystem/Combo/NipcatComboComponent.h"
#include "NipcatAbilitySystem/WorldConditions/NipcatAbilitySystemWorldConditionSchema.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"

FNipcatComboTransition::FNipcatComboTransition()
{
	// Conditions.DebugInitialize(nullptr, UNipcatAbilitySystemWorldConditionSchema::StaticClass(), TConstArrayView<FWorldConditionEditable>());
	// UObject* Outer = nullptr;
	
	// const UNipcatAbilitySystemWorldConditionSchema* Schema = GetDefault<UNipcatAbilitySystemWorldConditionSchema>();
	// ContextData = FWorldConditionContextData(*Schema);
	// Conditions.Activate(*Outer, ContextData);
}

bool FNipcatComboTransition::CanEnterTransition(const UNipcatComboComponent* ComboComponent, const FGameplayTag& InputEventTag, bool& IsOffset)
{
	if (!ComboComponent || !InputEventTag.IsValid())
	{
		return false;
	}
	// ContextData.SetContextData(GetDefault<UNipcatAbilitySystemWorldConditionSchema>()->GetASCRef(), ASC);

	const auto ASC = ComboComponent->AbilitySystemComponent.Get();
	if (!ASC)
	{
		return false;
	}
	
	const bool InputEventTagChecked = InputActions.IsEmpty() || InputActions.HasTag(InputEventTag);

	bool CurrentComboRequirementsChecked = CurrentComboRequirements.IsEmpty();
	if (!CurrentComboRequirementsChecked)
	{
		if (ComboOffsetRule != ENipcatComboOffsetRule::Hold)
		{
			if (ComboComponent->IsComboOffsetValid())
			{
				CurrentComboRequirementsChecked = CurrentComboRequirements.RequirementsMet(FGameplayTagContainer(ComboComponent->GetComboOffsetTag()));
			}
		}
		if (CurrentComboRequirementsChecked)
		{
			IsOffset = true;
		}
		else
		{
			CurrentComboRequirementsChecked = CurrentComboRequirements.RequirementsMet(FGameplayTagContainer(ComboComponent->GetCurrentComboTag()));
		}
	}
	
	const bool TagRequirementsChecked = OwningTagRequirements.IsEmpty() || OwningTagRequirements.RequirementsMet(ASC->GetOwnedGameplayTags());
	// const bool WorldConditionChecked = Conditions.IsTrue(ContextData);
	const bool WorldConditionChecked = true;
	
	return InputEventTagChecked && CurrentComboRequirementsChecked && TagRequirementsChecked && WorldConditionChecked;
}

bool UNipcatComboGraph::FindTransitionMapIndexByPriority(TArray<FNipcatComboTransitionPriorityMap>& InTransitionPriorityMaps,
	const int32& InPriority, int32& OutIndex)
{
	OutIndex = 0;
	for (const auto& TransitionMap : InTransitionPriorityMaps)
	{
		const int32 FoundPriority = TransitionMap.Priority;
		if (InPriority < FoundPriority)
		{
			++OutIndex;
		}
		else if (InPriority == FoundPriority)
		{
			return true;
		}
		else if (InPriority > FoundPriority)
		{
			return false;
		}
	}
	return false;
}

void UNipcatComboGraph::BuildTransitionPriorityMap()
{
	TransitionPriorityMaps.Empty();
	for (auto& Transition : Transitions)
	{
		if (Transition.IsValid())
		{
			int32 TransitionPriority = Transition.Priority;
			int32 IndexOfTransitionMaps;
			if (!FindTransitionMapIndexByPriority(TransitionPriorityMaps, TransitionPriority, IndexOfTransitionMaps))
			{
				TransitionPriorityMaps.EmplaceAt(IndexOfTransitionMaps, TransitionPriority);
			}
			auto& TransitionMap = TransitionPriorityMaps[IndexOfTransitionMaps];
			
			if (Transition.InputActions.IsEmpty())
			{
				auto& ActionArray = TransitionMap.Map.FindOrAdd(FGameplayTag::EmptyTag).Array;
				ActionArray.AddUnique(Transition);
			}
			else
			{
				for (const auto& Tag : Transition.InputActions)
				{
					auto& ActionArray = TransitionMap.Map.FindOrAdd(Tag).Array;
					ActionArray.AddUnique(Transition);
				}
			}
		}
	}
}

#if WITH_EDITOR

void UNipcatComboGraph::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, Transitions))
	{
		BuildTransitionPriorityMap();
	}
}

void UNipcatComboGraph::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
	
#if WITH_EDITORONLY_DATA
	if (PropertyChangedEvent.Property)
	{
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FNipcatComboTransition, Comment))
		{
			const auto Index = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, Transitions));

			if (Transitions.IsValidIndex(Index))
			{
				auto& Transition = Transitions[Index];
				
				if (Transition.Comment.IsEmpty())
				{
					Transition.bCommentModified = false;
					UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(Transition.Comment, Transition.NextComboTag, Transition.bCommentModified);
				}
				else if (Transition.Comment != Transition.NextComboTag.ToString())
				{
					Transition.bCommentModified = true;
				}
			}
		}
		if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_STRING_VIEW_CHECKED(FNipcatComboTransition, NextComboTag))
		{
			const auto Index = PropertyChangedEvent.GetArrayIndex(GET_MEMBER_NAME_STRING_CHECKED(ThisClass, Transitions));

			if (Transitions.IsValidIndex(Index))
			{
				auto& Transition = Transitions[Index];
				UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(Transition.Comment, Transition.NextComboTag, Transition.bCommentModified);
			}
		}
	}
#endif
}

void UNipcatComboGraph::PostLoad()
{
	Super::PostLoad();
	BuildTransitionPriorityMap();
	for (auto& Transition : Transitions)
	{
		UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(Transition.Comment, Transition.NextComboTag, Transition.bCommentModified);
	}
}

#endif