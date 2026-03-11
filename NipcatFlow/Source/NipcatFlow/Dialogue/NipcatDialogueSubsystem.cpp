// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatDialogueSubsystem.h"

#include "NipcatDialogueFlowComponent.h"
#include "NipcatFlow/NipcatFlowComponentOwnerInterface.h"

UNipcatDialogueFlowComponent* UNipcatDialogueSubsystem::GetLocalPlayerDialogueFlowComponent() const
{
	if (const auto LocalPlayer = GetLocalPlayer())
	{
		if (LocalPlayer->PlayerController)
		{
			if (const auto Pawn = LocalPlayer->PlayerController->GetPawn())
			{
				if (Pawn->Implements<UNipcatFlowComponentOwnerInterface>())
				{
					if (const auto Comp = INipcatFlowComponentOwnerInterface::Execute_GetNipcatFlowComponent(Pawn, NipcatFlowTags::Flow_ComponentType_Dialogue))
					{
						return Cast<UNipcatDialogueFlowComponent>(Comp);
					}
				}
			}
		}
	}
	return nullptr;
}

void UNipcatDialogueSubsystem::StartDialogue(UNipcatDialogueFlowAsset* FlowAsset)
{
	OnDialogueStart(FlowAsset);
}

void UNipcatDialogueSubsystem::FinishDialogue(UNipcatDialogueFlowAsset* FlowAsset)
{
	OnDialogueFinish(FlowAsset);
}

void UNipcatDialogueSubsystem::PushParagraph(FNipcatDialogueParagraph Paragraph, FNipcatDialogueFinishDelegate OnFinish)
{
	CurrentParagraph = Paragraph;
	CurrentParagraphFinishDelegate = OnFinish;
	CurrentSentenceIndex = -1;
	OnParagraphUpdate(CurrentParagraph);
	Continue();
}

int32 UNipcatDialogueSubsystem::FindOptionResult(const FDataTableRowHandle& OptionIDHandle) const
{
	if (const auto Component = GetLocalPlayerDialogueFlowComponent())
	{
		return Component->FindOptionResult(OptionIDHandle);
	}
	return INDEX_NONE;
}

void UNipcatDialogueSubsystem::RegisterOptionResult(const FDataTableRowHandle& OptionIDHandle, int32 Index)
{
	if (const auto Component = GetLocalPlayerDialogueFlowComponent())
	{
		Component->RegisterOptionResult(OptionIDHandle, Index);
	}
}

void UNipcatDialogueSubsystem::UnregisterOptionResult(const FDataTableRowHandle& OptionIDHandle)
{
	if (const auto Component = GetLocalPlayerDialogueFlowComponent())
	{
		Component->UnregisterOptionResult(OptionIDHandle);
	}
}

FNipcatDialogueOptionID UNipcatDialogueSubsystem::GetOptionID(const FDataTableRowHandle& OptionIDHandle)
{
	if (const auto OptionList = OptionIDHandle.GetRow<FNipcatDialogueOptionList>(TEXT("")))
	{
		return FNipcatDialogueOptionID(OptionList->OptionGuid, OptionIDHandle.RowName);
	}
	return FNipcatDialogueOptionID();
}

void UNipcatDialogueSubsystem::Continue()
{
	if (CurrentSentenceIndex < CurrentParagraph.Sentences.Num())
	{
		CurrentSentenceIndex++;
		if (CurrentParagraph.Sentences.IsValidIndex(CurrentSentenceIndex))
		{
			OnSentenceUpdate(CurrentParagraph.Sentences[CurrentSentenceIndex], CurrentSentenceIndex);
		}
		else if (CurrentSentenceIndex == CurrentParagraph.Sentences.Num())
		{
			if (CurrentParagraph.Options.OptionList.IsEmpty())
			{
				FinishParagraph();
			}
			else
			{
				ShowOptions(CurrentParagraph.Options.OptionList);
			}
		}
	}
}

void UNipcatDialogueSubsystem::Skip()
{
	// TODO
}

void UNipcatDialogueSubsystem::SelectOption(int32 Index)
{
	FinishParagraph(Index);
}

void UNipcatDialogueSubsystem::OnSentenceUpdate_Implementation(const FNipcatDialogueSentence& Sentence, int32 Index)
{
}

void UNipcatDialogueSubsystem::ShowOptions_Implementation(const TArray<FNipcatDialogueOption>& Options)
{
	
}

void UNipcatDialogueSubsystem::OnParagraphUpdate_Implementation(const FNipcatDialogueParagraph& Paragraph)
{
}

void UNipcatDialogueSubsystem::OnParagraphFinish_Implementation(const FNipcatDialogueParagraph& Paragraph,
	int32 SelectedOptionIndex)
{
}


void UNipcatDialogueSubsystem::OnDialogueStart(UNipcatDialogueFlowAsset* FlowAsset)
{
	CurrentFlowAsset = FlowAsset;
	LastFlowAsset = FlowAsset;
	BP_OnDialogueStart(FlowAsset);
}

void UNipcatDialogueSubsystem::OnDialogueFinish(UNipcatDialogueFlowAsset* FlowAsset)
{
	BP_OnDialogueFinish(FlowAsset);
	CurrentFlowAsset = nullptr;
}

void UNipcatDialogueSubsystem::FinishParagraph(int32 SelectedOptionIndex)
{
	CurrentParagraphFinishDelegate.ExecuteIfBound(SelectedOptionIndex);
	OnParagraphFinish(CurrentParagraph, SelectedOptionIndex);
}

UWorld* UNipcatDialogueSubsystem::GetWorld() const
{
	if (GetLocalPlayer())
	{
		return GetLocalPlayer()->GetWorld();
	}
	return nullptr;
}

