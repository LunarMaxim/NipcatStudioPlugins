// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractComponent.h"

#include "NativeGameplayTags.h"
#include "NipcatInteractableInterface.h"
#include "Condition/NipcatInteractCondition.h"
#include "NipcatInteractTypes.h"
#include "NipcatInteractAction.h"
#include "InputAction.h"
#include "NipcatInteractableComponent.h"
#include "NipcatInteractIndicatorWidgetInterface.h"
#include "UMG.h"
#include "NipcatBasicGameplay/ActorTag/NipcatActorTagSubsystem.h"
#include "NipcatBasicGameplay/UI/IndicatorSystem/IndicatorDescriptor.h"
#include "NipcatBasicGameplay/UI/IndicatorSystem/NipcatIndicatorManagerComponent.h"
#include "NipcatBasicGameplay/UI/IndicatorSystem/NipcatIndicatorComponent.h"
#include "NipcatBasicGameplay/UI/IndicatorSystem/NipcatIndicatorManagerComponentOwnerInterface.h"

namespace NipcatInteractTags
{
	UE_DEFINE_GAMEPLAY_TAG(Actor_Interactable, "Actor.Interactable")
}

UNipcatInteractComponent::UNipcatInteractComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}


void UNipcatInteractComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	UpdateInRangeInteractableActors();
	UpdateInteractableList();
}

void UNipcatInteractComponent::UpdateInRangeInteractableActors()
{
	TArray<TWeakObjectPtr<UNipcatInteractableComponent>> OldInRangeInteractable = InRangeInteractable;
	InRangeInteractable.Empty();
	const auto InRangeInteractableActors = GetWorld()->GetSubsystem<UNipcatActorTagSubsystem>()->GetWeakActorsOfTagInRange(NipcatInteractTags::Actor_Interactable, GetOwner()->GetActorLocation(), MaxInteractRange);
	for (auto Actor : InRangeInteractableActors)
	{
		if (Actor.IsValid() && Actor.Get()->GetClass()->ImplementsInterface(UNipcatInteractableInterface::StaticClass()))
		{
			InRangeInteractable.Append(INipcatInteractableInterface::Execute_GetNipcatInteractableComponents(Actor.Get()));
		}
	}
	for (auto OldInRangeInteractableComponent : OldInRangeInteractable)
	{
		if (!InRangeInteractable.Contains(OldInRangeInteractableComponent))
		{
			ForceRemoveInteractable(OldInRangeInteractableComponent);
		}
	}
	for (auto InRangeInteractableComponent : InRangeInteractable)
	{
		if (InRangeInteractableComponent.IsValid())
		{
			if (!OldInRangeInteractable.Contains(InRangeInteractableComponent))
			{
				AddIndicator(InRangeInteractableComponent.Get());
			}
		}
	}
}

void UNipcatInteractComponent::UpdateInteractableList()
{
	for (auto It = InRangeInteractable.CreateIterator(); It; ++It)
	{
		if (It->IsValid())
		{
			const auto InteractableComponent = It->Get();
			FNipcatInteractConfig InteractConfig = InteractableComponent->GetInteractConfig();
		
			bool HasValidState = false;
			for (const auto& State : InteractConfig.States)
			{
				if (!InteractableComponent->IsStateEnabled(State.StateTag))
				{
					continue;
				}
				int32 FailedConditionIndex = -1;
				FText FailedReason;
				if (UNipcatInteractCondition::EvaluateInteractConditions(State.EnterStateConditions, this, InteractableComponent, FailedReason, FailedConditionIndex))
				{
					SetInteractState(InteractableComponent, State.StateTag);
					HasValidState = true;
					if (State.HasInteractActions)
					{
						AddInteractable(InteractConfig.InputAction, *It);
					}
					else
					{
						RemoveInteractable(InteractConfig.InputAction, *It);
					}
					break;
				}
			}
			if (!HasValidState)
			{
				SetInteractState(InteractableComponent, FGameplayTag::EmptyTag);
				RemoveInteractable(InteractConfig.InputAction, *It);
			}
		}
	}

	SortInteractable();

	for (auto& [Interactable, StateTag] : InteractStateToUpdateThisFrame)
	{
		SetInteractState_Internal(Interactable, StateTag, GetSelectedInteractable(Interactable->GetInteractConfig().InputAction.LoadSynchronous()) == Interactable ? true : false);
	}
	
	InteractStateToUpdateThisFrame.Empty();
}


