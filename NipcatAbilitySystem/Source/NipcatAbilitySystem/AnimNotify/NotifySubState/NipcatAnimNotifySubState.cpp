// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAnimNotifySubState.h"

#if WITH_EDITOR
#include "AnimationBlueprintLibrary.h"
#endif
#include "Kismet/KismetMathLibrary.h"


UNipcatAnimNotifySubState::UNipcatAnimNotifySubState()
{
	Self = this;
}

FString UNipcatAnimNotifySubState::GetNotifyName_Implementation() const
{
	// return NotifySubStateName;
	if (SubStateDefinitions.IsValidIndex(Index))
	{
		return SubStateDefinitions[Index].NotifyName;
	}
	return Super::GetNotifyName_Implementation();
}

FLinearColor UNipcatAnimNotifySubState::GetEditorColor()
{
	if (SubStateDefinitions.IsValidIndex(Index))
	{
		return SubStateDefinitions[Index].Color;
	}
	return Super::GetEditorColor();
}

TArray<UNipcatAnimNotifySubState*> UNipcatAnimNotifySubState::GetAllSubStates() const
{
	TArray<UNipcatAnimNotifySubState*> Result;
	auto State = PrimarySubState;
	while (State)
	{
		Result.Add(State);
		State = State->NextSubState;
	}
	return Result;
}


void UNipcatAnimNotifySubState::PostDuplicate(bool bDuplicateForPIE)
{
#if WITH_EDITOR
	Self = this;

#endif
	Super::PostDuplicate(bDuplicateForPIE);
}

#if WITH_EDITOR
void UNipcatAnimNotifySubState::UpdateWithSubStateDefinition()
{
	const int32 IndexInDefinition = SubStateDefinitions.IndexOfByPredicate([this](const FNipcatAnimNotifySubStateDefinition& Def)
	{
		return Def.NotifyName == NotifySubStateName;
	});

	if (IndexInDefinition == INDEX_NONE)
	{
		if (PrevSubState)
		{
			PrevSubState->NextSubState = NextSubState;
			PrevSubState->GetNotifyEvent()->SetDuration(PrevSubState->GetNotifyEvent()->GetDuration() + GetNotifyEvent()->GetDuration());
			NextSubState->PrevSubState = PrevSubState;
		}
		else
		{
			NextSubState->PrimarySubState = NextSubState;
			NextSubState->PrimarySubStateName = FName(NextSubState->NotifySubStateName);
		}
		auto& NextState = NextSubState;
		while (NextState)
		{
			NextState->Index--;
			if (NextState->NextSubState)
			{
				NextState->NextSubState->PrimarySubState = NextState->PrimarySubState;
				NextState->NextSubState->PrimarySubStateName = NextState->PrimarySubStateName;
			}
			NextState = NextState->NextSubState;
		}
		UAnimSequenceBase* AnimationSequenceBase = const_cast<UAnimSequenceBase*>(GetNotifyEvent()->GetLinkedMontage()? GetNotifyEvent()->GetLinkedMontage() : GetNotifyEvent()->GetLinkedSequence());
		if (AnimationSequenceBase)
		{
			AnimationSequenceBase->Notifies.RemoveAll(
				[&](const FAnimNotifyEvent& Event)
			{
				return Event == *GetNotifyEvent();
			});

			// Refresh all cached data
			AnimationSequenceBase->RefreshCacheData();
		}
	}
	else
	{
		for (int32 InsertIndex = 0; InsertIndex < SubStateDefinitions.Num(); ++InsertIndex)
		{
			const auto& SubStateDefinition = SubStateDefinitions[InsertIndex];
			const auto& AllSubStates = GetAllSubStates();
			if (!AllSubStates.ContainsByPredicate([SubStateDefinition](const auto& SubState)
			{
				return SubState->NotifySubStateName == SubStateDefinition.NotifyName;
			}))
			{
				UAnimSequenceBase* AnimationSequenceBase = const_cast<UAnimSequenceBase*>(GetNotifyEvent()->GetLinkedMontage()? GetNotifyEvent()->GetLinkedMontage() : GetNotifyEvent()->GetLinkedSequence());
	
				UNipcatAnimNotifySubState* NewNotifyState = NewObject<UNipcatAnimNotifySubState>(AnimationSequenceBase, GetClass(), NAME_None, RF_Transactional);
				NewNotifyState->Index = InsertIndex;
				NewNotifyState->NotifySubStateName = SubStateDefinition.NotifyName;
				
				if (InsertIndex != 0)
				{
					NewNotifyState->PrevSubState = AllSubStates[InsertIndex - 1];
					NewNotifyState->PrimarySubStateName = PrimarySubStateName;
					NewNotifyState->PrimarySubState = PrimarySubState;
					AllSubStates[InsertIndex - 1]->NextSubState = NewNotifyState;
				}
				else
				{
					NewNotifyState->PrimarySubStateName = FName(NewNotifyState->NotifySubStateName);
					NewNotifyState->PrimarySubState = NewNotifyState;
				}
				if (InsertIndex < AllSubStates.Num())
				{
					NewNotifyState->NextSubState = AllSubStates[InsertIndex];
					AllSubStates[InsertIndex]->PrevSubState =NewNotifyState;
				}

				const float StartTime = InsertIndex == 0 ? 0 : AllSubStates[InsertIndex - 1]->GetNotifyEvent()->GetEndTriggerTime();
				const float Duration = InsertIndex >= AllSubStates.Num() ? AnimationSequenceBase->GetPlayLength() - StartTime : AllSubStates[InsertIndex]->GetNotifyEvent()->GetDuration() / 2;

				UAnimationBlueprintLibrary::AddAnimationNotifyStateEventObject(
					AnimationSequenceBase,
					StartTime,
					Duration,
					NewNotifyState,
					AnimationSequenceBase->AnimNotifyTracks[GetNotifyEvent()->TrackIndex].TrackName);

				NewNotifyState->GetNotifyEvent()->TrackIndex =GetNotifyEvent()->TrackIndex;
				NewNotifyState->CachedTrackIndex = GetNotifyEvent()->TrackIndex;
				NewNotifyState->GetNotifyEvent()->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetAfter);
				NewNotifyState->GetNotifyEvent()->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetBefore);
		
				NewNotifyState->CachedTriggerTime = NewNotifyState->GetNotifyEvent()->GetTriggerTime();
				NewNotifyState->CachedDuration = NewNotifyState->GetNotifyEvent()->GetDuration();
				NewNotifyState->CachedEndTime = NewNotifyState->GetNotifyEvent()->GetEndTriggerTime();
			}
		}
	}
}

