// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatBasicGameplayEditorFunctionLibrary.h"

#include "EditorScriptingHelpers.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "Engine/SimpleConstructionScript.h"
#include "Engine/SCS_Node.h"
#include "StaticMeshEditorSubsystem.h"
#include "Engine/SkinnedAssetCommon.h"
#include "RetargetEditor/IKRetargetBatchOperation.h"


UActorComponent* UNipcatBasicGameplayEditorFunctionLibrary::FindDefaultComponentByClass(
	const TSubclassOf<AActor> InActorClass, const TSubclassOf<UActorComponent> InComponentClass)
{
	if (!IsValid(InActorClass))
	{
		return nullptr;
	}

	// Check CDO.
	AActor* ActorCDO = InActorClass->GetDefaultObject<AActor>();
	UActorComponent* FoundComponent = ActorCDO->FindComponentByClass(InComponentClass);

	if (FoundComponent != nullptr)
	{
		return FoundComponent;
	}

	// Check blueprint nodes. Components added in blueprint editor only (and not in code) are not available from
	// CDO.
	UBlueprintGeneratedClass* RootBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(InActorClass);
	UClass* ActorClass = InActorClass;

	// Go down the inheritance tree to find nodes that were added to parent blueprints of our blueprint graph.
	do
	{
		const UBlueprintGeneratedClass* ActorBlueprintGeneratedClass = Cast<UBlueprintGeneratedClass>(ActorClass);
		if (!ActorBlueprintGeneratedClass)
		{
			return nullptr;
		}

		if (ActorBlueprintGeneratedClass->SimpleConstructionScript)
		{
			const TArray<USCS_Node*>& ActorBlueprintNodes =
				ActorBlueprintGeneratedClass->SimpleConstructionScript->GetAllNodes();

			for (const USCS_Node* Node : ActorBlueprintNodes)
			{
				if (Node->ComponentClass->IsChildOf(InComponentClass))
				{
					return Node->GetActualComponentTemplate(RootBlueprintGeneratedClass);
				}
			}
		}

		ActorClass = Cast<UClass>(ActorClass->GetSuperStruct());

	} while (ActorClass != AActor::StaticClass());

	return nullptr;
}

void UNipcatBasicGameplayEditorFunctionLibrary::SetAnimBlueprintSkeleton(UObject* AnimBlueprintAsset, USkeleton* InSkeleton)
{
	if (UAnimBlueprint* AnimBlueprint = Cast<UAnimBlueprint>(AnimBlueprintAsset))
	{
		if (InSkeleton)
		{
			AnimBlueprint->TargetSkeleton = InSkeleton;
		}
	}
}

void UNipcatBasicGameplayEditorFunctionLibrary::SetAllowCpuAccessForStaticMesh(UStaticMesh* StaticMesh, bool NewAllow)
{
	if (StaticMesh && StaticMesh->bAllowCPUAccess != NewAllow)
	{
		if (GEditor)
		{
			if (const auto SubSystem = GEditor->GetEditorSubsystem<UStaticMeshEditorSubsystem>())
			{
				SubSystem->SetAllowCPUAccess(StaticMesh, NewAllow);
			}
		}
	}
}

void UNipcatBasicGameplayEditorFunctionLibrary::SetAllowCpuAccessForSkeletalMesh(USkeletalMesh* SkeletalMesh, bool NewAllow, int32 MaxLOD)
{
	TGuardValue<bool> UnattendedScriptGuard(GIsRunningUnattendedScript, true);

	if (!EditorScriptingHelpers::CheckIfInEditorAndPIE())
	{
		return;
	}

	if (SkeletalMesh)
	{
		for (int32 i = 0; i <= MaxLOD; ++i)
		{
			if (FSkeletalMeshLODInfo* LODInfo = SkeletalMesh->GetLODInfo(i))
			{
				if (LODInfo->bAllowCPUAccess != NewAllow)
				{
					SkeletalMesh->Modify();
					LODInfo->bAllowCPUAccess = NewAllow;
					SkeletalMesh->PostEditChange();
				}
			}
		}
	}
}

void UNipcatBasicGameplayEditorFunctionLibrary::AdvancedDuplicateAndRetarget(
	const TArray<FAssetData>& AssetsToRetarget, USkeletalMesh* SourceMesh, USkeletalMesh* TargetMesh,
	UIKRetargeter* IKRetargetAsset, const FString& Search, const FString& Replace, const FString& Prefix,
	const FString& Suffix, const FString& FolderPath, const bool bOverwriteExistingFiles, const bool bIncludeReferencedAssets)
{
	// fill the context with all the data needed to run a batch retarget
	FIKRetargetBatchOperationContext Context;
	for (const FAssetData& Asset : AssetsToRetarget)
	{
		if (UObject* Object = Cast<UObject>(Asset.GetAsset()))
		{
			Context.AssetsToRetarget.Add(Object); // convert asset data to soft refs
		}
	}
	Context.SourceMesh = SourceMesh;
	Context.TargetMesh = TargetMesh;
	Context.IKRetargetAsset = IKRetargetAsset;
	Context.NameRule.Prefix = Prefix;
	Context.NameRule.Suffix = Suffix;
	Context.NameRule.ReplaceFrom = Search;
	Context.NameRule.ReplaceTo = Replace;
	Context.NameRule.FolderPath = FolderPath;
	Context.bOverwriteExistingFiles = bOverwriteExistingFiles;
	Context.bIncludeReferencedAssets = bIncludeReferencedAssets;

	// actually run the batch operation
	UIKRetargetBatchOperation* BatchOperation = NewObject<UIKRetargetBatchOperation>();
	BatchOperation->AddToRoot();
	BatchOperation->RunRetarget(Context);
	BatchOperation->RemoveFromRoot();
}
