// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAbilitySystem/AbilityNotify/AbilityNotifyStateInterface.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayEffectTypes.h"
#include "LinkedAnimNotify/NipcatLinkableAnimNotifyInterface.h"
#include "LinkedAnimNotify/NipcatLinkedAnimNotify.h"
#include "NipcatAnimNotifyState.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, EditInlineNew, meta=(ShowWorldContextPin))
class NIPCATABILITYSYSTEM_API UNipcatAnimNotifyState : public UAnimNotifyState, public IAbilityNotifyStateInterface, public INipcatLinkableAnimNotifyInterface
{
	GENERATED_BODY()
public:
	UNipcatAnimNotifyState();
	~UNipcatAnimNotifyState();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FNipcatLinkedAnimNotify> LinkedAnimNotifies;

	virtual TArray<FNipcatLinkedAnimNotify> GetLinkedAnimNotifies_Implementation() override;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagRequirements TagRequirements;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool TriggerOnDragInEditor = true;
	
	virtual bool CanExecuteNotifyState_Implementation(UGameplayAbility* OwningAbility, UAnimMontage* Montage, float TriggerTime, float CurrentAnimationTime, float TotalDuration) const override;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	float ActualStartTime = 0.f;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	float ActualDuration = 0.f;
	
	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
	float ActualEndTime = 0.f;
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;
	virtual FName GetNotifyStateID_Implementation(UGameplayAbility* OwningAbility) const override;

	virtual FLinearColor GetEditorColor() override;
	
	virtual FLinearColor GetNotifyColor_Implementation() const override;

	UPROPERTY()
	USkeletalMeshComponent* MeshContext;
	virtual UWorld* GetWorld() const override;

	virtual void PostDuplicate(bool bDuplicateForPIE) override;

	
#if WITH_EDITOR
	
	void UpdateTimeScaledTriggerTime();
	virtual FAnimNotifyEvent* GetNotifyEvent();
	virtual void OnNotifyChanged();
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
#endif
	
};
