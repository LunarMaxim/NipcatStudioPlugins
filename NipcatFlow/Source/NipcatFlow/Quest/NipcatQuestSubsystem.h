// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NipcatQuestInstance.h"

#include "NipcatQuestSubsystem.generated.h"

class UNipcatQuestComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnQuestProgressChanged, UNipcatQuestData*, QuestData, ENipcatQuestProgress,
                                               NewProgress, UNipcatQuestInstance*, Instance);

class UNipcatQuestData;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class NIPCATFLOW_API UNipcatQuestSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void Deinitialize() override;
	
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="QuestID"))
	static UNipcatQuestData* GetQuestDataFromID(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID);

	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="QuestID"))
	UNipcatQuestInstance* FindQuestInstance(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID) const;

	// 当组件里的任务状态改变时，调用这个函数，让 Subsystem 转发给 UI
	UFUNCTION(BlueprintCallable)
	void NotifyQuestProgressChanged(UNipcatQuestData* QuestData, ENipcatQuestProgress NewProgress, UNipcatQuestInstance* QuestInstance);

	// 供 UI 绑定的委托
	UPROPERTY(BlueprintAssignable)
	FOnQuestProgressChanged OnQuestProgressChanged;
    
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="QuestID"))
	FGameplayTagContainer GetQuestState(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID) const;

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="QuestID,Tags"))
	void SetQuestState(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID, const FGameplayTagContainer& Tags, bool bOverride = false);

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="QuestID"))
	void AddQuest(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID, bool bAutoAccept = false);
    
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="QuestID"))
	void AcceptQuest(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID);
    
	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="QuestID"))
	void FinishQuest(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID);

	UFUNCTION(BlueprintPure)
	UNipcatQuestComponent* GetFirstPlayerQuestComponent() const;

	void OnCheatManagerCreate(UCheatManager* CheatManager);
	
protected:
	
	FDelegateHandle CheatManagerCreateHandle;
	
};
