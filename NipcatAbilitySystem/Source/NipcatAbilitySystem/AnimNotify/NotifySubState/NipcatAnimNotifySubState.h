// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAbilitySystem/AnimNotify/NipcatAnimNotifyState.h"
#include "NipcatAnimNotifySubState.generated.h"

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatAnimNotifySubStateDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString NotifyName;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FColor Color = FColor(200, 200, 255, 255);

	bool operator==(const FNipcatAnimNotifySubStateDefinition& Other) const
	{
		return NotifyName == Other.NotifyName;
	}
};

/**
 * 
 */
UCLASS(Abstract)
class NIPCATABILITYSYSTEM_API UNipcatAnimNotifySubState : public UNipcatAnimNotifyState
{
	GENERATED_BODY()

public:
	UNipcatAnimNotifySubState();
	
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	FString SubStateGroupName;

	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
	TArray<FNipcatAnimNotifySubStateDefinition> SubStateDefinitions;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 Index;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FString NotifySubStateName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName PrimarySubStateName;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UNipcatAnimNotifySubState* PrimarySubState;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UNipcatAnimNotifySubState* PrevSubState;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UNipcatAnimNotifySubState* Self;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UNipcatAnimNotifySubState* NextSubState;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float CachedTriggerTime;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float CachedDuration;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	float CachedEndTime;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	int32 CachedTrackIndex;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FName NotifyEventName;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool bInitialized;
	
	virtual FString GetNotifyName_Implementation() const override;
	virtual FLinearColor GetEditorColor() override;

	TArray<UNipcatAnimNotifySubState*> GetAllSubStates() const;
	
private:
	FAnimNotifyEvent* NotifyEvent;

public:
	virtual void PostDuplicate(bool bDuplicateForPIE) override;

	
#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, CallInEditor)
	void UpdateWithSubStateDefinition();
	
	virtual FAnimNotifyEvent* GetNotifyEvent() override;
	FAnimNotifyEvent* GetNotifyEventByIndex(int32 InIndex);
	
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	virtual void OnAnimNotifyCreatedInEditor(FAnimNotifyEvent& ContainingAnimNotifyEvent) override;
	
	virtual void OnNotifyChanged() override;

	virtual void PostLoad() override;
	
	// static FAnimNotifyEvent* AddAnimationNotifyStateEventObject(UAnimSequenceBase* AnimationSequenceBase, float StartTime, float Duration, UAnimNotifyState* NotifyState, FName NotifyTrackName);

#endif
	
};
