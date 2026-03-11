// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "NipcatLocomotionSystemEditorFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class NIPCATLOCOMOTIONSYSTEMEDITOR_API UNipcatLocomotionSystemEditorFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void ApplyAnimationModifierToAnimSequence(UAnimSequence* AnimSequence, TSubclassOf<UAnimationModifier> AnimationModifier);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void SetCurveCompressionSettings(UAnimSequence* AnimSequence, UAnimCurveCompressionSettings* CurveCompressionSettings);

	UFUNCTION(BlueprintCallable, Category = "Nipcat LocomotionSystem Editor Function Library", meta=(CallableWithoutWorldContext))
	static void AddNewVirtualBone(USkeleton* Skeleton, const FName SourceBoneName, const FName TargetBoneName, FName NewVirtualBoneName);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void RemoveVirtualBone(USkeleton* Skeleton, const FName VirtualBoneName);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void ExportMontageTrackAnimation(UAnimMontage* AnimMontage);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void TrimAnimation(UAnimSequence* AnimSequence, int StartFrame, int EndFrame);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void ApplyPlayRateCurve(UAnimSequenceBase* AnimSequenceBase, const FName PlayRateCurveName = TEXT("PlayRate"));
	
	static void TimeManipulation(const IAnimationDataModel* Model, IAnimationDataController& Controller, const FRichCurve& TimeManipulationCurve);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void AdjustAnimationSpeed(UAnimSequence* AnimSequence, float Speed = 1.f, int32 StartFrame = 0, int32 EndFrame = 0, int32 BlendInFrames = 0, int32 BlendOutFrames = 0);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void AppendAnimation(UAnimSequence* AnimSequence, UAnimSequence* NextAnimSequence, int32 NextStartTime = 0, int32 NextBlendFrames = 0);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void NormalizeAnimationRootMotion(UAnimSequence* AnimSequence, bool AutoDetectNormalizeDirection = true, FVector Scale = FVector(1, 1, 1), int32 StartFrame = 0, int32 EndFrame = -1, bool KeepRootMotionAfterEndFrame = false);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void NormalizeAnimationRotation(UAnimSequence* AnimSequence);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void AddRootRotation(UAnimSequence* AnimSequence, FRotator DeltaRotation);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static bool IsDirectChildOfRootBone(USkeleton* Skeleton, FName BoneName);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void Snap(UAnimSequence* AnimSequence, const TMap<FName, FName>& SnapToBone);
	
	static void CalculateHierarchyTransforms(const IAnimationDataModel* Model, const int32 j, const TArray<FName>& hierarchy, TArray<FTransform>& hierarchyTransforms, TArray<FTransform>& localTransforms, TArray<FTransform>& parentCompSpaceTransforms);
	
	static void CalculateHierarchyTransforms(const UAnimSequence* AnimSequence, const FName snapBoneName, const FName targetBoneName, const int32 Num
		, TArray<FTransform>& snapLocalTransforms, TArray<FTransform>& snapParentCompSpaceTransforms, FTransform& snapParentRefTransform
		, TArray<FTransform>& targetLocalTransforms, TArray<FTransform>& targetParentCompSpaceTransforms, FTransform& targetParentRefTransform, TArray<FTransform>& commonParentCompSpaceTransforms);
	
	static void FixedGetBoneTrackTransforms(const UAnimSequence* AnimSequence, FName TrackName, TArray<FTransform>& OutTransforms);
	
	static void ModifyBoneTransformAndKeepChildren(UAnimSequence* AnimSequence, FName BoneName,
	                                        const TArray<FTransform>& NewBoneTransforms, bool bShouldTransact = false);
	
	static bool CalculateCircumcenter(const FVector& A, const FVector& B, const FVector& C, FVector& OutCenter, FVector& OutNormal);
	static float CalculateSignedAngle(const FVector& From, const FVector& To, const FVector& Axis);

	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void ModifyBoneTrajectory(UAnimSequence* AnimSequence, FName BoneName, FVector TargetLocation, FName AlphaCurveName, bool Circular = true);
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void ModifyBoneTrajectory_Bilinear(UAnimSequence* AnimSequence, FName BoneName, FName DistanceCurveName, FVector ForwardDirection = FVector(0, 1, 0));
	
	UFUNCTION(BlueprintCallable, Category = "Nipcat Locomotion System Editor Function Library", meta=(CallableWithoutWorldContext))
	static void CookBoneLocationToCurve(UAnimSequence* AnimSequence, FName BoneName, FName XCurveName, FName YCurveName, FName ZCurveName);
	
#endif
};
