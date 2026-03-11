// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NipcatBasicGameplayEditorFunctionLibrary.generated.h"

class UIKRetargeter;
/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAYEDITOR_API UNipcatBasicGameplayEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Editor", CallInEditor, meta=(CallableWithoutWorldContext, DeterminesOutputType = "InComponentClass"))
	static UActorComponent* FindDefaultComponentByClass(const TSubclassOf<AActor> InActorClass, const TSubclassOf<UActorComponent> InComponentClass);
	
	template <typename  T>
	T* FindDefaultComponentByClass(const TSubclassOf<T> InActorClass) const
	{
		return (T*)FindDefaultComponentByClass(InActorClass, T::StaticClass());
	}
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Editor", CallInEditor, meta=(CallableWithoutWorldContext))
	static void SetAnimBlueprintSkeleton(UObject* AnimBlueprintAsset, USkeleton* InSkeleton);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Editor", CallInEditor, meta=(CallableWithoutWorldContext, ScriptMethod))
	static void SetAllowCpuAccessForStaticMesh(UStaticMesh* StaticMesh, bool NewAllow = true);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Editor", CallInEditor, meta=(CallableWithoutWorldContext, ScriptMethod))
	static void SetAllowCpuAccessForSkeletalMesh(USkeletalMesh* SkeletalMesh, bool NewAllow = true, int32 MaxLOD = 0);
	
	UFUNCTION(BlueprintCallable, Category= "Nipcat Basic Gameplay Function Library|Editor", CallInEditor, meta=(CallableWithoutWorldContext))
	static void AdvancedDuplicateAndRetarget(
		const TArray<FAssetData>& AssetsToRetarget,
		USkeletalMesh* SourceMesh,
		USkeletalMesh* TargetMesh,
		UIKRetargeter* IKRetargetAsset,
		const FString& Search = "",
		const FString& Replace = "",
		const FString& Prefix = "",
		const FString& Suffix = "",
		const FString& FolderPath = "",
		const bool bOverwriteExistingFiles=true,
		const bool bIncludeReferencedAssets=true);
};
