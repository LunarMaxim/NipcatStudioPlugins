// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatQuestComponent.h"

#include "FlowSubsystem.h"
#include "NipcatQuestData.h"
#include "NipcatQuestFlowAsset.h"
#include "NipcatQuestSubsystem.h"
#include "Asset/FlowAssetParamsTypes.h"
#include "Asset/FlowAssetParams.h"
#include "NipcatFlow/NipcatFlowFunctionLibrary.h"
#include "Nodes/Graph/FlowNode_CustomInput.h"


void UNipcatQuestComponent::SetQuestStateByData(UNipcatQuestData* QuestData, const FGameplayTagContainer& Tags,
                                                bool bOverride)
{
	if (auto QuestInstance = FindQuestInstanceByData(QuestData))
	{
		if (bOverride)
		{
			QuestInstance->QuestState = Tags;
		}
		else
		{
			QuestInstance->QuestState.AppendTags(Tags);
		}
	}
}

void UNipcatQuestComponent::SetQuestStateByID(const FPrimaryAssetId& QuestID, const FGameplayTagContainer& Tags,
                                              bool bOverride)
{
	if (auto QuestInstance = FindQuestInstanceByID(QuestID))
	{
		if (bOverride)
		{
			QuestInstance->QuestState = Tags;
		}
		else
		{
			QuestInstance->QuestState.AppendTags(Tags);
		}
	}
}

FGameplayTagContainer UNipcatQuestComponent::GetQuestStateByData(UNipcatQuestData* QuestData) const
{
	if (auto QuestInstance = FindQuestInstanceByData(QuestData))
	{
		return QuestInstance->QuestState;
	}
	return FGameplayTagContainer();
}

FGameplayTagContainer UNipcatQuestComponent::GetQuestStateByID(const FPrimaryAssetId& QuestID) const
{
	if (auto QuestInstance = FindQuestInstanceByID(QuestID))
	{
		return QuestInstance->QuestState;
	}
	return FGameplayTagContainer();
}

void UNipcatQuestComponent::AddQuest(UNipcatQuestData* QuestData, bool bAutoAccept)
{
	if (!QuestData || QuestData->QuestFlow.IsNull()) return;

	if (FindQuestInstanceByData(QuestData))
	{
		return;
	}
	
	if (UFlowSubsystem* FlowSys = GetFlowSubsystem())
	{
		const TScriptInterface<IFlowDataPinValueSupplierInterface> RootFlowParamsAsInterface = QuestData->FlowParams.ResolveFlowAssetParams();

		if (const auto FlowTemplate = QuestData->QuestFlow.LoadSynchronous())
		{
			if (UFlowAsset* NewFlow = FlowSys->CreateRootFlow(GetOwner(), FlowTemplate, true))
			{
				NewFlow->StartFlow(RootFlowParamsAsInterface.GetInterface());
				
				auto QuestInstance = NewObject<UNipcatQuestInstance>();
				QuestInstance->QuestData = QuestData;
				QuestInstance->QuestProgress = bAutoAccept ? ENipcatQuestProgress::InProgress : ENipcatQuestProgress::NotStarted;
				QuestInstances.Add(QuestInstance);
				if (bAutoAccept)
				{
					NewFlow->TriggerCustomInput(NipcatQuestFlowCustomInputNames::OnQuestAccepted);
				}
			}
		}
	}
}

void UNipcatQuestComponent::AcceptQuest(UNipcatQuestData* QuestData)
{
	if (auto QuestInstance = FindQuestInstanceByData(QuestData))
	{
		if (QuestInstance->QuestProgress == ENipcatQuestProgress::NotStarted)
		{
			QuestInstance->QuestProgress = ENipcatQuestProgress::InProgress;
			if (auto QuestSubsystem = GetQuestSubsystem())
			{
				QuestSubsystem->NotifyQuestProgressChanged(QuestData, ENipcatQuestProgress::InProgress, QuestInstance);
			}
			if (const auto Flow = FindFlowInstanceByData(QuestData))
			{
				Flow->TriggerCustomInput(NipcatQuestFlowCustomInputNames::OnQuestAccepted);
			}
		}
	}
}

