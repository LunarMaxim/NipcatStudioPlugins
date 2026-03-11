// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_AbilityEvents.generated.h"

/**
 * 
 */
UCLASS(DisplayName=AbilityEvents)
class NIPCATABILITYSYSTEM_API UAnimNotifyState_AbilityEvents : public UAnimNotifyState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, DisplayName="通知名称", Category="配置")
	FString NotifyName{TEXT("")};
	
	UPROPERTY(EditAnywhere, DisplayName="开始标签", Category="配置")
	FGameplayTag StartEventTag;
	UPROPERTY(EditAnywhere, DisplayName="Tick标签", Category="配置")
	FGameplayTag TickEventTag;
	UPROPERTY(EditAnywhere, DisplayName="结束标签", Category="配置")
	FGameplayTag EndEventTag;

public:
	virtual FString GetNotifyName_Implementation() const override;
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
