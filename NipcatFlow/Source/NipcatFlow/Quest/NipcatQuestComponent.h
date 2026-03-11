// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatQuestInstance.h"
#include "Components/ActorComponent.h"
#include "NipcatQuestComponent.generated.h"


class UNipcatQuestSubsystem;
class UFlowSubsystem;
class UFlowAsset;

UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent))
class NIPCATFLOW_API UNipcatQuestComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame)
	TArray<TObjectPtr<UNipcatQuestInstance>> QuestInstances;

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="Tags"))
	void SetQuestStateByData(UNipcatQuestData* QuestData, const FGameplayTagContainer& Tags, bool bOverride = false);

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="QuestID,Tags"))
	void SetQuestStateByID(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID, const FGameplayTagContainer& Tags, bool bOverride = false);

	UFUNCTION(BlueprintPure)
	FGameplayTagContainer GetQuestStateByData(UNipcatQuestData* QuestData) const;

	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="QuestID"))
	FGameplayTagContainer GetQuestStateByID(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID) const;

	UFUNCTION(BlueprintCallable)
	void AddQuest(UNipcatQuestData* QuestData, bool bAutoAccept = false);

	UFUNCTION(BlueprintCallable)
	void AcceptQuest(UNipcatQuestData* QuestData);

	UFUNCTION(BlueprintCallable)
	void FinishQuest(UNipcatQuestData* QuestData);
	
	UFUNCTION(BlueprintPure)
	UFlowAsset* FindFlowInstanceByData(const UNipcatQuestData* QuestData) const;
	
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="QuestID"))
	UFlowAsset* FindFlowInstanceByID(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID) const;
	
	UFUNCTION(BlueprintPure)
	UNipcatQuestInstance* FindQuestInstanceByData(UNipcatQuestData* QuestData) const;
	
	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="QuestID"))
	UNipcatQuestInstance* FindQuestInstanceByID(UPARAM(meta=(AllowedTypes="NipcatQuestData")) const FPrimaryAssetId& QuestID) const;
	
	UFlowSubsystem* GetFlowSubsystem() const;
	
	UNipcatQuestSubsystem* GetQuestSubsystem() const;
};
