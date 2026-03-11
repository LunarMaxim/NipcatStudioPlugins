// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatFlow/NipcatFlowTypes.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "NipcatDialogueSubsystem.generated.h"

class UNipcatDialogueFlowComponent;
class UNipcatDialogueFlowAsset;
struct FNipcatDialogueParagraph;
class UFlowComponent;
DECLARE_DYNAMIC_DELEGATE_OneParam(FNipcatDialogueFinishDelegate, int32, SelectedOptionIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FNipcatOptionResultRegisteredDelegate, const FDataTableRowHandle&, OptionIDHandle, int32, SelectedOptionIndex);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable, Abstract)
class NIPCATFLOW_API UNipcatDialogueSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY(BlueprintReadWrite, VisibleAnywhere)
	TObjectPtr<UUserWidget> DialogueWidget;
	
	UPROPERTY(BlueprintAssignable)
	FNipcatOptionResultRegisteredDelegate OnOptionResultRegistered;
	
	UFUNCTION(BlueprintPure)
	UNipcatDialogueFlowComponent* GetLocalPlayerDialogueFlowComponent() const;
	
	UFUNCTION()
	void StartDialogue(UNipcatDialogueFlowAsset* FlowAsset);
	
	UFUNCTION()
	void FinishDialogue(UNipcatDialogueFlowAsset* FlowAsset);
	
	// --- Flow Node 专用接口 ---
	
	UFUNCTION(BlueprintCallable)
	void PushParagraph(FNipcatDialogueParagraph Paragraph, FNipcatDialogueFinishDelegate OnFinish);
	
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="OptionIDHandle"))
	int32 FindOptionResult(const FDataTableRowHandle& OptionIDHandle) const;
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="OptionIDHandle"))
	void RegisterOptionResult(const FDataTableRowHandle& OptionIDHandle, int32 Index);
	
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="OptionIDHandle"))
	void UnregisterOptionResult(const FDataTableRowHandle& OptionIDHandle);
	
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="OptionIDHandle"))
	static FNipcatDialogueOptionID GetOptionID(const FDataTableRowHandle& OptionIDHandle);
	
	// --- UI 专用接口 ---
	
	UFUNCTION(BlueprintCallable)
	void Continue();
	
	UFUNCTION(BlueprintCallable)
	void Skip();
	
	UFUNCTION(BlueprintCallable)
	void SelectOption(int32 Index);
	
	UFUNCTION(BlueprintNativeEvent)
	void ShowOptions(const TArray<FNipcatDialogueOption>& Options);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnSentenceUpdate(const FNipcatDialogueSentence& Sentence, int32 Index);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnParagraphUpdate(const FNipcatDialogueParagraph& Paragraph);
	
	UFUNCTION(BlueprintNativeEvent)
	void OnParagraphFinish(const FNipcatDialogueParagraph& Paragraph, int32 SelectedOptionIndex);
	
	UFUNCTION()
	void OnDialogueStart(UNipcatDialogueFlowAsset* FlowAsset);
	
	UFUNCTION()
	void OnDialogueFinish(UNipcatDialogueFlowAsset* FlowAsset);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnDialogueStart")
	void BP_OnDialogueStart(UNipcatDialogueFlowAsset* FlowAsset);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="OnDialogueFinish")
	void BP_OnDialogueFinish(UNipcatDialogueFlowAsset* FlowAsset);
	
	// --- Internal ---
	
	void FinishParagraph(int32 SelectedOptionIndex = -1);
	
	virtual UWorld* GetWorld() const override;
	
protected:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UNipcatDialogueFlowAsset* CurrentFlowAsset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UNipcatDialogueFlowAsset* LastFlowAsset;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FNipcatDialogueParagraph CurrentParagraph;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CurrentSentenceIndex;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FNipcatDialogueFinishDelegate CurrentParagraphFinishDelegate;
};
