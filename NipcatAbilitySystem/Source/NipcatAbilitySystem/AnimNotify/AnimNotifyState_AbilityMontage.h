// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AbilityMontage.generated.h"

class INipcatMontageNotifyAbilityInterface;
/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, meta=(DisplayName="AbilityMontage"))
class NIPCATABILITYSYSTEM_API UAnimNotifyState_AbilityMontage : public UAnimNotifyState
{
	GENERATED_BODY()

public:
#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, DisplayName="通知名称", Category="AbilityMontageTag")
	FString NotifyName;
#endif

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	INipcatMontageNotifyAbilityInterface* GetNotifyAbility(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation);
};


UCLASS(BlueprintType, Blueprintable, meta=(DisplayName="AbilityMontageTag"))
class NIPCATABILITYSYSTEM_API UAnimNotifyState_AbilityMontageTag : public UAnimNotifyState_AbilityMontage
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, DisplayName="标签", Category="AbilityMontage", meta=(DisplayAfter="NotifyName"))
	FGameplayTag Tag;
};