void UNipcatQuestComponent::FinishQuest(UNipcatQuestData* QuestData)
{
	if (auto QuestInstance = FindQuestInstanceByData(QuestData))
	{
		if (QuestInstance->QuestProgress != ENipcatQuestProgress::Completed)
		{
			QuestInstance->QuestProgress = ENipcatQuestProgress::Completed;
			if (auto QuestSubsystem = GetQuestSubsystem())
			{
				QuestSubsystem->NotifyQuestProgressChanged(QuestData, ENipcatQuestProgress::Completed, QuestInstance);
			}
			if (const auto Flow = FindFlowInstanceByData(QuestData))
			{
				bool HasQuestFinishNode = false;
				if (const auto OnQuestFinishNode = Flow->TryFindCustomInputNodeByEventName(NipcatQuestFlowCustomInputNames::OnQuestFinish))
				{
					if (OnQuestFinishNode->IsOutputConnected(OnQuestFinishNode->GetOutputPins()[0]))
					{
						if (UNipcatFlowFunctionLibrary::GetSignalMode(OnQuestFinishNode) == EFlowSignalMode::Enabled)
						{
							HasQuestFinishNode = true;
							Flow->TriggerCustomInput(NipcatQuestFlowCustomInputNames::OnQuestFinish);
						}
					}
				}
				if (!HasQuestFinishNode)
				{
					Flow->FinishFlow(EFlowFinishPolicy::Keep);
				}
			}
		}
	}
}

UFlowAsset* UNipcatQuestComponent::FindFlowInstanceByData(const UNipcatQuestData* QuestData) const
{
	if (!QuestData || QuestData->QuestFlow.IsNull()) return nullptr;
	
	for (auto& FlowAsset : QuestData->QuestFlow.LoadSynchronous()->GetActiveInstances())
	{
		if (FlowAsset->GetOwner() == GetOwner())
		{
			return FlowAsset;
		}
	}

	return nullptr;
}

UFlowAsset* UNipcatQuestComponent::FindFlowInstanceByID(const FPrimaryAssetId& QuestID) const
{
	if (const UFlowSubsystem* FlowSubsystem = GetFlowSubsystem())
	{
		if (const auto QuestData = UNipcatQuestSubsystem::GetQuestDataFromID(QuestID))
		{
			const TSet<UFlowAsset*> Result = FlowSubsystem->GetRootInstancesByOwner(this);
			for (auto& FlowAsset : Result)
			{
				if (FlowAsset == QuestData->QuestFlow.LoadSynchronous())
				{
					return FlowAsset;
				}
			}
		}
	}

	return nullptr;
}

UNipcatQuestInstance* UNipcatQuestComponent::FindQuestInstanceByData(UNipcatQuestData* QuestData) const
{
	for (auto& QuestInstance : QuestInstances)
	{
		if (QuestInstance->QuestData == QuestData)
		{
			return QuestInstance;
		}
	}
	return nullptr;
}

UNipcatQuestInstance* UNipcatQuestComponent::FindQuestInstanceByID(const FPrimaryAssetId& QuestID) const
{
	if (const auto QuestData = UNipcatQuestSubsystem::GetQuestDataFromID(QuestID))
	{
		return FindQuestInstanceByData(QuestData);
	}
	return nullptr;
}

UFlowSubsystem* UNipcatQuestComponent::GetFlowSubsystem() const
{
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		return GetWorld()->GetGameInstance()->GetSubsystem<UFlowSubsystem>();
	}

	return nullptr;
}

UNipcatQuestSubsystem* UNipcatQuestComponent::GetQuestSubsystem() const
{
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		return GetWorld()->GetGameInstance()->GetSubsystem<UNipcatQuestSubsystem>();
	}

	return nullptr;
}
