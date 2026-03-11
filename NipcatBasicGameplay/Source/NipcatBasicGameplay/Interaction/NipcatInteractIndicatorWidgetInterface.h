// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "NipcatInteractIndicatorWidgetInterface.generated.h"

struct FNipcatInteractableState;
class UNipcatInteractComponent;
class UNipcatInteractableComponent;
class UNipcatInteractCondition;
class INipcatInteractableInterface;

UINTERFACE(MinimalAPI, BlueprintType)
class UNipcatInteractIndicatorWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class NIPCATBASICGAMEPLAY_API INipcatInteractIndicatorWidgetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="NipcatInteract")
	void OnInteractStateUpdated(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, const FNipcatInteractableState& OldState, const FNipcatInteractableState& NewState);
	
	UFUNCTION(BlueprintNativeEvent, Category="NipcatInteract")
	void OnInteractSucceed(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, FGameplayTag StateTag);
	
	UFUNCTION(BlueprintNativeEvent, Category="NipcatInteract")
	void OnInteractFailed(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, FGameplayTag StateTag, const FText& FailedReason = FText::GetEmpty(), UNipcatInteractCondition* FailedCondition = nullptr);
};
