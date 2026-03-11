// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatDialogueFlowAsset.h"

#include "NipcatDialogueSubsystem.h"

void UNipcatDialogueFlowAsset::OnFlowStart()
{
	if (const auto Subsystem = GetDialogueSubsystem())
	{
		Subsystem->StartDialogue(this);
	}
	Super::OnFlowStart();
}

void UNipcatDialogueFlowAsset::OnFlowFinish()
{
	Super::OnFlowFinish();
	if (const auto Subsystem = GetDialogueSubsystem())
	{
		Subsystem->FinishDialogue(this);
	}
}

UNipcatDialogueSubsystem* UNipcatDialogueFlowAsset::GetDialogueSubsystem() const
{
	if (GetWorld() && GetWorld()->GetFirstLocalPlayerFromController())
	{
		return GetWorld()->GetFirstLocalPlayerFromController()->GetSubsystem<UNipcatDialogueSubsystem>();
	}

	return nullptr;
}
