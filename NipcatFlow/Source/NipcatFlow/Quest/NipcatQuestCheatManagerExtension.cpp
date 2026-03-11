// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatQuestCheatManagerExtension.h"

#include "GameplayTagContainer.h"
#include "NipcatQuestSubsystem.h"

void UNipcatQuestCheatManagerExtension::GM_AddQuest(const FPrimaryAssetId& QuestID, bool AutoStart)
{
	if (const auto QuestSubsystem = GetQuestSubsystem())
	{
		QuestSubsystem->AddQuest(QuestID, AutoStart);
	}
}

void UNipcatQuestCheatManagerExtension::GM_StartQuest(const FPrimaryAssetId& QuestID)
{
	if (const auto QuestSubsystem = GetQuestSubsystem())
	{
		QuestSubsystem->AcceptQuest(QuestID);
	}
}

void UNipcatQuestCheatManagerExtension::GM_FinishQuest(const FPrimaryAssetId& QuestID)
{
	if (const auto QuestSubsystem = GetQuestSubsystem())
	{
		QuestSubsystem->FinishQuest(QuestID);
	}
}

UNipcatQuestSubsystem* UNipcatQuestCheatManagerExtension::GetQuestSubsystem() const
{
	if (GetWorld() && GetWorld()->GetGameInstance())
	{
		return GetWorld()->GetGameInstance()->GetSubsystem<UNipcatQuestSubsystem>();
	}

	return nullptr;
}
