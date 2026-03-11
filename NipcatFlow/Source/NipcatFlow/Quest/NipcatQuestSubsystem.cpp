// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatQuestSubsystem.h"

#include "NipcatQuestCheatManagerExtension.h"
#include "NipcatQuestComponent.h"
#include "NipcatQuestComponentOwnerInterface.h"
#include "NipcatQuestData.h"
#include "Engine/AssetManager.h"
#include "GameFramework/CheatManager.h"

void UNipcatQuestSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

#if UE_WITH_CHEAT_MANAGER
	CheatManagerCreateHandle = UCheatManager::RegisterForOnCheatManagerCreated(
			FOnCheatManagerCreated::FDelegate::CreateUObject(this, &ThisClass::OnCheatManagerCreate));
#endif
}

void UNipcatQuestSubsystem::Deinitialize()
{
	Super::Deinitialize();
	
#if UE_WITH_CHEAT_MANAGER
	if (CheatManagerCreateHandle.IsValid())
	{
		UCheatManager::UnregisterFromOnCheatManagerCreated(CheatManagerCreateHandle);
	}
#endif
	
}

UNipcatQuestData* UNipcatQuestSubsystem::GetQuestDataFromID(const FPrimaryAssetId& QuestID)
{
	if (QuestID.IsValid())
	{
		auto& AssetManager = UAssetManager::Get();
		
		if (const auto LoadedObject = AssetManager.GetPrimaryAssetPath(QuestID).TryLoad())
		{
			if (const auto QuestData = Cast<UNipcatQuestData>(LoadedObject))
			{
				return QuestData;
			}
		}
	}
	return nullptr;
}

UNipcatQuestInstance* UNipcatQuestSubsystem::FindQuestInstance(const FPrimaryAssetId& QuestID) const
{
	if (const auto QuestComponent = GetFirstPlayerQuestComponent())
	{
		if (const auto QuestData = GetQuestDataFromID(QuestID))
		{
			return QuestComponent->FindQuestInstanceByData(QuestData);
		}
	}
	return nullptr;
}

void UNipcatQuestSubsystem::NotifyQuestProgressChanged(UNipcatQuestData* QuestData, ENipcatQuestProgress NewProgress,
	UNipcatQuestInstance* QuestInstance)
{
	OnQuestProgressChanged.Broadcast(QuestData, NewProgress, QuestInstance);
}


FGameplayTagContainer UNipcatQuestSubsystem::GetQuestState(const FPrimaryAssetId& QuestID) const
{
	if (const auto QuestComponent = GetFirstPlayerQuestComponent())
	{
		if (const auto QuestData = GetQuestDataFromID(QuestID))
		{
			return QuestComponent->GetQuestStateByData(QuestData);
		}
	}
	return FGameplayTagContainer();
}

void UNipcatQuestSubsystem::SetQuestState(const FPrimaryAssetId& QuestID, const FGameplayTagContainer& Tags, bool bOverride)
{
	if (const auto QuestComponent = GetFirstPlayerQuestComponent())
	{
		if (const auto QuestData = GetQuestDataFromID(QuestID))
		{
			QuestComponent->SetQuestStateByData(QuestData, Tags, bOverride);
		}
	}
}

void UNipcatQuestSubsystem::AddQuest(const FPrimaryAssetId& QuestID, bool bAutoAccept)
{
	if (const auto QuestComponent = GetFirstPlayerQuestComponent())
	{
		if (const auto QuestData = GetQuestDataFromID(QuestID))
		{
			QuestComponent->AddQuest(QuestData, bAutoAccept);
		}
	}
}

void UNipcatQuestSubsystem::AcceptQuest(const FPrimaryAssetId& QuestID)
{
	if (const auto QuestComponent = GetFirstPlayerQuestComponent())
	{
		if (const auto QuestData = GetQuestDataFromID(QuestID))
		{
			QuestComponent->AcceptQuest(QuestData);
		}
	}
}

void UNipcatQuestSubsystem::FinishQuest(const FPrimaryAssetId& QuestID)
{
	if (const auto QuestComponent = GetFirstPlayerQuestComponent())
	{
		if (const auto QuestData = GetQuestDataFromID(QuestID))
		{
			QuestComponent->FinishQuest(QuestData);
		}
	}
}

UNipcatQuestComponent* UNipcatQuestSubsystem::GetFirstPlayerQuestComponent() const
{
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (const auto Pawn = PC->GetPawn())
		{
			if (const auto QuestComponent = INipcatQuestComponentOwnerInterface::Execute_GetNipcatQuestComponent(Pawn))
			{
				return QuestComponent;
			}
		}
	}
	return nullptr;
}

void UNipcatQuestSubsystem::OnCheatManagerCreate(UCheatManager* CheatManager)
{
	CheatManager->AddCheatManagerExtension(NewObject<UCheatManagerExtension>(CheatManager, UNipcatQuestCheatManagerExtension::StaticClass()));
}
