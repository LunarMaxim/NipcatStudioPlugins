// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"

#include "NipcatFlowTypes.generated.h"

namespace NipcatFlowTags
{
	NIPCATFLOW_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Flow_ComponentType_Default);
	NIPCATFLOW_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Flow_ComponentType_Dialogue);
}

/**
 * 
 */
USTRUCT(BlueprintType)
struct FNipcatFlowIdentityInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer AddedTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTagContainer RemovedTags;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Team"))
	bool bOverrideTeam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Team", EditCondition="bOverrideTeam"))
	FGameplayTag TeamTag;

	FNipcatFlowIdentityInfo& operator+=(const FNipcatFlowIdentityInfo& Other)
	{
		TSet<FGameplayTag> OldAddedTagSet;
		TSet<FGameplayTag> OldRemovedTagSet;
		TSet<FGameplayTag> NewAddedTagSet;
		TSet<FGameplayTag> NewRemovedTagSet;
		
		OldAddedTagSet.Append(AddedTags.GetGameplayTagArray());
		OldRemovedTagSet.Append(RemovedTags.GetGameplayTagArray());
		NewAddedTagSet.Append(Other.AddedTags.GetGameplayTagArray());
		NewRemovedTagSet.Append(Other.RemovedTags.GetGameplayTagArray());
		
		RemovedTags.RemoveTags(FGameplayTagContainer::CreateFromArray(NewAddedTagSet.Intersect(OldRemovedTagSet).Array()));
		AddedTags.AppendTags(FGameplayTagContainer::CreateFromArray(NewAddedTagSet.Difference(OldRemovedTagSet).Array()));
		
		AddedTags.RemoveTags(FGameplayTagContainer::CreateFromArray(NewRemovedTagSet.Intersect(OldAddedTagSet).Array()));
		RemovedTags.AppendTags(FGameplayTagContainer::CreateFromArray(NewRemovedTagSet.Difference(OldAddedTagSet).Array()));
		
		if (Other.bOverrideTeam)
		{
			TeamTag = Other.TeamTag;
		}
		return *this;
	}
	
	void Reset()
	{
		AddedTags.Reset();
		RemovedTags.Reset();
		TeamTag = FGameplayTag();
	}
};


USTRUCT(BlueprintType)
struct FNipcatFlowIdentityInfoModifier
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bUseGuid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="bUseGuid", EditConditionHides))
	FGuid ComponentGuid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(Categories="Flow.Identity", EditCondition="!bUseGuid", EditConditionHides))
	FGameplayTag IdentityTag;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition="!bUseGuid", EditConditionHides))
	bool bExactMatch;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FNipcatFlowIdentityInfo IdentityInfo;
	
	// 记录是谁注册的 (Flow Node)，以便注销时只删除自己的
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TWeakObjectPtr<UObject> SourceObject; 
};

USTRUCT(BlueprintType)
struct FNipcatFlowIdentityInfoModifierArray
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FNipcatFlowIdentityInfoModifier> Array;
};

USTRUCT(BlueprintType)
struct FNipcatDialogueSentence
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine))
	FText Content;
	
	// TODO:
	// Sounds
	// Emotion
};


USTRUCT(BlueprintType)
struct FNipcatDialogueOption
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText OptionText;
	
	// TODO: Conditions
	// UI Style
};

USTRUCT(BlueprintType)
struct FNipcatDialogueOptionList : public FTableRowBase
{
	GENERATED_BODY()
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGuid OptionGuid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty="OptionText"))
	TArray<FNipcatDialogueOption> OptionList;
	
};


USTRUCT(BlueprintType)
struct FNipcatDialogueOptionID : public FTableRowBase
{
	GENERATED_BODY()
	
	FNipcatDialogueOptionID(){}
	
	FNipcatDialogueOptionID(FGuid InGuid, FName InRowName)
	{
		OptionGuid = InGuid;
		RowName = InRowName;
	}
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	FGuid OptionGuid;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty="OptionText"))
	FName RowName;
	
	bool operator==(const FNipcatDialogueOptionID& Other) const
	{
		return OptionGuid == Other.OptionGuid && RowName == Other.RowName;
	}

	bool IsValid() const
	{
		return OptionGuid.IsValid() && RowName != NAME_None;
	}
	
	friend uint32 GetTypeHash(const FNipcatDialogueOptionID& ID)
	{
		return HashCombine(GetTypeHash(ID.OptionGuid), GetTypeHash(ID.RowName));
	}
};

USTRUCT(BlueprintType)
struct FNipcatDialogueParagraph
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SpeakerName;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(TitleProperty="Content"))
	TArray<FNipcatDialogueSentence> Sentences;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(MultiLine, ShowOnlyInnerProperties))
	FNipcatDialogueOptionList Options;
	
};