FAnimNotifyEvent* UNipcatAnimNotifySubState::GetNotifyEvent()
{
	FAnimNotifyEvent* FoundNotifyEvent = Super::GetNotifyEvent();
	if (FoundNotifyEvent && NotifyEventName != FoundNotifyEvent->GetNotifyEventName())
	{
		NotifyEventName = FoundNotifyEvent->GetNotifyEventName();
	}
	return FoundNotifyEvent;
}

FAnimNotifyEvent* UNipcatAnimNotifySubState::GetNotifyEventByIndex(int32 InIndex)
{
	FAnimNotifyEvent* FoundNotifyEvent = nullptr;
	if (UObject* Outer = GetOuter())
	{
		if (UAnimSequenceBase* AnimationSequenceBase = Cast<UAnimSequenceBase>(Outer))
		{
			for (auto& AnimNotifyTrack : AnimationSequenceBase->AnimNotifyTracks)
			{
				if (FAnimNotifyEvent** FoundNotifyEventPtr = AnimNotifyTrack.Notifies.FindByPredicate([&](const FAnimNotifyEvent* Event)
					{
						if (const UNipcatAnimNotifySubState* SubState = Cast<UNipcatAnimNotifySubState>(Event->NotifyStateClass))
						{
							if (SubState->Index == InIndex)
							{
								return true;
							}
						}
						return false;
					}))
				{
					if (*FoundNotifyEventPtr)
					{
						FoundNotifyEvent = *FoundNotifyEventPtr;
						break;
					}
				}
			}
		}
	}
	if (FoundNotifyEvent && NotifyEventName != FoundNotifyEvent->GetNotifyEventName())
	{
		NotifyEventName = FoundNotifyEvent->GetNotifyEventName();
	}
	return FoundNotifyEvent;
}

void UNipcatAnimNotifySubState::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (SubStateDefinitions.IsValidIndex(Index))
	{
		if (NotifyColor != SubStateDefinitions[Index].Color)
		{
			NotifyColor = SubStateDefinitions[Index].Color;
		}
	}
}

