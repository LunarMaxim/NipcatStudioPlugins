// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NipcatBasicGameplayFunctionLibrary.generated.h"

class UCharacterMovementComponent;
class UNipcatCustomMovementMode;
class UPathFollowingComponent;
class UNavMovementComponent;
struct FGameplayTag;
class UCharacterRotationModifier;
struct FScalableFloat;
struct FAnimationPoseData;
/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API UNipcatBasicGameplayFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	
	UFUNCTION(BlueprintCallable, Category="Nipcat Basic Gameplay Function Library|Component", meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Component") )
	static void ForceDestroyComponent(UActorComponent* Component);
	
	UFUNCTION(BlueprintPure, Category="Nipcat Basic Gameplay Function Library|Utilities|Time", meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext) )
	static float GetGlobalTimeDilation(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="Nipcat Basic Gameplay Function Library|Utilities|Time", meta=(WorldContext="WorldContextObject", CallableWithoutWorldContext) )
	static void SetGlobalTimeDilation(const UObject* WorldContextObject, float TimeDilation);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(DeterminesOutputType = "InClass", CallableWithoutWorldContext))
	static const UObject* GetClassDefaultObject(TSubclassOf<UObject> InClass);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library", meta=(DeterminesOutputType = "InClass", CallableWithoutWorldContext))
	static UObject* GetMutableClassDefaultObject(TSubclassOf<UObject> InClass);
	
	static void DeepCopyInstancedObjects(UScriptStruct* ScriptStruct, void* StructMemory, UObject* Outer);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library", meta=(WorldContext="WorldContextObject", UnsafeDuringActorConstruction="true", AdvancedDisplay = "Owner", DeterminesOutputType = "ActorClass"))
	static AActor* SpawnActorDeferred(UObject* WorldContextObject,
		TSubclassOf<AActor> ActorClass,
		FTransform const& Transform,
		AActor* Owner = nullptr,
		APawn* Instigator = nullptr,
		ESpawnActorCollisionHandlingMethod CollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::Undefined,
		ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library", meta=(CallableWithoutWorldContext))
	static void FinishActorSpawning(AActor* Actor, FTransform const& Transform);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Input", meta=(DefaultToSelf = "PlayerController", CallableWithoutWorldContext))
	static void FlushPressedKeys(APlayerController* PlayerController);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Input", meta=(DefaultToSelf = "PlayerController", CallableWithoutWorldContext))
	static void FlushPressedActionBindingKeys(APlayerController* PlayerController, FName ActionName);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Character", meta=(DefaultToSelf = "Character", CallableWithoutWorldContext))
	static void AddRotationModifier(ACharacter* Character, UCharacterRotationModifier* RotationModifier);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Character", meta=(DefaultToSelf = "Character", CallableWithoutWorldContext))
	static void RemoveRotationModifier(ACharacter* Character, UCharacterRotationModifier* RotationModifier);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Curve", meta=(ScriptMethod, CallableWithoutWorldContext))
	static bool FindFloatCurveFromAnimation(const UAnimSequenceBase* AnimSequenceBase, FName CurveName, FFloatCurve& OutFloatCurve);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Curve", meta=(CallableWithoutWorldContext))
	static float GetScaledTimeStep(const FFloatCurve& RateScaleCurve, float StartTime, float InStep, int32 SampleRate = 120);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Curve", meta=(CallableWithoutWorldContext))
	static float GetOriginalTimeStep(const FFloatCurve& RateScaleCurve, float StartTime, float InStep, int32 SampleRate = 120);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Float", meta=(CallableWithoutWorldContext, ScriptMethod))
	static float GetScalableFloatValueAtLevel(const FScalableFloat& ScalableFloat, float Level, const FString& ContextString);
	
	static FCompactPose ExtractAnimMontagePose(const UAnimMontage* TargetMontage, double DesiredAnimTime, FName SlotName,
	                                   UAnimInstance* AnimInstance);
	
	static FTransform GetMeshSpaceTransform(const USkeletalMeshComponent* MeshComp, FName BoneName, FCompactPose Pose);

	static FTransform GetMeshSpaceTransform(const USkeletalMeshComponent* MeshComp, FName BoneName, FPoseSnapshot Pose);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Animation", meta=(CallableWithoutWorldContext))
	static FTransform GetMontageBoneTransform(UAnimMontage* TargetMontage, USkeletalMeshComponent* MeshComp, FName BoneName, float DesiredAnimTime, FName SlotName = FName(TEXT("DefaultSlot")), bool bApplyPostProcessPoseAsAdditive = true, bool bMeshSpaceAdditive = true, bool bShowDebug = false);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Curve", meta = (CallableWithoutWorldContext))
	static FAnimationCurveIdentifier MakeCurveIdentifier(FName Name, ERawCurveTrackTypes CurveType);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Animation", meta = (ScriptMethod, DefaultToSelf="AnimNotify", CallableWithoutWorldContext))
	static UAnimSequenceBase* GetAnimationAssetFromNotify(const UAnimNotify* AnimNotify);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Animation", meta = (ScriptMethod, DefaultToSelf="AnimNotifyState", CallableWithoutWorldContext))
	static UAnimSequenceBase* GetAnimationAssetFromNotifyState(const UAnimNotifyState* AnimNotifyState);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Animation", meta = (ScriptMethod, CallableWithoutWorldContext))
	static USkeleton* GetAnimAssetSkeleton(const UAnimSequenceBase* AnimSequenceBase);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static TArray<FName> GetAttachedObjectSocketNames(const USkeleton* Skeleton);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static UObject* GetPreviewAssetAtSocket(const USkeleton* Skeleton, const FName Socket);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static TMap<FName, UObject*> GetAllPreviewAssets(const USkeleton* Skeleton);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static void AddPreviewAsset(USkeleton* Skeleton, UStaticMesh* StaticMesh, FName Socket);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Animation", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="AnimationAsset"), CallInEditor)
	static void AddPreviewAssetForAnimationAsset(UAnimationAsset* AnimationAsset, UObject* PreviewAsset, FName Socket);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static void RemovePreviewAsset(USkeleton* Skeleton, const UStaticMesh* StaticMesh);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static void RemovePreviewAssetAtSocket(USkeleton* Skeleton, FName Socket);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Animation", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="AnimationAsset"), CallInEditor)
	static void RemovePreviewAssetForAnimation(UAnimationAsset* AnimationAsset, UObject* PreviewAsset, FName Socket);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static void RemoveAllAttachedAssets(USkeleton* Skeleton);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static USkeletalMesh* GetPreviewMesh(USkeleton* Skeleton);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static void SetPreviewMesh(USkeleton* Skeleton, USkeletalMesh* PreviewMesh);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Animation", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="AnimationAsset"), CallInEditor)
	static USkeletalMesh* GetAnimationPreviewMesh(UAnimationAsset* AnimationAsset);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Animation", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="AnimationAsset"), CallInEditor)
	static void SetAnimationPreviewMesh(UAnimationAsset* AnimationAsset, USkeletalMesh* PreviewMesh);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="Skeleton"), CallInEditor)
	static TArray<FName> GetSkeletonSocketNames(USkeleton* Skeleton);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Skeleton", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="MeshComp"), CallInEditor)
	static USceneComponent* GetComponentForAttachedObject(USkeletalMeshComponent* MeshComp, UObject* AttachedObject, const FName AttachName);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Camera Shake", meta=(CallableWithoutWorldContext, ScriptMethod, AutoCreateRefTerm="CameraShakeClass"))
	static float GetCameraShakeDuration(const TSubclassOf<UCameraShakeBase>& CameraShakeClass);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Camera Shake", meta=(CallableWithoutWorldContext, ScriptMethod))
	static void GetCameraShakeBlendTimes(TSubclassOf<UCameraShakeBase> CameraShakeClass, float& OutBlendIn, float& OutBlendOut);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|AI|PathFollowing", meta=(CallableWithoutWorldContext, ScriptMethod))
	static void InitializePathFollowingComponent(UPathFollowingComponent* PathFollowingComponent);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Animation", meta=(CallableWithoutWorldContext))
	static void GetAdditiveRefFrameIndex(const UAnimSequence* AnimationSequence, int32& RefFrameIndex);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Animation", meta=(CallableWithoutWorldContext))
	static void SetAdditiveRefFrameIndex(UAnimSequence* AnimationSequence, const int32 RefFrameIndex);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Animation", meta=(CallableWithoutWorldContext))
	static void GetAdditiveRefPoseSeq(const UAnimSequence* AnimationSequence, UAnimSequence*& RefPoseSeq);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Animation", meta=(CallableWithoutWorldContext))
	static void SetAdditiveRefPoseSeq(UAnimSequence* AnimationSequence, UAnimSequence* RefPoseSeq);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|StaticMesh", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="StaticMesh"), CallInEditor)
	static TArray<FName> GetStaticMeshSocketNames(UStaticMesh* StaticMesh);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|SkeletalMesh", meta=(CallableWithoutWorldContext, ScriptMethod, DefaultToSelf="SkeletalMesh"), CallInEditor)
	static TArray<FName> GetSkeletalMeshSocketNames(USkeletalMesh* SkeletalMesh);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Post Processing", meta=(CallableWithoutWorldContext), CallInEditor)
	static float GetPostProcessVolumeLocalWeight(APostProcessVolume* Volume, FVector& OutViewLocation);

	UFUNCTION(BlueprintCallable, Category="Nipcat Basic Gameplay Function Library|Post Processing", meta=(CallableWithoutWorldContext), CallInEditor)
	static void RemoveInvalidBlendable(APostProcessVolume* PostProcessVolume);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Math", meta=(CallableWithoutWorldContext), CallInEditor)
	static float GetWeightedAverageFloat(const TArray<float>& Items, const TArray<float>& Weights);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Math", meta=(CallableWithoutWorldContext), CallInEditor)
	static FLinearColor GetWeightedAverageLinearColor(const TArray<FLinearColor>& Items, const TArray<float>& Weights);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Math", meta=(CallableWithoutWorldContext), CallInEditor)
	static FLinearColor GetWeightedAverageLinearColorUsingHSV(const TArray<FLinearColor>& Items, const TArray<float>& Weights);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(CallableWithoutWorldContext, DefaultToSelf = "Object"), CallInEditor)
	static TArray<FName> GetModifiedPropertyNames(UObject* Object, TArray<FName> Properties);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(CallableWithoutWorldContext, DefaultToSelf = "Object"), CallInEditor)
	static TArray<FName> GetModifiedPropertyNamesDiffAgainstOther(UObject* Object, UObject* DiffAgainstObject, TArray<FName> Properties);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(CallableWithoutWorldContext, DefaultToSelf = "Object"), CallInEditor)
	static TArray<FName> GetAllInstanceEditableProperties(const UObject* Object);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(CallableWithoutWorldContext, DefaultToSelf = "Object"), CallInEditor)
	static TArray<FName> GetAllInstanceNotEditableProperties(const UObject* Object);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(CallableWithoutWorldContext, DefaultToSelf = "Object"), CallInEditor)
	static TArray<FName> GetAllTransientProperties(const UObject* Object);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Actor", meta=(CallableWithoutWorldContext, DefaultToSelf = "Actor"), CallInEditor)
	static bool HasActorBegunPlay(AActor* Actor);

	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(CallableWithoutWorldContext), CallInEditor)
	static bool IfWithEditor();
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(CallableWithoutWorldContext), CallInEditor)
	static bool IfUEBuildShipping();
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library", meta=(DefaultToSelf="PlayerController"))
	static void SeamlessClientTravel(APlayerController* PlayerController, FString URL);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(DefaultToSelf="Actor", ScriptMethod, CallableWithoutWorldContext))
	static APlayerController* GetPlayerControllerFromActor(AActor* Actor);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(DefaultToSelf="Subsystem", ScriptMethod, CallableWithoutWorldContext))
	static APlayerController* GetPlayerControllerFromLocalPlayerSubsystem(const ULocalPlayerSubsystem* Subsystem);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library", meta=(ScriptMethod, CallableWithoutWorldContext))
	static bool GetPrimaryAssetDataList(FPrimaryAssetType PrimaryAssetType, TArray<FAssetData>& AssetDataList);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(ScriptMethod, CallableWithoutWorldContext))
	static FPrimaryAssetId GetPrimaryAssetIdFromAssetData(const FAssetData& AssetData);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library", meta = (DefaultToSelf="Object", ScriptMethod, CallableWithoutWorldContext), CallInEditor)
	static bool SaveAsset(UObject* Object);
	
	static FName GetValidDataTableRowName(const UDataTable* DataTable);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|DataTable", CustomThunk, meta = (ScriptMethod, CallableWithoutWorldContext, CustomStructureParam="Row"), CallInEditor)
	static FName AddDataTableRow(UDataTable* DataTable, FTableRowBase Row, FName RowName, bool bSave);

	static FName Generic_AddDataTableRow(UDataTable* DataTable, void* Row, FName RowName, bool bSave);
	
	DECLARE_FUNCTION(execAddDataTableRow);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|DataTable", meta=(ScriptMethod, CallableWithoutWorldContext), CallInEditor)
	static void RemoveDataTableRow(UDataTable* DataTable, FName RowName, bool bSave);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|DataTable", meta=(ScriptMethod, CallableWithoutWorldContext), CallInEditor)
	static void SetDataTableRowStruct(UDataTable* DataTable, UScriptStruct* Struct);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", CustomThunk, meta = (CustomStructureParam = "InStruct"))
	static UScriptStruct* MakeScriptStruct(const int32& InStruct);
	
	DECLARE_FUNCTION(execMakeScriptStruct);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Math", CustomThunk, meta = (ArrayParm = "Items", ArrayTypeDependentParams = "Items,ReturnValue"))
	static void GetRandomItemByWeight(const TArray<int32>& Items, const TArray<float>& Weights, int32& ReturnValue, int32& Index);
	
	static void Generic_GetRandomItemByWeight(void* ItemsAddr, FArrayProperty* ItemsProperty, const TArray<float>& Weights, void* ReturnValueAddr, FProperty* ReturnValueProperty, int32& OutIndex);

	DECLARE_FUNCTION(execGetRandomItemByWeight)
	{
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FArrayProperty>(NULL);
		void* ItemsAddr = Stack.MostRecentPropertyAddress;
		FArrayProperty* ItemsProperty = static_cast<FArrayProperty*>(Stack.MostRecentProperty);
		if (!ItemsProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}
		P_GET_TARRAY_REF(float, Z_Param_Weights);
		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FProperty>(NULL);
		void* ReturnValueAddr = Stack.MostRecentPropertyAddress;
		FProperty* ReturnValueProperty = Stack.MostRecentProperty;
		P_GET_PROPERTY_REF(FIntProperty, Z_Param_Out_Index);
		P_FINISH;
		P_NATIVE_BEGIN;
		Generic_GetRandomItemByWeight(ItemsAddr, ItemsProperty, Z_Param_Weights, ReturnValueAddr, ReturnValueProperty, Z_Param_Out_Index);
		P_NATIVE_END;
	}

	// Parser property access exec functions
	DECLARE_FUNCTION(execGetPropertyByReflection)
	{
		P_GET_OBJECT(UObject, OwnerObject);
		P_GET_PROPERTY(FNameProperty, PropertyName);

		Stack.StepCompiledIn<FStructProperty>(NULL);
		void* SrcPropertyAddr = Stack.MostRecentPropertyAddress;
		FProperty* SrcProperty = CastField<FProperty>(Stack.MostRecentProperty);

		P_FINISH;

		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = Generic_GetPropertyByReflection(OwnerObject, PropertyName, SrcPropertyAddr, SrcProperty);
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execSetPropertyByReflection)
	{
		P_GET_OBJECT(UObject, OwnerObject);
		P_GET_PROPERTY(FNameProperty, PropertyName);

		Stack.StepCompiledIn<FStructProperty>(NULL);
		void* SrcPropertyAddr = Stack.MostRecentPropertyAddress;
		FProperty* SrcProperty = CastField<FProperty>(Stack.MostRecentProperty);

		P_FINISH;

		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = Generic_SetPropertyByReflection(OwnerObject, PropertyName, SrcPropertyAddr);
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execGetArrayByReflection)
	{
		P_GET_OBJECT(UObject, OwnerObject);
		P_GET_PROPERTY(FNameProperty, ArrayPropertyName);

		Stack.StepCompiledIn<FArrayProperty>(NULL);
		void* SrcArrayAddr = Stack.MostRecentPropertyAddress;
		FArrayProperty* SrcArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);

		P_FINISH;

		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = Generic_GetArrayByReflection(OwnerObject, ArrayPropertyName, SrcArrayAddr, SrcArrayProperty);
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execSetArrayByReflection)
	{
		P_GET_OBJECT(UObject, OwnerObject);
		P_GET_PROPERTY(FNameProperty, ArrayPropertyName);

		Stack.StepCompiledIn<FArrayProperty>(NULL);
		void* SrcArrayAddr = Stack.MostRecentPropertyAddress;
		FArrayProperty* SrcArrayProperty = CastField<FArrayProperty>(Stack.MostRecentProperty);

		P_FINISH;

		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = Generic_SetArrayByReflection(OwnerObject, ArrayPropertyName, SrcArrayAddr);
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execGetSetByReflection)
	{
		P_GET_OBJECT(UObject, OwnerObject);
		P_GET_PROPERTY(FNameProperty, SetPropertyName);

		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FSetProperty>(NULL);
		void* SetAddr = Stack.MostRecentPropertyAddress;
		FSetProperty* SetProperty = CastField<FSetProperty>(Stack.MostRecentProperty);
		if (!SetProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_FINISH;

		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = Generic_GetSetByReflection(OwnerObject, SetPropertyName, SetAddr, SetProperty);
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execSetSetByReflection)
	{
		P_GET_OBJECT(UObject, OwnerObject);
		P_GET_PROPERTY(FNameProperty, SetPropertyName);

		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FSetProperty>(NULL);
		void* SetAddr = Stack.MostRecentPropertyAddress;
		FSetProperty* SetProperty = CastField<FSetProperty>(Stack.MostRecentProperty);
		if (!SetProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_FINISH;

		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = Generic_SetSetByReflection(OwnerObject, SetPropertyName, SetAddr);
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execGetMapByReflection)
	{
		P_GET_OBJECT(UObject, OwnerObject);
		P_GET_PROPERTY(FNameProperty, MapPropertyName);

		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FMapProperty>(NULL);
		void* SrcMapAddr = Stack.MostRecentPropertyAddress;
		FMapProperty* SrcMapProperty = CastField<FMapProperty>(Stack.MostRecentProperty);
		if (!SrcMapProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_FINISH;

		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = Generic_GetMapByReflection(OwnerObject, MapPropertyName, SrcMapAddr, SrcMapProperty);
		P_NATIVE_END;
	}

	DECLARE_FUNCTION(execSetMapByReflection)
	{
		P_GET_OBJECT(UObject, OwnerObject);
		P_GET_PROPERTY(FNameProperty, MapPropertyName);

		Stack.MostRecentProperty = nullptr;
		Stack.StepCompiledIn<FMapProperty>(NULL);
		void* SrcMapAddr = Stack.MostRecentPropertyAddress;
		FMapProperty* SrcMapProperty = CastField<FMapProperty>(Stack.MostRecentProperty);
		if (!SrcMapProperty)
		{
			Stack.bArrayContextFailed = true;
			return;
		}

		P_FINISH;

		P_NATIVE_BEGIN;
		*(bool*)RESULT_PARAM = Generic_SetMapByReflection(OwnerObject, MapPropertyName, SrcMapAddr);
		P_NATIVE_END;
	}

#if WITH_EDITOR
	static void TryUpdateComment(FString& CommentString, const TSoftClassPtr<UObject>& ClassObject, const bool& bCommentModified);
	static void TryUpdateComment(FString& CommentString, const FGameplayTag& Tag, const bool& bCommentModified);
#endif

	// Generic functions for Parser property access
	static bool Generic_GetPropertyByReflection(UObject* OwnerObject, FName PropertyName, void*& OutPropertyAddr, FProperty*& OutProperty);
	static bool Generic_SetPropertyByReflection(UObject* OwnerObject, FName PropertyName, const void* SrcPropertyAddr);
	static bool Generic_GetArrayByReflection(UObject* OwnerObject, FName ArrayPropertyName, void*& OutArrayAddr, FArrayProperty*& OutArrayProperty);
	static bool Generic_SetArrayByReflection(UObject* OwnerObject, FName ArrayPropertyName, const void* SrcArrayAddr);
	static bool Generic_GetSetByReflection(UObject* OwnerObject, FName SetPropertyName, void*& OutSetAddr, FSetProperty*& OutSetProperty);
	static bool Generic_SetSetByReflection(UObject* OwnerObject, FName SetPropertyName, const void* SrcSetAddr);
	static bool Generic_GetMapByReflection(UObject* OwnerObject, FName MapPropertyName, void*& OutMapAddr, FMapProperty*& OutMapProperty);
	static bool Generic_SetMapByReflection(UObject* OwnerObject, FName MapPropertyName, const void* SrcMapAddr);



	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library", meta=(CallableWithoutWorldContext), CallInEditor)
	static FString GetClassObjectName(const TSoftClassPtr<UObject>& ClassObject);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Level", meta=(WorldContext="Actor", DefaultToSelf="Actor"), CallInEditor)
	static FString GetActorLevelName(const AActor* Actor);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|NavMovementComponent", meta=(WorldContext="NavMovementComponent", DefaultToSelf="NavMovementComponent", ScriptMethod, CallableWithoutWorldContext), CallInEditor)
	static FVector GetFeetLocation(const UNavMovementComponent* NavMovementComponent);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|CharacterMovementComponent", meta=(DefaultToSelf="CharacterMovementComponent", ScriptMethod, CallableWithoutWorldContext))
	static UNipcatCustomMovementMode* FindCustomMovementMode(const UCharacterMovementComponent* CharacterMovementComponent, uint8 InCustomMovementMode);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|CharacterMovementComponent", meta=(DefaultToSelf="CharacterMovementComponent", ScriptMethod, CallableWithoutWorldContext))
	static UNipcatCustomMovementMode* FindCustomMovementModeByTag(const UCharacterMovementComponent* CharacterMovementComponent, UPARAM(meta=(GameplayTagFilter="MovementMode.Custom")) FGameplayTag InCustomMovementModeTag);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|CharacterMovementComponent", meta=(DefaultToSelf="CharacterMovementComponent", ScriptMethod, CallableWithoutWorldContext))
	static void SetCustomMovementMode(UCharacterMovementComponent* CharacterMovementComponent, UPARAM(meta=(GameplayTagFilter="MovementMode.Custom")) FGameplayTag InCustomMovementModeTag);
	
	UFUNCTION(BlueprintPure, Category = "Nipcat Basic Gameplay Function Library|Controller", meta=(CallableWithoutWorldContext, DefaultToSelf="Controller"), CallInEditor)
	static ACharacter* GetCharacter(const AController* Controller);

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType = "FilterClass", DynamicOutputParam = "FilteredArray"), Category = "Utilities|Array")
	static void FilterObjectArray(const TArray<UObject*>& TargetArray, TSubclassOf<class UObject> FilterClass, TArray<UObject*>& FilteredArray);
	
	// Parser-style property access functions
	/**
	 * Get object PROPERTY value by property name using reflection
	 *
	 * @param Object - object that owns this PROPERTY
	 * @param PropertyName - property name
	 * @param OutValue - returned object property value
	 * @return true if property was found and retrieved successfully
	 */
	UFUNCTION(BlueprintPure, CustomThunk, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (CustomStructureParam = "OutValue", AutoCreateRefTerm = "OutValue", DisplayName = "Get Property By Reflection", DefaultToSelf = "Object"))
	static bool GetPropertyByReflection(UObject* Object, FName PropertyName, int32& OutValue);

	/**
	 * Set object PROPERTY value by property name using reflection
	 *
	 * @param Object - object that owns this PROPERTY
	 * @param PropertyName - property name
	 * @param Value - value to set to the property
	 * @return true if property was found and set successfully
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (CustomStructureParam = "Value", AutoCreateRefTerm = "Value", DisplayName = "Set Property By Reflection", DefaultToSelf = "Object"))
	static bool SetPropertyByReflection(UObject* Object, FName PropertyName, const int32& Value);

	/**
	 * Get object ARRAY value by property name using reflection
	 *
	 * @param Object - object that owns this ARRAY
	 * @param PropertyName - array property name
	 * @param OutValue - returned object array value
	 * @return true if array property was found and retrieved successfully
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (ArrayParm = "OutValue", ArrayTypeDependentParams = "OutValue", DisplayName = "Get Array By Reflection", DefaultToSelf = "Object"))
	static bool GetArrayByReflection(UObject* Object, FName PropertyName, TArray<int32>& OutValue);

	/**
	 * Set object ARRAY value by property name using reflection
	 *
	 * @param Object - object that owns this ARRAY
	 * @param PropertyName - array property name
	 * @param Value - array value to set
	 * @return true if array property was found and set successfully
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (ArrayParm = "Value", ArrayTypeDependentParams = "Value", DisplayName = "Set Array By Reflection", DefaultToSelf = "Object"))
	static bool SetArrayByReflection(UObject* Object, FName PropertyName, const TArray<int32>& Value);

	/**
	 * Get object SET value by property name using reflection
	 *
	 * @param Object - object that owns this SET
	 * @param PropertyName - set property name
	 * @param OutValue - returned object set value
	 * @return true if set property was found and retrieved successfully
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (SetParam = "OutValue", DisplayName = "Get Set By Reflection", DefaultToSelf = "Object"))
	static bool GetSetByReflection(UObject* Object, FName PropertyName, TSet<int32>& OutValue);

	/**
	 * Set object SET value by property name using reflection
	 *
	 * @param Object - object that owns this SET
	 * @param PropertyName - set property name
	 * @param Value - set value to set
	 * @return true if set property was found and set successfully
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (SetParam = "Value", DisplayName = "Set Set By Reflection", DefaultToSelf = "Object"))
	static bool SetSetByReflection(UObject* Object, FName PropertyName, const TSet<int32>& Value);

	/**
	 * Get object MAP value by property name using reflection
	 *
	 * @param Object - object that owns this MAP
	 * @param PropertyName - map property name
	 * @param OutValue - returned object map value
	 * @return true if map property was found and retrieved successfully
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (MapParam = "OutValue", DisplayName = "Get Map By Reflection", DefaultToSelf = "Object"))
	static bool GetMapByReflection(UObject* Object, FName PropertyName, TMap<int32, int32>& OutValue);

	/**
	 * Set object MAP value by property name using reflection
	 *
	 * @param Object - object that owns this MAP
	 * @param PropertyName - map property name
	 * @param Value - map value to set
	 * @return true if map property was found and set successfully
	 */
	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (MapParam = "Value", DisplayName = "Set Map By Reflection", DefaultToSelf = "Object"))
	static bool SetMapByReflection(UObject* Object, FName PropertyName, const TMap<int32, int32>& Value);

	/**
	 * Safe Get property value by name with type validation
	 * Returns true if successful, false if property not found or type mismatch
	 */
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (DisplayName = "Parser Safe Get"))
	static bool ParserSafeGetProperty(UObject* Object, FName PropertyName, int32& OutValue);

	/**
	 * Safe Set property value by name with type validation
	 * Returns true if successful, false if property not found or type mismatch
	 */
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (DisplayName = "Parser Safe Set"))
	static bool ParserSafeSetProperty(UObject* Object, FName PropertyName, const int32& Value);

	/**
	 * Check if property exists and get its type information
	 */
	UFUNCTION(BlueprintCallable, Category = "Nipcat Basic Gameplay Function Library|Parser Property", meta = (DisplayName = "Parser Get Property Info"))
	static bool ParserGetPropertyInfo(UObject* Object, FName PropertyName, FString& OutTypeName, int32& OutSize);

	template<typename T>
	static void SerializeObjectArray(FArchive& Ar, TArray<TObjectPtr<T>>& Array, UObject* Outer)
	{
		if (Ar.IsSaveGame())
		{
			if (Ar.IsSaving())
			{
				int32 ItemCount = Array.Num();
				Ar << ItemCount;
				for (const auto& Item : Array)
				{ 
					bool bValidItem = Item != nullptr && Item->GetClass();
					Ar << bValidItem;
					if (bValidItem)
					{
						UClass* ItemClass = Item->GetClass();
						Ar << ItemClass;
						Item->Serialize(Ar);
					}
				}
			}
			else if (Ar.IsLoading())
			{
				int32 ItemCount;
				Ar << ItemCount;
				Array.SetNum(ItemCount);
				for (int32 i = 0; i < ItemCount; ++i)
				{
					bool bValidItem;
					Ar << bValidItem;
					if (bValidItem)
					{
						UClass* ItemClass;
						Ar << ItemClass;
						if (ItemClass)
						{
							T* Item = Array[i];
							if (!Item)
							{
								Item = NewObject<T>(Outer, ItemClass);
							}
							Item->Serialize(Ar);
							Array[i] = Item;
						}
						else  
						{
							Array[i] = nullptr;
						}
					}
					else
					{
						Array[i] = nullptr;
					}
				}
			}
		}
	}
	
	template<typename KeyType, typename ValueType>
	static void SerializeObjectMap(FArchive& Ar, TMap<KeyType, TObjectPtr<ValueType>>& Map, UObject* Outer)
	{
		if (Ar.IsSaveGame())
		{
			if (Ar.IsSaving())
			{
				int32 ItemCount = Map.Num();
				Ar << ItemCount;
				for (const auto& [Key, Item] : Map)
				{
					Ar << const_cast<KeyType&>(Key);
					bool bValidItem = Item != nullptr && Item->GetClass();
					Ar << bValidItem;
					if (bValidItem)
					{
						UClass* ItemClass = Item->GetClass();
						Ar << ItemClass;
						Item->Serialize(Ar);
					}
				}
			}
			else if (Ar.IsLoading())
			{
				int32 ItemCount;
				Ar << ItemCount;
				for (auto It = Map.CreateIterator(); It; ++It)
				{
					KeyType Key = It.Key();
					TObjectPtr<ValueType> Value = It.Value();
					It.RemoveCurrent();
					if (auto Object = Cast<UObject>(Value.Get()))
					{
						const FString NewName = Object->GetName() + TEXT("_Garbage_") + FGuid::NewGuid().ToString();
						Object->Rename(*NewName, GetTransientPackage());
						Object->ConditionalBeginDestroy();
					}
				}
				Map.Empty();
				Map.Reserve(ItemCount);
				for (int32 i = 0; i < ItemCount; ++i)
				{
					KeyType Key;
					Ar << Key;
					bool bValidItem;
					Ar << bValidItem;
					if (bValidItem)
					{
						UClass* ItemClass;
						Ar << ItemClass;
						if (ItemClass)
						{
							ValueType* Item = NewObject<ValueType>(Outer, ItemClass);
							Item->Serialize(Ar);
							Map.Add(Key, Item);
						}
						else
						{
							Map.Add(Key);
						}
					}
					else
					{
						Map.Add(Key);
					}
				}
			}
		}
	}
};