bool UNipcatInteractComponent::TryInteract(UInputAction* InputAction)
{
	if (IsValid(InputAction))
	{
		if (const auto Interactable = GetSelectedInteractable(InputAction); IsValid(Interactable))
		{
			const FNipcatInteractConfig& InteractConfig = Interactable->GetInteractConfig();

			if (InteractConfig.States.IsEmpty())
			{
				return false;
			}
			
			FNipcatInteractStateConfig StateConfig = InteractConfig.States[0];
			
			if (const auto State = GetInteractState(Interactable); State.IsValid())
			{
				if (const auto FoundResult = InteractConfig.States.FindByPredicate([=](const FNipcatInteractStateConfig& Config)
				{
					return Config.StateTag == State.StateTag;
				}))
				{
					StateConfig = *FoundResult;
				}
			}
			
			int32 FailedConditionIndex = -1;
			FText FailedReason;
			if (UNipcatInteractCondition::EvaluateInteractConditions(StateConfig.InteractConditions, this, Interactable, FailedReason, FailedConditionIndex))
			{
				if (const auto Indicator = GetIndicator(Interactable))
				{
					if (const auto IndicatorWidget = Indicator->IndicatorWidget; IndicatorWidget.IsValid())
					{
						if (IndicatorWidget.Get()->GetClass()->ImplementsInterface(UNipcatInteractIndicatorWidgetInterface::StaticClass()))
						{
							INipcatInteractIndicatorWidgetInterface::Execute_OnInteractSucceed(Indicator->IndicatorWidget.Get(), this, Interactable, StateConfig.StateTag);
						}
					}
				}
				
				for (const auto Action : StateConfig.SucceedActions)
				{
					if (Action)
					{
						Action->ExecuteAction(this, Interactable);
					}
				}

				// Maybe Save in BP
				Interactable->OnInteractSucceed.Broadcast(this, StateConfig.StateTag);

				return true;
			}
			
			Interactable->OnInteractFailed.Broadcast(this, StateConfig.StateTag, FailedReason, StateConfig.InteractConditions[FailedConditionIndex]);

			if (const auto Indicator = GetIndicator(Interactable))
			{
				if (const auto IndicatorWidget = Indicator->IndicatorWidget; IndicatorWidget.IsValid())
				{
					if (IndicatorWidget.Get()->GetClass()->ImplementsInterface(UNipcatInteractIndicatorWidgetInterface::StaticClass()))
					{
						INipcatInteractIndicatorWidgetInterface::Execute_OnInteractFailed(Indicator->IndicatorWidget.Get(), this, Interactable, StateConfig.StateTag, FailedReason, StateConfig.InteractConditions[FailedConditionIndex]);
					}
				}
			}
			
			for (const auto Action : StateConfig.FailedActions)
			{
				if (Action)
				{
					Action->ExecuteAction(this, Interactable, FailedReason, StateConfig.InteractConditions[FailedConditionIndex]);
				}
			}
		}
	}
	return false;
}

void UNipcatInteractComponent::CyclePrevInteractable(UInputAction* InputAction)
{
	auto& List = InteractableList.FindOrAdd(TSoftObjectPtr(InputAction)).List;
	if (List.Num() > 1)
	{
		List.Add(List[0]);
		List.RemoveAt(0);
		UpdateSelectedInteractable(List.Last(1).Get(), List.Last(0).Get());
	}
}

void UNipcatInteractComponent::CycleNextInteractable(UInputAction* InputAction)
{
	auto& List = InteractableList.FindOrAdd(TSoftObjectPtr(InputAction)).List;
	if (List.Num() > 1)
	{
		const auto LastElement = List.Pop();
		List.Insert(LastElement, 0);
		UpdateSelectedInteractable(List[0].Get(), List.Last(0).Get());
	}
}

bool UNipcatInteractComponent::HasInteractable(UInputAction* InputAction)
{
	if (const auto List = InteractableList.Find(TSoftObjectPtr(InputAction)))
	{
		return !List->List.IsEmpty();
	}
	return false;
}

void UNipcatInteractComponent::AddInteractable(const TSoftObjectPtr<UInputAction>& InputAction,
                                               const TWeakObjectPtr<UNipcatInteractableComponent>& Interactable)
{
	if (!InputAction.IsNull() && Interactable.IsValid())
	{
		auto& List = InteractableList.FindOrAdd(InputAction).List;
		if (!List.Contains(Interactable))
		{
			List.Add(Interactable);
		}
	}
}

