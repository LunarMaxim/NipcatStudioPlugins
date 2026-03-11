// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAnimNotify.h"

#include "AbilitySystemComponent.h"
#include "Camera/CameraModifier_CameraShake.h"
#include "Abilities/GameplayAbility.h"
#include "Camera/CameraShakeSourceComponent.h"
#include "Animation/AnimMetaData.h"


TArray<FNipcatLinkedAnimNotify> UNipcatAnimNotify::GetLinkedAnimNotifies_Implementation()
{
	return LinkedAnimNotifies;
}

void UNipcatAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                               const FAnimNotifyEventReference& EventReference)
{
	MeshCompContext = MeshComp;
#if WITH_EDITOR
	if (GEditor && ShouldRegisterEditorViewPortModifyEvent)
	{
		const TArray<FEditorViewportClient*>& ViewportClients = GEditor->GetAllViewportClients();
		for (FEditorViewportClient* ViewportClient : ViewportClients)
		{
			if(ViewportClient->GetWorld()->GetMapName() == TEXT("Transient"))
			{
				if (!ViewportClient->ViewModifiers.IsBoundToObject(this))
				{
					ViewportClient->ViewModifiers.AddUObject(this, &UNipcatAnimNotify::OnModifyView);
				}
			}
		}
		if (!CameraShakePreviewUpdater)
		{
			CameraShakePreviewUpdater.Reset(new FNipcatCameraShakePreviewer());
		}
	}
#endif
	
	Super::Notify(MeshComp, Animation, EventReference);

#if WITH_EDITOR
	if (MeshComp->GetWorld()->WorldType == EWorldType::EditorPreview)
	{
		if (TriggerOnDragInEditor || EventReference.GetCurrentAnimationTime() - EventReference.GetNotify()->GetTriggerTime() >= 0)
		{
			Execute_PreviewExecute(this, MeshComp, Animation, EventReference.GetNotify()->GetTriggerTime(), EventReference.GetCurrentAnimationTime());
		}
	}	
#endif
	MeshCompContext = nullptr;
}

bool UNipcatAnimNotify::CanExecuteNotify_Implementation(UGameplayAbility* OwningAbility, UAnimMontage* Montage, float TriggerTime, float CurrentAnimationTime) const
{
	return TagRequirements.IsEmpty() || TagRequirements.RequirementsMet(OwningAbility->GetAbilitySystemComponentFromActorInfo()->GetOwnedGameplayTags());
}

bool UNipcatAnimNotify::GetAlwaysTriggerBeforeStartTimeSeconds_Implementation() const
{
	return AlwaysTriggerBeforeStartTimeSeconds;
}

bool UNipcatAnimNotify::GetAlwaysTriggerAfterInterrupt_Implementation() const
{
	return AlwaysTriggerAfterInterrupt;
}

FString UNipcatAnimNotify::GetNotifyName_Implementation() const
{
	FString Name;
	Super::GetNotifyName_Implementation().Split(TEXT("_"), nullptr,  &Name);
	return *Name;
}

FName UNipcatAnimNotify::GetNotifyID_Implementation(UGameplayAbility* OwningAbility) const
{
	return FName(OwningAbility->GetName().Append(GetName()));
}


FLinearColor UNipcatAnimNotify::GetEditorColor()
{
#if WITH_EDITOR
	return Execute_GetNotifyColor(this);
#else
	return FLinearColor::Black;
#endif
}

FLinearColor UNipcatAnimNotify::GetNotifyColor_Implementation() const
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

void UNipcatAnimNotify::PlayPreviewCameraShake(TSubclassOf<UCameraShakeBase> ShakeClass, float Scale,
	ECameraShakePlaySpace PlaySpace, FRotator UserPlaySpaceRot, UCameraShakeSourceComponent* SourceComponent,
	bool bEnablePreview) const
{
	
#if WITH_EDITOR
	if(bEnablePreview && CameraShakePreviewUpdater)
	{
		FAddCameraShakeParams Params;
		Params.Scale = Scale;
		Params.PlaySpace = PlaySpace;
		Params.UserPlaySpaceRot = UserPlaySpaceRot;
		Params.SourceComponent = SourceComponent;
		UCameraShakeBase* ShakeInstance = CameraShakePreviewUpdater->AddCameraShake(ShakeClass, Params);
	}
#endif
}

void UNipcatAnimNotify::StopPreviewCameraShake(const TSubclassOf<UCameraShakeBase> OptionalShakeClass,
	bool bImmediately) const
{
#if WITH_EDITOR
	if(CameraShakePreviewUpdater)
	{
		CameraShakePreviewUpdater->RemoveAllCameraShake(OptionalShakeClass, bImmediately);
	}
#endif
}

UWorld* UNipcatAnimNotify::GetWorld() const
{
	if (MeshCompContext)
	{
		if (UWorld* World = MeshCompContext->GetWorld())
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

void UNipcatAnimNotify::PostDuplicate(bool bDuplicateForPIE)
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
void UNipcatAnimNotify::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
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

void UNipcatAnimNotify::OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent)
{
	Super::OnAnimNotifyCreatedInEditor(ContainingAnimNotifyEvent);
	Execute_BlueprintOnAnimNotifyCreatedInEditor(this, ContainingAnimNotifyEvent);
}
#endif

#if WITH_EDITOR
void UNipcatAnimNotify::OnModifyView(FEditorViewportViewModifierParams& Params) const
{
	CameraShakePreviewUpdater->ModifyCamera(Params);
}
#endif
