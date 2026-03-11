// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAnimNotifyState.h"

#include "AbilitySystemComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Animation/AnimMetaData.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"

UNipcatAnimNotifyState::UNipcatAnimNotifyState()
{
	
#if WITH_EDITOR
	if (GetOuter())
	{
		if (UAnimSequenceBase* AnimationSequenceBase = Cast<UAnimSequenceBase>(GetOuter()))
		{
			AnimationSequenceBase->RegisterOnNotifyChanged(UAnimSequenceBase::FOnNotifyChanged::CreateUObject(this, &UNipcatAnimNotifyState::OnNotifyChanged));
		}
	}
#endif
}

UNipcatAnimNotifyState::~UNipcatAnimNotifyState()
{
#if WITH_EDITOR
	if (GetOuter())
	{
		if (UAnimSequenceBase* AnimationSequenceBase = Cast<UAnimSequenceBase>(GetOuter()))
		{
			AnimationSequenceBase->UnregisterOnNotifyChanged(this);
		}
	}
#endif
}

TArray<FNipcatLinkedAnimNotify> UNipcatAnimNotifyState::GetLinkedAnimNotifies_Implementation()
{
	return LinkedAnimNotifies;
}

bool UNipcatAnimNotifyState::CanExecuteNotifyState_Implementation(UGameplayAbility* OwningAbility, UAnimMontage* Montage,
	float TriggerTime, float CurrentAnimationTime, float TotalDuration) const
{
	return TagRequirements.IsEmpty() || TagRequirements.RequirementsMet(OwningAbility->GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags());
}

void UNipcatAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	MeshContext = MeshComp;
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
#if WITH_EDITOR
	if (MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		if (TriggerOnDragInEditor || EventReference.GetCurrentAnimationTime() - EventReference.GetNotify()->GetTriggerTime() >= 0)
		{
			Execute_PreviewBegin(this, MeshComp, Animation, EventReference.GetNotify()->GetTriggerTime(), EventReference.GetCurrentAnimationTime(), TotalDuration);
		}
	}
#endif
}

void UNipcatAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

#if WITH_EDITOR
	if (MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		if (TriggerOnDragInEditor || FrameDeltaTime >= 0)
		{
			Execute_PreviewTick(this, MeshComp, Animation, FrameDeltaTime);
		}
	}
#endif
}

void UNipcatAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

#if WITH_EDITOR
	if (MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		if (TriggerOnDragInEditor || EventReference.GetCurrentAnimationTime() - EventReference.GetNotify()->GetEndTriggerTime() >= 0)
		{
			Execute_PreviewEnd(this, MeshComp, Animation);
		}
	}
#endif
	
	MeshContext = nullptr;
}

FString UNipcatAnimNotifyState::GetNotifyName_Implementation() const
{
	FString Name;
	Super::GetNotifyName_Implementation().Split(TEXT("_"), nullptr,  &Name);
	return *Name;
}

FName UNipcatAnimNotifyState::GetNotifyStateID_Implementation(UGameplayAbility* OwningAbility) const
{
	return FName(OwningAbility->GetName().Append(GetName()));
}

FLinearColor UNipcatAnimNotifyState::GetEditorColor()
{
#if WITH_EDITORONLY_DATA
	return Execute_GetNotifyColor(this);
#else
	return FLinearColor::Black;
#endif // WITH_EDITORONLY_DATA
}

FLinearColor UNipcatAnimNotifyState::GetNotifyColor_Implementation() const
{
#if WITH_EDITOR
	
	// Super::GetEditorColor()
#if WITH_EDITORONLY_DATA
	return FLinearColor(NotifyColor); 
#else
	return FLinearColor::Black;
#endif 
	
#else
	return FLinearColor::Black;
#endif
}

UWorld* UNipcatAnimNotifyState::GetWorld() const
{
	if (MeshContext)
	{
		if (UWorld* World = MeshContext->GetWorld())
		{
			return World;
		}
	}
	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& Context : WorldContexts)
	{
		if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game)) && (Context.World() != NULL))
		{
			return Context.World();
		}
	}
	return nullptr;
}

void UNipcatAnimNotifyState::PostDuplicate(bool bDuplicateForPIE)
{
	Super::PostDuplicate(bDuplicateForPIE);
	
#if WITH_EDITOR
	if (GetOuter())
	{
		if (const UAnimSequenceBase* AnimSequenceBase = Cast<UAnimSequenceBase>(GetOuter()))
		{
			if (!AnimSequenceBase->HasParentAsset())
			{
				Execute_BlueprintPostDuplicate(this, const_cast<FAnimNotifyEvent&>(AnimSequenceBase->Notifies.Last(0)));
			}
		}
	}
#endif
}


#if WITH_EDITOR

void UNipcatAnimNotifyState::UpdateTimeScaledTriggerTime()
{
	if (GetOuter())
	{
		if (const UAnimSequenceBase* AnimSequenceBase = Cast<UAnimSequenceBase>(GetOuter()))
		{
			if (const FAnimNotifyEvent* NotifyEvent = GetNotifyEvent())
			{
				const IAnimationDataModel* Model = AnimSequenceBase->GetDataModel();
				if (const FFloatCurve* PlayRateCurve = Model->FindFloatCurve(FAnimationCurveIdentifier(TEXT("PlayRate"), ERawCurveTrackTypes::RCT_Float)))
				{
					const float StartTime = NotifyEvent->GetTime();
					const float Duration = NotifyEvent->GetDuration();
					ActualStartTime = UNipcatBasicGameplayFunctionLibrary::GetScaledTimeStep(*PlayRateCurve, 0.f, StartTime);
					ActualDuration = UNipcatBasicGameplayFunctionLibrary::GetScaledTimeStep(*PlayRateCurve, StartTime, Duration);
					ActualEndTime = ActualStartTime + ActualDuration;
				}
				else
				{
					ActualStartTime = NotifyEvent->GetTriggerTime();
					ActualDuration = NotifyEvent->GetDuration();
					ActualEndTime = NotifyEvent->GetEndTriggerTime();
				}
			}
		}
	}
}


FAnimNotifyEvent* UNipcatAnimNotifyState::GetNotifyEvent()
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
						if (Event->NotifyStateClass)
						{
							if (Event->NotifyStateClass == this)
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
	return FoundNotifyEvent;
}

void UNipcatAnimNotifyState::OnNotifyChanged()
{
	UpdateTimeScaledTriggerTime();
}

void UNipcatAnimNotifyState::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if (GetOuter())
	{
		if (const UAnimMontage* AnimMontage = Cast<UAnimMontage>(GetOuter()))
		{
			for (UAnimMetaData* MetaData : AnimMontage->GetMetaData())
			{
				MetaData->PostEditChangeProperty(PropertyChangedEvent);
			}
		}
	}
	Execute_BlueprintPostEditChangeProperty(this, PropertyChangedEvent.GetPropertyName(), PropertyChangedEvent.GetMemberPropertyName());
}

void UNipcatAnimNotifyState::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	Execute_BlueprintOnAnimNotifyCreatedInEditor(this, ContainingAnimNotifyEvent);
}
#endif