void UNipcatAnimNotifySubState::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);

	if (!ContainingAnimNotifyEvent.NotifyStateClass || ContainingAnimNotifyEvent.NotifyStateClass.GetClass() != GetClass())
	{
		return;
	}
	
	PrimarySubStateName = GetFName();
	PrimarySubState = this;

	UAnimSequenceBase* AnimationSequenceBase = const_cast<UAnimSequenceBase*>(ContainingAnimNotifyEvent.GetLinkedMontage()? ContainingAnimNotifyEvent.GetLinkedMontage() : ContainingAnimNotifyEvent.GetLinkedSequence());

	UNipcatAnimNotifySubState* TempPrevSubState = this;
	FAnimNotifyEvent* PrevSubStateEvent = &ContainingAnimNotifyEvent;
	PrevSubStateEvent->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetAfter);
	PrevSubStateEvent->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetBefore);
	
	CachedTriggerTime = 0;
	CachedDuration = UKismetMathLibrary::SafeDivide(AnimationSequenceBase->GetPlayLength(),SubStateDefinitions.Num());
		
	ContainingAnimNotifyEvent.SetTime(CachedTriggerTime);
	ContainingAnimNotifyEvent.SetDuration(CachedDuration);

	CachedEndTime = ContainingAnimNotifyEvent.GetEndTriggerTime();

	// NotifyEvent = &ContainingAnimNotifyEvent;
	NotifyEventName = ContainingAnimNotifyEvent.GetNotifyEventName();
	
	NotifySubStateName = SubStateDefinitions[0].NotifyName;
	
	for(int32 CurIndex = 1; CurIndex < SubStateDefinitions.Num(); ++CurIndex)
	{
		UNipcatAnimNotifySubState* NewNotifyState = NewObject<UNipcatAnimNotifySubState>(AnimationSequenceBase, GetClass(), NAME_None, RF_Transactional);
		NewNotifyState->Index = CurIndex;
		NewNotifyState->NotifySubStateName = SubStateDefinitions[CurIndex].NotifyName;
		NewNotifyState->PrevSubState = TempPrevSubState;
		NewNotifyState->PrimarySubStateName = PrimarySubStateName;
		NewNotifyState->PrimarySubState = PrimarySubState;
		
		TempPrevSubState->NextSubState = NewNotifyState;

		UAnimationBlueprintLibrary::AddAnimationNotifyStateEventObject(
			AnimationSequenceBase,
			PrevSubStateEvent->GetEndTriggerTime(),
			PrevSubStateEvent->GetDuration(),
			NewNotifyState,
			AnimationSequenceBase->AnimNotifyTracks[ContainingAnimNotifyEvent.TrackIndex].TrackName);
		
		PrevSubStateEvent = NewNotifyState->GetNotifyEvent();
		PrevSubStateEvent->TrackIndex = NewNotifyState->PrevSubState->GetNotifyEvent()->TrackIndex;
		NewNotifyState->CachedTrackIndex = PrevSubStateEvent->TrackIndex;
		PrevSubStateEvent->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetAfter);
		PrevSubStateEvent->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetBefore);
		
		TempPrevSubState = Cast<UNipcatAnimNotifySubState>(PrevSubStateEvent->NotifyStateClass);
		TempPrevSubState->CachedTriggerTime = PrevSubStateEvent->GetTriggerTime();
		TempPrevSubState->CachedDuration = PrevSubStateEvent->GetDuration();
		TempPrevSubState->CachedEndTime = PrevSubStateEvent->GetEndTriggerTime();
		// TempPrevSubState->NotifyEvent = PrevSubStateEvent;
		TempPrevSubState->NotifyEventName = TempPrevSubState->GetNotifyEvent()->GetNotifyEventName();
	}

	UNipcatAnimNotifySubState* CurSubNotifyState = this;
	while (CurSubNotifyState)
	{
		CurSubNotifyState->bInitialized = true;
		CurSubNotifyState = CurSubNotifyState->NextSubState;
	}
}

