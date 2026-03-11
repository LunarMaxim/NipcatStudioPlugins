// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ChooserPropertyAccess.h"
#include "IChooserParameterGameplayTag.h"
#include "GameplayTagContainer.h"


#include "NipcatChooserTypes.generated.h"

/**
 * 
 */

USTRUCT(DisplayName = "Single Gameplay Tag Property Binding")
struct NIPCATBASICGAMEPLAY_API FSingleGameplayTagContextProperty :  public FChooserParameterGameplayTagBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, NoClear, Meta = (BindingType = "FGameplayTag", BindingColor = "StructPinTypeColor"), Category = "Binding")
	FChooserPropertyBinding Binding;
	
	virtual bool GetValue(FChooserEvaluationContext& Context, const FGameplayTagContainer*& OutResult) const override;

	FGameplayTagContainer TagContainer;
	
	CHOOSER_PARAMETER_BOILERPLATE();
};

USTRUCT(DisplayName = "Parent Gameplay Tag Property Binding")
struct NIPCATBASICGAMEPLAY_API FParentGameplayTagContextProperty :  public FChooserParameterGameplayTagBase
{
	GENERATED_BODY()
public:

	UPROPERTY(EditAnywhere, NoClear, Meta = (BindingType = "FGameplayTag", BindingColor = "StructPinTypeColor"), Category = "Binding")
	FChooserPropertyBinding Binding;
	
	virtual bool GetValue(FChooserEvaluationContext& Context, const FGameplayTagContainer*& OutResult) const override;
	
	FGameplayTagContainer TagContainer;
	
	CHOOSER_PARAMETER_BOILERPLATE();
};