// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "NipcatQuestCheatManagerExtension.generated.h"

class UNipcatQuestSubsystem;
struct FGameplayTag;
/**
 * 
 */
UCLASS()
class NIPCATFLOW_API UNipcatQuestCheatManagerExtension : public UCheatManagerExtension
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Exec)
	void GM_AddQuest(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID, bool AutoStart = false);
	
	UFUNCTION(Exec)
	void GM_StartQuest(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID);
	
	UFUNCTION(Exec)
	void GM_FinishQuest(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID);
	
	UNipcatQuestSubsystem* GetQuestSubsystem() const;
};