void UNipcatAnimNotifySubState::OnNotifyChanged()
{
	Super::OnNotifyChanged();
	if (PrimarySubState->GetOuter() != GetOuter())
	{
		if (const FAnimNotifyEvent* FoundNotifyEvent = GetNotifyEventByIndex(0))
		{
			PrimarySubState = Cast<UNipcatAnimNotifySubState>(FoundNotifyEvent->NotifyStateClass);
			PrimarySubStateName = PrimarySubState->GetFName();
		}
	}
	if (PrevSubState && PrevSubState->GetOuter() != GetOuter())
	{
		if (const FAnimNotifyEvent* FoundNotifyEvent = GetNotifyEventByIndex(PrevSubState->Index))
		{
			PrevSubState = Cast<UNipcatAnimNotifySubState>(FoundNotifyEvent->NotifyStateClass);
		}
	}
	if (NextSubState && NextSubState->GetOuter() != GetOuter())
	{
		if (const FAnimNotifyEvent* FoundNotifyEvent = GetNotifyEventByIndex(NextSubState->Index))
		{
			NextSubState = Cast<UNipcatAnimNotifySubState>(FoundNotifyEvent->NotifyStateClass);
		}
	}

	if (GetOuter())
	{
		if (const UAnimSequenceBase* AnimationSequenceBase = Cast<UAnimSequenceBase>(GetOuter()))
		{
			if (AnimationSequenceBase->HasParentAsset())
			{
				return;
			}
		}
	}
	
	if (!bInitialized)
	{
		return;
	}
	if (!GetNotifyEvent())
	{
		return;
	}
	
	if (NotifyEventName != GetNotifyEvent()->GetNotifyEventName())
	{
		NotifyEventName = GetNotifyEvent()->GetNotifyEventName();
	}
	
	if (GetNotifyEvent()->TrackIndex != CachedTrackIndex)
	{
		const int32 NewIndex = GetNotifyEvent()->TrackIndex;
		UNipcatAnimNotifySubState* CurSubState = PrimarySubState;
		do 
		{
			CurSubState->CachedTrackIndex = NewIndex;
			CurSubState->GetNotifyEvent()->TrackIndex = NewIndex;
			CurSubState->GetNotifyEvent()->RefreshSegmentOnLoad();
			CurSubState = CurSubState->NextSubState;
		}
		while (CurSubState);
	}

	CachedTrackIndex = GetNotifyEvent()->TrackIndex;


	float CurTriggerTime = GetNotifyEvent()->GetTriggerTime();
	const float CurCachedTriggerTime = CachedTriggerTime;
	const float CurDuration = GetNotifyEvent()->GetDuration();
	const float CurCachedDuration= CachedDuration;
	float CurEndTime = GetNotifyEvent()->GetEndTriggerTime();
	const float CurCachedEndTime = CachedEndTime;


	if (!FMath::IsNearlyEqual(CurTriggerTime, CurCachedTriggerTime))
	{
		if (PrevSubState && PrevSubState->GetNotifyEvent())
		{
			const float PrevTriggerTime = PrevSubState->GetNotifyEvent()->GetTriggerTime();

			if (CurTriggerTime < PrevTriggerTime)
			{
				CurTriggerTime = PrevTriggerTime;
			}

			PrevSubState->GetNotifyEvent()->SetDuration(CurTriggerTime - PrevTriggerTime);
			PrevSubState->GetNotifyEvent()->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetAfter);
			PrevSubState->GetNotifyEvent()->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetBefore);
			PrevSubState->CachedTriggerTime = PrevSubState->GetNotifyEvent()->GetTriggerTime();
			PrevSubState->CachedDuration = PrevSubState->GetNotifyEvent()->GetDuration();
			PrevSubState->CachedEndTime = PrevSubState->GetNotifyEvent()->GetEndTriggerTime();
		}
		
		GetNotifyEvent()->SetDuration(CurCachedEndTime - CurTriggerTime);
		GetNotifyEvent()->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetAfter);
		GetNotifyEvent()->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetBefore);
		CachedTriggerTime = GetNotifyEvent()->GetTriggerTime();
		CachedDuration = GetNotifyEvent()->GetDuration();
		CachedEndTime = GetNotifyEvent()->GetEndTriggerTime();
		
	}
	else if (PrevSubState && PrevSubState->GetNotifyEvent())
	{
		const float PrevEndTime = PrevSubState->GetNotifyEvent()->GetEndTriggerTime();
		const float PrevDuration = PrevSubState->GetNotifyEvent()->GetDuration();
		const float PrevCachedEndTime = PrevSubState->CachedEndTime;
		if (!FMath::IsNearlyEqual(PrevEndTime, PrevCachedEndTime))
		{
			GetNotifyEvent()->SetTime(PrevEndTime);
			GetNotifyEvent()->SetDuration(CurDuration - (PrevEndTime - PrevCachedEndTime));
			GetNotifyEvent()->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetAfter);
			GetNotifyEvent()->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetBefore);
			CachedTriggerTime = GetNotifyEvent()->GetTriggerTime();
			CachedDuration = GetNotifyEvent()->GetDuration();
			CachedEndTime = GetNotifyEvent()->GetEndTriggerTime();
			
			PrevSubState->CachedDuration = PrevDuration;
			PrevSubState->CachedEndTime = PrevEndTime;
		}
	}
	if (!FMath::IsNearlyEqual(CurEndTime, CurCachedEndTime))
	{
		if (NextSubState && NextSubState->GetNotifyEvent())
		{
			const float NextEndTime = NextSubState->GetNotifyEvent()->GetEndTriggerTime();

			if (CurEndTime > NextEndTime)
			{
				CurEndTime = NextEndTime;
			}

			NextSubState->GetNotifyEvent()->SetTime(CurEndTime);
			NextSubState->GetNotifyEvent()->SetDuration(NextEndTime - CurEndTime);
			NextSubState->GetNotifyEvent()->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetAfter);
			NextSubState->GetNotifyEvent()->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetBefore);
			NextSubState->CachedTriggerTime = NextSubState->GetNotifyEvent()->GetTriggerTime();
			NextSubState->CachedDuration = NextSubState->GetNotifyEvent()->GetDuration();
			NextSubState->CachedEndTime = NextSubState->GetNotifyEvent()->GetEndTriggerTime();
		}
		
		CachedTriggerTime = GetNotifyEvent()->GetTriggerTime();
		CachedDuration = GetNotifyEvent()->GetDuration();
		CachedEndTime = GetNotifyEvent()->GetEndTriggerTime();
		
	}
	else if (NextSubState && NextSubState->GetNotifyEvent())
	{
		const float NextTriggerTime = NextSubState->GetNotifyEvent()->GetTriggerTime();
		const float NextCachedTriggerTime = NextSubState->CachedTriggerTime;
		if (!FMath::IsNearlyEqual(NextTriggerTime, NextCachedTriggerTime))
		{
			GetNotifyEvent()->SetDuration(NextTriggerTime - CurTriggerTime);
			GetNotifyEvent()->TriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetAfter);
			GetNotifyEvent()->EndTriggerTimeOffset = GetTriggerTimeOffsetForType(EAnimEventTriggerOffsets::Type::OffsetBefore);
			CachedTriggerTime = GetNotifyEvent()->GetTriggerTime();
			CachedDuration = GetNotifyEvent()->GetDuration();
			CachedEndTime = GetNotifyEvent()->GetEndTriggerTime();
			
			NextSubState->CachedTriggerTime = NextTriggerTime;
			NextSubState->CachedDuration = NextSubState->GetNotifyEvent()->GetDuration();
			NextSubState->CachedEndTime = NextSubState->GetNotifyEvent()->GetEndTriggerTime();
		}
	}
}

