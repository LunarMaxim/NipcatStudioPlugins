// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_AbilityEvent.generated.h"

/**
 * 
 */
UCLASS(DisplayName="AbilityEvent")
class NIPCATABILITYSYSTEM_API UAnimNotify_AbilityEvent : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, DisplayName="通知名称", Category="配置")
	FString NotifyName;
	UPROPERTY(EditAnywhere, DisplayName="标签", Category="配置")
	FGameplayTag EventTag;
};