void UNipcatInteractComponent::RemoveInteractable(const TSoftObjectPtr<UInputAction>& InputAction,
	const TWeakObjectPtr<UNipcatInteractableComponent>& Interactable)
{
	if (!InputAction.IsNull())
	{
		InteractableList.FindOrAdd(InputAction).List.Remove(Interactable);
	}
}

void UNipcatInteractComponent::SortInteractable()
{
	for (auto& [InputAction, List] : InteractableList)
	{
		List.List.StableSort([this](const TWeakObjectPtr<UNipcatInteractableComponent>& A, const TWeakObjectPtr<UNipcatInteractableComponent>& B)
		{
			return FVector::DistSquared(A->GetInteractPointLocation(), GetOwner()->GetActorLocation()) > FVector::DistSquared(B->GetInteractPointLocation(), GetOwner()->GetActorLocation());
		});
	}
}

void UNipcatInteractComponent::ForceRemoveInteractable(const TWeakObjectPtr<UNipcatInteractableComponent>& Interactable)
{
	for (auto& [Input, List] : InteractableList)
	{
		for (auto It = List.List.CreateIterator(); It; ++It)
		{
			if (*It == Interactable)
			{
				It.RemoveCurrent();
			}
		}
	}
	for (auto It = InteractableState.CreateIterator(); It; ++It)
	{
		if (It.Key() == Interactable)
		{
			It.RemoveCurrent();
		}
	}
	RemoveIndicator(Interactable);
}

void UNipcatInteractComponent::SetInteractState(UNipcatInteractableComponent* Interactable,
                                                FGameplayTag StateTag)
{
	InteractStateToUpdateThisFrame.Add(Interactable, StateTag);
}

void UNipcatInteractComponent::SetInteractState_Internal(UNipcatInteractableComponent* Interactable,
                                                         FGameplayTag StateTag, bool Selected)
{
	if (IsValid(Interactable))
	{
		const FNipcatInteractableState OldState = GetInteractState(Interactable);
		const FNipcatInteractableState NewState(StateTag, Selected);
		if (OldState != NewState)
		{
			InteractableState.Add(Interactable, NewState);
			NotifyInteractStateUpdated(Interactable, OldState, NewState);	
		}
	}
}


void UNipcatInteractComponent::NotifyInteractStateUpdated(
	UNipcatInteractableComponent* Interactable, const FNipcatInteractableState& OldState,
	const FNipcatInteractableState& NewState)
{
	Interactable->OnInteractStateUpdated.Broadcast(this, OldState, NewState);

	if (const auto Indicator = GetIndicator(TWeakObjectPtr(Interactable)))
	{
		if (const auto IndicatorWidget = Indicator->IndicatorWidget; IndicatorWidget.IsValid())
		{
			if (IndicatorWidget.Get()->GetClass()->ImplementsInterface(UNipcatInteractIndicatorWidgetInterface::StaticClass()))
			{
				INipcatInteractIndicatorWidgetInterface::Execute_OnInteractStateUpdated(Indicator->IndicatorWidget.Get(), this, Interactable, OldState, NewState);
			}
		}
	}	
}

FNipcatInteractableState UNipcatInteractComponent::GetInteractState(
	UNipcatInteractableComponent* Interactable) const
{
	if (IsValid(Interactable))
	{
		if (const auto State = InteractableState.Find(Interactable))
		{
			return *State;
		}
	}
	return FNipcatInteractableState();
}

UNipcatIndicatorManagerComponent* UNipcatInteractComponent::GetIndicatorManager() const
{
	if (GetOwner()->GetClass()->ImplementsInterface(UNipcatIndicatorManagerComponentOwnerInterface::StaticClass()))
	{
		return INipcatIndicatorManagerComponentOwnerInterface::Execute_GetNipcatIndicatorManagerComponent(GetOwner());
	}
	if (const auto Pawn = Cast<APawn>(GetOwner()))
	{
		if (const auto Controller = Pawn->GetController())
		{
			if (Controller->GetClass()->ImplementsInterface(UNipcatIndicatorManagerComponentOwnerInterface::StaticClass()))
			{
				return INipcatIndicatorManagerComponentOwnerInterface::Execute_GetNipcatIndicatorManagerComponent(Controller);
			}
		}
	}
	return nullptr;
}