void UNipcatAnimNotifySubState::PostLoad()
{
	Super::PostLoad();

	if (const FAnimNotifyEvent* FoundNotifyEvent = GetNotifyEvent())
	{
		if (NotifySubStateName.IsEmpty())
		{
			if (SubStateDefinitions.IsValidIndex(Index))
			{
				NotifySubStateName = SubStateDefinitions[Index].NotifyName;
			}
		}
		
		if (CachedTriggerTime != FoundNotifyEvent->GetTriggerTime())
		{
			CachedTriggerTime = FoundNotifyEvent->GetTriggerTime();
		}
		if (CachedDuration != FoundNotifyEvent->GetDuration())
		{
			CachedDuration = FoundNotifyEvent->GetDuration();
		}
		if (CachedEndTime != FoundNotifyEvent->GetEndTriggerTime())
		{
			CachedEndTime = FoundNotifyEvent->GetEndTriggerTime();
		}
	}
}

/*
FAnimNotifyEvent* UNipcatAnimNotifySubState::AddAnimationNotifyStateEventObject(UAnimSequenceBase* AnimationSequenceBase,
                                                                                float StartTime, float Duration, UAnimNotifyState* NotifyState, FName NotifyTrackName)
{
	FAnimNotifyEvent& NewEvent = AnimationSequenceBase->Notifies.AddDefaulted_GetRef();

	NewEvent.NotifyName = NAME_None;
	NewEvent.Link(AnimationSequenceBase, StartTime);
	NewEvent.TriggerTimeOffset = GetTriggerTimeOffsetForType(AnimationSequenceBase->CalculateOffsetForNotify(StartTime));
	NewEvent.TrackIndex = UAnimationBlueprintLibrary::GetTrackIndexForAnimationNotifyTrackName(AnimationSequenceBase, NotifyTrackName);
	NewEvent.NotifyStateClass = NotifyState;
	NewEvent.Notify = nullptr;
	NewEvent.SetDuration(Duration);
	NewEvent.EndLink.Link(AnimationSequenceBase, NewEvent.EndLink.GetTime());
	NewEvent.Guid = FGuid::NewGuid();

	// Refresh all cached data
	AnimationSequenceBase->RefreshCacheData();
	return &NewEvent;
}*/

#endif
