// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatFlow/NipcatFlowComponent.h"
#include "NipcatDialogueFlowComponent.generated.h"


class UNipcatDialogueSubsystem;

UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent))
class NIPCATFLOW_API UNipcatDialogueFlowComponent : public UNipcatFlowComponent
{
	GENERATED_BODY()

public:
	UNipcatDialogueFlowComponent();
	
	UPROPERTY(VisibleAnywhere, SaveGame)
	TMap<FNipcatDialogueOptionID, int32> OptionResult;
	
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="OptionIDHandle"))
	int32 FindOptionResult(const FDataTableRowHandle& OptionIDHandle) const;
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="OptionIDHandle"))
	void RegisterOptionResult(const FDataTableRowHandle& OptionIDHandle, int32 Index);
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="OptionIDHandle"))
	void UnregisterOptionResult(const FDataTableRowHandle& OptionIDHandle);
	
	UNipcatDialogueSubsystem* GetDialogueSubsystem() const;
};