void UNipcatInteractComponent::AddIndicator(const TWeakObjectPtr<UNipcatInteractableComponent>& InteractableComponent)
{
	if (InteractableComponent.IsValid())
	{
		if (const auto IndicatorManager = GetIndicatorManager())
		{
			UIndicatorDescriptor* IndicatorDescriptor = NewObject<UIndicatorDescriptor>(this);
			IndicatorDescriptor->SetDataObject(InteractableComponent.Get());
			
			if (const auto IndicatorComponent = InteractableComponent->GetIndicator())
			{
				IndicatorComponent->ApplyIndicatorConfig(IndicatorDescriptor);
			}
			IndicatorDescriptor->OnBindIndicator.AddDynamic(this, &ThisClass::OnBindIndicator);
			IndicatorManager->AddIndicator(IndicatorDescriptor);

			Indicators.Add(InteractableComponent, IndicatorDescriptor);
		}
	}
}

void UNipcatInteractComponent::OnBindIndicator(UIndicatorDescriptor* IndicatorDescriptor, UUserWidget* Widget)
{
	if (const auto Interactable = Cast<UNipcatInteractableComponent>(IndicatorDescriptor->GetDataObject()))
	{
		NotifyInteractStateUpdated(Interactable, FNipcatInteractableState(), GetInteractState(Interactable));
	}
}

void UNipcatInteractComponent::RemoveIndicator(const TWeakObjectPtr<UNipcatInteractableComponent>& InteractableComponent)
{
	if (const auto Indicator = GetIndicator(InteractableComponent))
	{
		if (const auto IndicatorManager = GetIndicatorManager())
		{
			IndicatorManager->RemoveIndicator(Indicator);
			Indicators.Remove(InteractableComponent);
		}
	}
}

UIndicatorDescriptor* UNipcatInteractComponent::GetIndicator(
	const TWeakObjectPtr<UObject>& Interactable) const
{
	if (Interactable.IsValid())
	{
		if (const auto FoundResult = Indicators.Find(Interactable))
		{
			return FoundResult->Get();
		}
	}
	return nullptr;
}

void UNipcatInteractComponent::UpdateSelectedInteractable(UNipcatInteractableComponent* OldSelectedInteractable,
	UNipcatInteractableComponent* NewSelectedInteractable)
{
	FNipcatInteractableState OldInteractableState = GetInteractState(OldSelectedInteractable);
	FNipcatInteractableState NewInteractableState = GetInteractState(NewSelectedInteractable);

	OldInteractableState.Selected = false;
	NewInteractableState.Selected = true;
	
	NotifyInteractStateUpdated(OldSelectedInteractable, OldInteractableState, OldInteractableState);
	NotifyInteractStateUpdated(NewSelectedInteractable, NewInteractableState, NewInteractableState);
}

UNipcatInteractableComponent* UNipcatInteractComponent::GetSelectedInteractable(UInputAction* InputAction)
{
	if (IsValid(InputAction))
	{
		auto& List = InteractableList.FindOrAdd(TSoftObjectPtr(InputAction)).List;
		if (!List.IsEmpty())
		{
			return List.Last().Get();
		}
	}
	return nullptr;
}

TScriptInterface<INipcatInteractableInterface> UNipcatInteractComponent::GetInteractInterface(
	const TWeakObjectPtr<UObject>& WeakObjectPtr)
{
	if (WeakObjectPtr.IsValid())
	{
		if (WeakObjectPtr.Get()->GetClass()->ImplementsInterface(UNipcatInteractableInterface::StaticClass()))
		{
			return TScriptInterface<INipcatInteractableInterface>(WeakObjectPtr.Get());
		}
	}
	return TScriptInterface<INipcatInteractableInterface>();
}

TArray<UNipcatInteractableComponent*> UNipcatInteractComponent::GetInteractableComponents(
	const TWeakObjectPtr<UObject>& WeakObjectPtr)
{
	TArray<UNipcatInteractableComponent*> Result;
	if (WeakObjectPtr.IsValid())
	{
		if (WeakObjectPtr.Get()->GetClass()->ImplementsInterface(UNipcatInteractableInterface::StaticClass()))
		{
			Result = INipcatInteractableInterface::Execute_GetNipcatInteractableComponents(WeakObjectPtr.Get());
		}
	}
	return Result;
}