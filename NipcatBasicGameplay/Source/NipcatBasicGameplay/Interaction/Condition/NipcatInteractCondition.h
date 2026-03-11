// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatBasicGameplay/Interaction/NipcatInteractComponent.h"

#include "NipcatInteractCondition.generated.h"

class INipcatInteractableInterface;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, EditInlineNew, Abstract, CollapseCategories)
class NIPCATBASICGAMEPLAY_API UNipcatInteractCondition : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bInverseCondition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText DefaultFailedReason;

	UFUNCTION(BlueprintPure)
	FText GetFailedReason(const UNipcatInteractableComponent* Interactable) const;

	UFUNCTION(BlueprintCallable)
	bool EvaluateCondition(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, FText& OutFailedReason);
	
	UFUNCTION(BlueprintNativeEvent)
	bool CanInteract(UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, FText& OutFailedReason);
	
	UFUNCTION(BlueprintCallable, Category="NipcatInteract")
	static bool EvaluateInteractConditions(TArray<UNipcatInteractCondition*> Conditions, UNipcatInteractComponent* InteractComponent, UNipcatInteractableComponent* Interactable, FText& OutFailedReason, int32& FailedConditionIndex);

	UPROPERTY()
	UObject* InstigatorContext;
	
	UPROPERTY()
	UNipcatInteractableComponent* InteractableContext;
	
	virtual UWorld* GetWorld() const override;
};
