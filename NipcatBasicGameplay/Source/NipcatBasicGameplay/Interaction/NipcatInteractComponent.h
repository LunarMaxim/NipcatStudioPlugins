// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include "NipcatInteractComponent.generated.h"


class UNipcatIndicatorManagerComponent;
class UNipcatInteractableComponent;
struct FNipcatInteractConfig;
struct FNipcatInteractStateConfig;
class UIndicatorDescriptor;
class UNipcatInteractableList;
class UInputAction;
class INipcatInteractableInterface;

namespace NipcatInteractTags
{
	NIPCATBASICGAMEPLAY_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Actor_Interactable)
}

USTRUCT(BlueprintType)
struct NIPCATBASICGAMEPLAY_API FNipcatInteractableList
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere)
	TArray<TWeakObjectPtr<UNipcatInteractableComponent>> List;
	
};

USTRUCT(BlueprintType)
struct NIPCATBASICGAMEPLAY_API FNipcatInteractableState
{
	GENERATED_BODY()

	FNipcatInteractableState() = default;
	FNipcatInteractableState(FGameplayTag InStateTag, bool InSelected)
	{
		StateTag = InStateTag;
		Selected = InSelected;
	}
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	FGameplayTag StateTag;
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	bool Selected;

	bool IsValid() const
	{
		return StateTag.IsValid();
	}

	bool operator==(const FNipcatInteractableState& Other) const
	{
		return StateTag == Other.StateTag && Selected == Other.Selected;
	}
};

UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class NIPCATBASICGAMEPLAY_API UNipcatInteractComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNipcatInteractComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	double MaxInteractRange = 2000.f;
	
	UPROPERTY(VisibleAnywhere, Transient)
	TArray<TWeakObjectPtr<UNipcatInteractableComponent>> InRangeInteractable;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ForceInlineRow), Transient)
	TMap<TSoftObjectPtr<UInputAction>, FNipcatInteractableList> InteractableList;

	UPROPERTY(VisibleAnywhere, meta=(ForceInlineRow))
	TMap<TWeakObjectPtr<UObject>, FNipcatInteractableState> InteractableState;

	UPROPERTY(VisibleAnywhere)
	TMap<TWeakObjectPtr<UObject>, TObjectPtr<UIndicatorDescriptor>> Indicators;
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateInRangeInteractableActors();
	
	void UpdateInteractableList();
	
	UFUNCTION(BlueprintCallable)
	bool TryInteract(UInputAction* InputAction);
	
	UFUNCTION(BlueprintCallable)
	void CyclePrevInteractable(UInputAction* InputAction);
	
	UFUNCTION(BlueprintCallable)
	void CycleNextInteractable(UInputAction* InputAction);
	
	UFUNCTION(BlueprintPure)
	bool HasInteractable(UInputAction* InputAction);
	
	UFUNCTION()
	void AddInteractable(const TSoftObjectPtr<UInputAction>& InputAction, const TWeakObjectPtr<UNipcatInteractableComponent>& Interactable);
	
	UFUNCTION()
	void RemoveInteractable(const TSoftObjectPtr<UInputAction>& InputAction, const TWeakObjectPtr<UNipcatInteractableComponent>& Interactable);

	UFUNCTION()
	void SortInteractable();
	
	UFUNCTION()
	void ForceRemoveInteractable(const TWeakObjectPtr<UNipcatInteractableComponent>& Interactable);
	
	UFUNCTION(BlueprintCallable, meta=(GameplayTagFilter="InteractState"))
	void SetInteractState(UNipcatInteractableComponent* Interactable, FGameplayTag StateTag);

protected:
	UPROPERTY()
	TMap<UNipcatInteractableComponent*, FGameplayTag> InteractStateToUpdateThisFrame;
	
	UFUNCTION()
	void SetInteractState_Internal(UNipcatInteractableComponent* Interactable, FGameplayTag StateTag, bool Selected);

public:
	
	UFUNCTION()
	void NotifyInteractStateUpdated(UNipcatInteractableComponent* Interactable, const FNipcatInteractableState& OldState, const FNipcatInteractableState& NewState);
	
	UFUNCTION(BlueprintPure)
	FNipcatInteractableState GetInteractState(UNipcatInteractableComponent* Interactable) const;

	UFUNCTION()
	UNipcatIndicatorManagerComponent* GetIndicatorManager() const;

	UFUNCTION()
	void AddIndicator(const TWeakObjectPtr<UNipcatInteractableComponent>& InteractableComponent);

	UFUNCTION()
	void OnBindIndicator(UIndicatorDescriptor* IndicatorDescriptor, UUserWidget* Widget);
	
	UFUNCTION()
	void RemoveIndicator(const TWeakObjectPtr<UNipcatInteractableComponent>& InteractableComponent);
	
	UFUNCTION()
	UIndicatorDescriptor* GetIndicator(const TWeakObjectPtr<UObject>& Interactable) const;
	
	UFUNCTION()
	virtual void UpdateSelectedInteractable(UNipcatInteractableComponent* OldSelectedInteractable, UNipcatInteractableComponent* NewSelectedInteractable);
	
	UFUNCTION(BlueprintPure)
	UNipcatInteractableComponent* GetSelectedInteractable(UInputAction* InputAction);

	UFUNCTION()
	static TScriptInterface<INipcatInteractableInterface> GetInteractInterface(const TWeakObjectPtr<UObject>& WeakObjectPtr);
	
	UFUNCTION()
	static TArray<UNipcatInteractableComponent*> GetInteractableComponents(const TWeakObjectPtr<UObject>& WeakObjectPtr);
};
