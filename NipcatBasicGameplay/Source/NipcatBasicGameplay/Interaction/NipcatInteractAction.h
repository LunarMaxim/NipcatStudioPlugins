// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatInteractAction.generated.h"

class UNipcatInteractableComponent;
class UNipcatInteractComponent;
class INipcatInteractableInterface;
class UNipcatInteractCondition;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, Abstract, CollapseCategories)
class NIPCATBASICGAMEPLAY_API UNipcatInteractAction : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void ExecuteAction(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, const FText& FailedReason = FText::GetEmpty(), UNipcatInteractCondition* FailedCondition = nullptr);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="ExecuteAction")
	void BP_ExecuteAction(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, const FText& FailedReason = FText::GetEmpty(), UNipcatInteractCondition* FailedCondition = nullptr);

	UPROPERTY()
	AActor* InstigatorContext;
	
	UPROPERTY()
	TScriptInterface<INipcatInteractableInterface> InteractableContext;
	
	virtual UWorld* GetWorld() const override;
};
