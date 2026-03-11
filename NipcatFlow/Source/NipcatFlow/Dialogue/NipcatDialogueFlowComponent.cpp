// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatDialogueFlowComponent.h"

#include "NipcatDialogueSubsystem.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"


UNipcatDialogueFlowComponent::UNipcatDialogueFlowComponent()
{
	bAutoStartRootFlow = false;
}

int32 UNipcatDialogueFlowComponent::FindOptionResult(const FDataTableRowHandle& OptionIDHandle) const
{
	if (const auto OptionID = UNipcatDialogueSubsystem::GetOptionID(OptionIDHandle); OptionID.IsValid())
	{
		if (const auto Result = OptionResult.Find(OptionID))
		{
			return *Result;
		}
	}
	return INDEX_NONE;
}

void UNipcatDialogueFlowComponent::RegisterOptionResult(const FDataTableRowHandle& OptionIDHandle, int32 Index)
{
	if (const auto OptionID = UNipcatDialogueSubsystem::GetOptionID(OptionIDHandle); OptionID.IsValid())
	{
		OptionResult.Add(OptionID, Index);
		if (const auto DialogueSubsystem = GetDialogueSubsystem())
		{
			DialogueSubsystem->OnOptionResultRegistered.Broadcast(OptionIDHandle, Index);
		}
	}
}

void UNipcatDialogueFlowComponent::UnregisterOptionResult(const FDataTableRowHandle& OptionIDHandle)
{
	if (const auto OptionID = UNipcatDialogueSubsystem::GetOptionID(OptionIDHandle); OptionID.IsValid())
	{
		OptionResult.Remove(OptionID);
	}
}

UNipcatDialogueSubsystem* UNipcatDialogueFlowComponent::GetDialogueSubsystem() const
{
	if (const auto PC = UNipcatBasicGameplayFunctionLibrary::GetPlayerControllerFromActor(GetOwner()))
	{
		if (PC->GetLocalPlayer())
		{
			return PC->GetLocalPlayer()->GetSubsystem<UNipcatDialogueSubsystem>();
		}
	}
	return nullptr;
}



