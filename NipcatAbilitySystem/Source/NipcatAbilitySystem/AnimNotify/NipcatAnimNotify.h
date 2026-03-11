// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAbilitySystem/AbilityNotify/AbilityNotifyInterface.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Camera/CameraShakeSourceComponent.h"
#include "GameplayEffectTypes.h"
#include "LinkedAnimNotify/NipcatLinkableAnimNotifyInterface.h"
#include "LinkedAnimNotify/NipcatLinkedAnimNotify.h"

#if WITH_EDITOR
#include "NipcatAbilitySystemEditor/CameraShakePreview/NipcatCameraShakePreviewCameraManager.h"
#endif

#include "NipcatAnimNotify.generated.h"



/**
 * 
 */
UCLASS(Abstract, BlueprintType, EditInlineNew)
class NIPCATABILITYSYSTEM_API UNipcatAnimNotify : public UAnimNotify, public IAbilityNotifyInterface, public INipcatLinkableAnimNotifyInterface
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AlwaysTriggerBeforeStartTimeSeconds = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AlwaysTriggerAfterInterrupt = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FNipcatLinkedAnimNotify> LinkedAnimNotifies;

	virtual TArray<FNipcatLinkedAnimNotify> GetLinkedAnimNotifies_Implementation() override;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	bool ShouldRegisterEditorViewPortModifyEvent;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FGameplayTagRequirements TagRequirements;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool TriggerOnDragInEditor = true;
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual bool CanExecuteNotify_Implementation(UGameplayAbility* OwningAbility, UAnimMontage* Montage, float TriggerTime, float CurrentAnimationTime) const override;

	virtual bool GetAlwaysTriggerBeforeStartTimeSeconds_Implementation() const override;
	virtual bool GetAlwaysTriggerAfterInterrupt_Implementation() const override;
	
	virtual FString GetNotifyName_Implementation() const override;
	virtual FName GetNotifyID_Implementation(UGameplayAbility* OwningAbility) const override;
	
	virtual FLinearColor GetEditorColor() override;
	
	virtual FLinearColor GetNotifyColor_Implementation() const override;

	UFUNCTION(BlueprintCallable, meta=(CallableWithoutWorldContext))
	void PlayPreviewCameraShake(
		TSubclassOf<UCameraShakeBase> ShakeClass,
		float Scale = 1.f,
		ECameraShakePlaySpace PlaySpace = ECameraShakePlaySpace::CameraLocal,
		FRotator UserPlaySpaceRot = FRotator::ZeroRotator,
		UCameraShakeSourceComponent* SourceComponent = nullptr,
		bool bEnablePreview = true) const;

	UFUNCTION(BlueprintCallable, meta=(CallableWithoutWorldContext))
	void StopPreviewCameraShake(const TSubclassOf<UCameraShakeBase> OptionalShakeClass, bool bImmediately = true) const;
	
	UPROPERTY()
	USkeletalMeshComponent* MeshCompContext;
	virtual UWorld* GetWorld() const override;

	virtual void PostDuplicate(bool bDuplicateForPIE) override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
#endif
	
#if WITH_EDITOR
	TUniquePtr<FNipcatCameraShakePreviewer> CameraShakePreviewUpdater;
	void OnModifyView(FEditorViewportViewModifierParams& Params) const;
#endif
	
};
