// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatBasicGameplayFunctionLibrary.h"

#if WITH_EDITOR
#include "PersonaUtils.h"
#include "PersonaModule.h"
#include "IPersonaToolkit.h"
#include "IPersonaPreviewScene.h"
#include "DataTableEditorUtils.h"
#endif
#include "GameFramework/Character.h"
#include "GameFramework/PlayerInput.h"
#include "ScalableFloat.h"
#include "Animation/AnimationPoseData.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "Blueprint/BlueprintExceptionInfo.h"
#include "Engine/AssetManager.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Engine/StaticMeshSocket.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Navigation/PathFollowingComponent.h"
#include "NipcatBasicGameplay/Character/NipcatCharacterMovementComponent.h"
#include "NipcatBasicGameplay/Settings/NipcatBasicGameplayDeveloperSettings.h"
#include "UObject/SavePackage.h"

#if WITH_EDITOR
#include "Editor.h"
#include "Editor/EditorEngine.h"
#include "EditorViewportClient.h"
#endif

void UNipcatBasicGameplayFunctionLibrary::ForceDestroyComponent(UActorComponent* Component)
{
	if (Component)
	{
		Component->DestroyComponent();
	}
}

float UNipcatBasicGameplayFunctionLibrary::GetGlobalTimeDilation(const UObject* WorldContextObject)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	return World ? World->GetWorldSettings()->TimeDilation : 1.f;
}

void UNipcatBasicGameplayFunctionLibrary::SetGlobalTimeDilation(const UObject* WorldContextObject, float TimeDilation)
{
	UWorld* const World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World != nullptr)
	{
		AWorldSettings* const WorldSettings = World->GetWorldSettings();
		if (WorldSettings != nullptr)
		{
			float const ActualTimeDilation = WorldSettings->SetTimeDilation(TimeDilation);
			if (TimeDilation != ActualTimeDilation)
			{
				UE_LOG(LogBlueprintUserMessages, Warning, TEXT("Time Dilation must be between %f and %f.  Clamped value to that range."), WorldSettings->MinGlobalTimeDilation, WorldSettings->MaxGlobalTimeDilation);
			}
		}
	}
}

const UObject* UNipcatBasicGameplayFunctionLibrary::GetClassDefaultObject(TSubclassOf<UObject> InClass)
{
	if (InClass)
	{
		return InClass->GetDefaultObject();
	}
	return nullptr;
}

UObject* UNipcatBasicGameplayFunctionLibrary::GetMutableClassDefaultObject(TSubclassOf<UObject> InClass)
{
	if (InClass)
	{
		return GetMutableDefault<UObject>(InClass);
	}
	return nullptr;
}

void UNipcatBasicGameplayFunctionLibrary::DeepCopyInstancedObjects(UScriptStruct* ScriptStruct, void* StructMemory,
	UObject* Outer)
{
	// 遍历结构体的所有属性
    for (TFieldIterator<FProperty> PropIt(ScriptStruct); PropIt; ++PropIt)
    {
        FProperty* Prop = *PropIt;

        // --- 情况 1: 单个对象指针 (Object Property) ---
        if (FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop))
        {
            // 检查是否有 Instanced 标记 (CPF_InstancedReference)
            // 或者属性元数据里有 EditInline
            if (ObjProp->HasAnyPropertyFlags(CPF_InstancedReference | CPF_ContainsInstancedReference))
            {
                // 获取当前指针指向的旧对象
                UObject* OldObj = ObjProp->GetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(StructMemory));

                if (OldObj)
                {
                    // 【核心步骤】复制对象，并将 Outer 设置为新的 DataAsset
                    UObject* NewObj = DuplicateObject(OldObj, Outer);
                    
                    // 将新对象的指针写回结构体
                    ObjProp->SetObjectPropertyValue(ObjProp->ContainerPtrToValuePtr<void>(StructMemory), NewObj);
                }
            }
        }
        // --- 情况 2: 数组 (Array Property) ---
        else if (FArrayProperty* ArrayProp = CastField<FArrayProperty>(Prop))
        {
            FScriptArrayHelper Helper(ArrayProp, ArrayProp->ContainerPtrToValuePtr<void>(StructMemory));
            
            // 检查数组内部存的是不是对象指针
            if (FObjectProperty* InnerObjProp = CastField<FObjectProperty>(ArrayProp->Inner))
            {
                if (InnerObjProp->HasAnyPropertyFlags(CPF_InstancedReference | CPF_ContainsInstancedReference))
                {
                    for (int32 i = 0; i < Helper.Num(); ++i)
                    {
                        UObject* OldObj = InnerObjProp->GetObjectPropertyValue(Helper.GetRawPtr(i));
                        if (OldObj)
                        {
                            UObject* NewObj = DuplicateObject(OldObj, Outer);
                            InnerObjProp->SetObjectPropertyValue(Helper.GetRawPtr(i), NewObj);
                        }
                    }
                }
            }
            // 如果数组里存的是结构体，递归处理
            else if (FStructProperty* InnerStructProp = CastField<FStructProperty>(ArrayProp->Inner))
            {
                for (int32 i = 0; i < Helper.Num(); ++i)
                {
                    DeepCopyInstancedObjects(InnerStructProp->Struct, Helper.GetRawPtr(i), Outer);
                }
            }
        }
        // --- 情况 3: 嵌套结构体 (Struct Property) ---
        else if (FStructProperty* StructProp = CastField<FStructProperty>(Prop))
        {
            // 递归进入子结构体
            DeepCopyInstancedObjects(StructProp->Struct, StructProp->ContainerPtrToValuePtr<void>(StructMemory), Outer);
        }
    }
}

AActor* UNipcatBasicGameplayFunctionLibrary::SpawnActorDeferred(UObject* WorldContextObject,
                                                                TSubclassOf<AActor> ActorClass, FTransform const& Transform, AActor* Owner, APawn* Instigator,
                                                                ESpawnActorCollisionHandlingMethod CollisionHandlingOverride, ESpawnActorScaleMethod TransformScaleMethod)
{
	AActor* NewActor = nullptr;

	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (World && *ActorClass)
	{
		NewActor = World->SpawnActorDeferred<AActor>(*ActorClass, Transform, Owner, Instigator, CollisionHandlingOverride, TransformScaleMethod);
	}
	
	return NewActor;
}

void UNipcatBasicGameplayFunctionLibrary::FinishActorSpawning(AActor* Actor, FTransform const& Transform)
{
	Actor->FinishSpawning(Transform);
}

void UNipcatBasicGameplayFunctionLibrary::FlushPressedKeys(APlayerController* PlayerController)
{
	if (UPlayerInput* PlayerInput = PlayerController->PlayerInput)
	{
		PlayerInput->FlushPressedKeys();
	}
}

void UNipcatBasicGameplayFunctionLibrary::FlushPressedActionBindingKeys(APlayerController* PlayerController,
	FName ActionName)
{
	if (UPlayerInput* PlayerInput = PlayerController->PlayerInput)
	{
		PlayerInput->FlushPressedActionBindingKeys(ActionName);
	}
}

void UNipcatBasicGameplayFunctionLibrary::AddRotationModifier(ACharacter* Character,
	UCharacterRotationModifier* RotationModifier)
{
	if (UNipcatCharacterMovementComponent* NipcatCharacterMovementComponent =
		Cast<UNipcatCharacterMovementComponent>(Character->GetCharacterMovement()))
	{
		NipcatCharacterMovementComponent->AddRotationModifier(RotationModifier);
	}
}

void UNipcatBasicGameplayFunctionLibrary::RemoveRotationModifier(ACharacter* Character,
	UCharacterRotationModifier* RotationModifier)
{
	if (UNipcatCharacterMovementComponent* NipcatCharacterMovementComponent =
		Cast<UNipcatCharacterMovementComponent>(Character->GetCharacterMovement()))
	{
		NipcatCharacterMovementComponent->RemoveRotationModifier(RotationModifier);
	}
}

bool UNipcatBasicGameplayFunctionLibrary::FindFloatCurveFromAnimation(const UAnimSequenceBase* AnimSequenceBase,
	FName CurveName, FFloatCurve& OutFloatCurve)
{
	if (!AnimSequenceBase)
	{
		return false;
	}
	for (auto& Curve : AnimSequenceBase->GetCurveData().FloatCurves)
	{
		if (Curve.GetName() == CurveName)
		{
			OutFloatCurve = Curve;
			return true;
		}
	}
	return false;
}

float UNipcatBasicGameplayFunctionLibrary::GetScaledTimeStep(const FFloatCurve& RateScaleCurve, float StartTime, float InStep, int32 SampleRate)
{
	if (InStep <= 0.0f || SampleRate <= 0)
	{
		return 0.0f;
	}

	float TotalScaledTime = 0.0f;
	float Remaining = InStep;
	float CurrentTime = StartTime;
	const float MaxStepSize = 1.0f / static_cast<float>(SampleRate);

	while (Remaining > 0.0f)
	{
		// 计算当前时间片段的实际步长
		const float StepSize = FMath::Min(MaxStepSize, Remaining);
        
		// 获取当前时间的速率缩放值
		const float Rate = RateScaleCurve.Evaluate(CurrentTime);
        
		// 处理无效速率值（保持时间正向流动）
		const float SafeRate = FMath::Max(Rate, KINDA_SMALL_NUMBER);
        
		// 计算缩放后的时间并累加
		TotalScaledTime += StepSize / SafeRate;
        
		// 更新时间参数
		CurrentTime += StepSize;
		Remaining -= StepSize;
	}

	return TotalScaledTime;
	/*
	if (InStep == 0)
	{
		return InStep;
	}
	
	const float FPSStep = 1.0f / SampleRate;
	float FinalStep = 0.0f;
	float SubStep = 0.0f;
	while (true)
	{
		float StretchRate = RateScaleCurve.Evaluate(StartTime + SubStep);
		if (StretchRate == 0.0f)
		{
			StretchRate = 0.00001f;
		}
		float S;
		if (StretchRate >= 1.0f)
		{
			S = FPSStep;
			
		}
		else
		{
			S = FPSStep * StretchRate;
		}
		SubStep += S;
		const float ScaledStep = S * StretchRate;
			
		if (InStep <= ScaledStep)
		{
			// FinalStep += StretchRate * InStep;
			break;
		}
		InStep -= ScaledStep;
		FinalStep += S;
	}
	return FinalStep;*/
}

float UNipcatBasicGameplayFunctionLibrary::GetOriginalTimeStep(const FFloatCurve& RateScaleCurve, float StartTime, float InStep, int32 SampleRate)
{
	// 输入校验
	if (InStep <= 0.0f || SampleRate <= 0)
	{
		return 0.0f;
	}

	// 二分法参数配置
	const int32 MaxIterations = 100;        // 最大迭代次数防止死循环
	const float Tolerance = 0.001f;         // 精度阈值（毫秒级）
	const float MaxOriginalEstimate = 100.0f * InStep; // 最大原始时间估计值

	float Low = 0.0f;
	float High = MaxOriginalEstimate;
	float BestGuess = 0.0f;

	for (int32 i = 0; i < MaxIterations; ++i)
	{
		float Mid = (Low + High) * 0.5f;
        
		// 正向计算当前猜测值对应的实际时间
		float CalculatedStep = GetScaledTimeStep(RateScaleCurve, StartTime, Mid, SampleRate);
        
		// 误差计算
		float Error = CalculatedStep - InStep;
        
		// 找到足够精确的解
		if (FMath::Abs(Error) <= Tolerance)
		{
			return Mid;
		}

		// 调整搜索区间
		if (Error > 0)
		{
			High = Mid;  // 实际时间过长，需要减少原始时间
		}
		else
		{
			Low = Mid;    // 实际时间不足，需要增加原始时间
			BestGuess = Mid; // 记录最近的有效值
		}
	}

	// 返回最佳近似值（达到最大迭代次数时）
	return BestGuess;
	
	/*
	if (InStep == 0)
	{
		return InStep;
	}
	
	const float IntegralStep = (1.f / SampleRate) * FMath::Sign(InStep);
	float IntegralDuration = 0.f;
	
	float RuntimeDuration = 0.f;
	while (FMath::Abs(RuntimeDuration) <= FMath::Abs(InStep))
	{
		const float StretchRate = FMath::Max(RateScaleCurve.Evaluate(StartTime + IntegralDuration), 0.01f);
		IntegralDuration += IntegralStep;
		RuntimeDuration += IntegralStep / StretchRate;
	}

	return IntegralDuration;*/
}

float UNipcatBasicGameplayFunctionLibrary::GetScalableFloatValueAtLevel(const FScalableFloat& ScalableFloat, float Level,
	const FString& ContextString)
{
	return ScalableFloat.GetValueAtLevel(Level, &ContextString);
}

FCompactPose UNipcatBasicGameplayFunctionLibrary::ExtractAnimMontagePose(const UAnimMontage* TargetMontage, double DesiredAnimTime, FName SlotName, UAnimInstance* AnimInstance)
{
	// Pose Data
	FCompactPose Pose;
	
	if (!AnimInstance || !TargetMontage)
	{
		return FCompactPose();
	}
	
	Pose.SetBoneContainer(&AnimInstance->GetRequiredBones());
		
	FBlendedCurve Curve;
	Curve.InitFrom(AnimInstance->GetRequiredBones());
		
	UE::Anim::FStackAttributeContainer Attributes;
		
	FAnimationPoseData PoseData(Pose, Curve, Attributes);

	// Extract Context
	const FAnimExtractContext ExtractionContext(DesiredAnimTime, true);
		
	TargetMontage->GetAnimationData(SlotName)->GetAnimationPose(PoseData, ExtractionContext);

	return PoseData.GetPose();
}

FTransform UNipcatBasicGameplayFunctionLibrary::GetMeshSpaceTransform(const USkeletalMeshComponent* MeshComp, FName BoneName, FCompactPose Pose)
{
	int32 TargetBoneIndex = MeshComp->GetBoneIndex(BoneName);
	FCompactPoseBoneIndex BoneIndex(TargetBoneIndex);
	if (!Pose.IsValidIndex(BoneIndex))
	{
		return FTransform::Identity;
	}
	
	FTransform BoneTransform = Pose[BoneIndex];

	int32 CurLoop = 0;
	while (true)
	{
		BoneName = MeshComp->GetParentBone(BoneName);
		if (BoneName != NAME_None && CurLoop < 16)
		{
			TargetBoneIndex = MeshComp->GetBoneIndex(BoneName);
			BoneIndex = FCompactPoseBoneIndex(TargetBoneIndex);
			FTransform ParentBoneTrans = Pose[BoneIndex];
			BoneTransform = BoneTransform * ParentBoneTrans;

			CurLoop += 1;
		}
		else
		{
			break;
		}
	}
	
	return BoneTransform;
}

FTransform UNipcatBasicGameplayFunctionLibrary::GetMeshSpaceTransform(const USkeletalMeshComponent* MeshComp,
	FName BoneName, FPoseSnapshot Pose)
{
	FTransform SocketTransform = FTransform::Identity;
	
	if (USkeletalMeshSocket const* Socket = MeshComp->GetSocketByName(BoneName))
	{
		SocketTransform = MeshComp->GetSocketTransform(BoneName, RTS_ParentBoneSpace);
		BoneName = Socket->BoneName;
	}
	
	int32 BoneIndex = MeshComp->GetBoneIndex(BoneName);
	if (!Pose.LocalTransforms.IsValidIndex(BoneIndex))
	{
		return FTransform::Identity;
	}
	
	FTransform BoneTransform = SocketTransform * Pose.LocalTransforms[BoneIndex];

	int32 CurLoop = 0;
	while (true)
	{
		BoneName = MeshComp->GetParentBone(BoneName);
		if (BoneName != NAME_None && CurLoop < 16)
		{
			BoneIndex = MeshComp->GetBoneIndex(BoneName);
			FTransform ParentBoneTrans = Pose.LocalTransforms[BoneIndex];
			BoneTransform = BoneTransform * ParentBoneTrans;

			CurLoop += 1;
		}
		else
		{
			break;
		}
	}
	
	return BoneTransform;
}

FTransform UNipcatBasicGameplayFunctionLibrary::GetMontageBoneTransform(
	UAnimMontage* TargetMontage,
	USkeletalMeshComponent* MeshComp,
	FName BoneName,
	float DesiredAnimTime,
	FName SlotName,
	bool bApplyPostProcessPoseAsAdditive,
	bool bMeshSpaceAdditive,
	bool bShowDebug
)
{
	if (!MeshComp)
	{
		return FTransform::Identity;
	}
	UAnimInstance* AnimInstance = MeshComp->GetAnimInstance();

	if (!AnimInstance)
	{
		return FTransform::Identity;
	}
	
	if (!TargetMontage)
	{
		if (MeshComp)
		{
			AnimInstance->IsPlayingSlotAnimation(nullptr, SlotName, TargetMontage);
		}
	}
	if (MeshComp && TargetMontage && TargetMontage->IsValidSlot(SlotName))
	{
		/** To prevent the crash with error !bShouldEnforceAllocMarks || NumMarks > 0, add the below line. */
		FMemMark Mark(FMemStack::Get());
		
		FCompactPose Pose = ExtractAnimMontagePose(TargetMontage, DesiredAnimTime, SlotName, AnimInstance);

		if (bApplyPostProcessPoseAsAdditive)
		{
			FPoseSnapshot FinalPoseSnapShot;
			MeshComp->SnapshotPose(FinalPoseSnapShot);

			const float MontagePosition = AnimInstance->Montage_GetPosition(TargetMontage);
			FCompactPose ActualMontagePose = ExtractAnimMontagePose(TargetMontage, MontagePosition, SlotName, AnimInstance);
			
			FTransform SnapShotBoneTransform;
			FTransform BaseTransform;
			FTransform AdditiveTransform;
			FTransform BoneTransform;
			if (bMeshSpaceAdditive)
			{
				SnapShotBoneTransform = GetMeshSpaceTransform(MeshComp, BoneName, FinalPoseSnapShot);
				BaseTransform = GetMeshSpaceTransform(MeshComp, BoneName, ActualMontagePose);
				AdditiveTransform = BaseTransform.Inverse() * SnapShotBoneTransform;
				BoneTransform = GetMeshSpaceTransform(MeshComp, BoneName, Pose);

				if (bShowDebug)
				{
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, FString::Printf(TEXT("SnapShot Bone Transform: %s"), *SnapShotBoneTransform.ToString()));
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, FString::Printf(TEXT("Base Transform: %s"), *BaseTransform.ToString()));
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, FString::Printf(TEXT("Additive Transform: %s"), *AdditiveTransform.ToString()));
					
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, FString::Printf(TEXT("Pre Fix Transform: %s"), *BoneTransform.ToString()));
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, FString::Printf(TEXT("Fixed Transform: %s"), *(BoneTransform * AdditiveTransform).ToString()));
					GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT(" ")));
				}
				return BoneTransform * AdditiveTransform * MeshComp->GetComponentTransform();
			}
			else
			{
				for (int32 i = 0; i < FinalPoseSnapShot.BoneNames.Num(); ++i)
				{
					const FName SnapShotBoneName = FinalPoseSnapShot.BoneNames[i];
					int32 SnapShotBoneIndexNumber = MeshComp->GetBoneIndex(SnapShotBoneName);
					FCompactPoseBoneIndex SnapShotBoneIndex(SnapShotBoneIndexNumber);

					
					SnapShotBoneTransform = FinalPoseSnapShot.LocalTransforms[i];
					BaseTransform = ActualMontagePose[SnapShotBoneIndex];
					AdditiveTransform = BaseTransform.Inverse() * SnapShotBoneTransform;

					
					if (SnapShotBoneName == BoneName && bShowDebug)
					{
						GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, FString::Printf(TEXT("SnapShot Bone Transform: %s"), *SnapShotBoneTransform.ToString()));
						GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, FString::Printf(TEXT("Base Transform: %s"), *BaseTransform.ToString()));
						GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Orange, FString::Printf(TEXT("Additive Transform: %s"), *AdditiveTransform.ToString()));
					}
					
					BoneTransform = Pose[SnapShotBoneIndex];
					
					Pose[SnapShotBoneIndex] = BoneTransform * AdditiveTransform;
					
					if (SnapShotBoneName == BoneName && bShowDebug)
					{
						GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, FString::Printf(TEXT("Pre Fix Transform: %s"), *BoneTransform.ToString()));
						GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Cyan, FString::Printf(TEXT("Fixed Transform: %s"), *Pose[SnapShotBoneIndex].ToString()));
						GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT(" ")));
					}
				}
				
				BoneTransform = GetMeshSpaceTransform(MeshComp, BoneName, Pose);
				
				BoneTransform = BoneTransform * MeshComp->GetComponentTransform();

				return BoneTransform;
			}
		}
		else
		{
			FTransform BoneTransform = GetMeshSpaceTransform(MeshComp, BoneName, Pose);

			BoneTransform = BoneTransform * MeshComp->GetComponentTransform();

			return BoneTransform;
		}
	}
	return FTransform::Identity;
}

FAnimationCurveIdentifier UNipcatBasicGameplayFunctionLibrary::MakeCurveIdentifier(FName Name,
	ERawCurveTrackTypes CurveType)
{
	return FAnimationCurveIdentifier(Name, CurveType);
}

UAnimSequenceBase* UNipcatBasicGameplayFunctionLibrary::GetAnimationAssetFromNotify(const UAnimNotify* AnimNotify)
{
	if (AnimNotify && AnimNotify->GetOuter())
	{
		return Cast<UAnimSequenceBase>(AnimNotify->GetOuter());
	}
	return nullptr;
}

UAnimSequenceBase* UNipcatBasicGameplayFunctionLibrary::GetAnimationAssetFromNotifyState(
	const UAnimNotifyState* AnimNotifyState)
{
	if (AnimNotifyState && AnimNotifyState->GetOuter())
	{
		return Cast<UAnimSequenceBase>(AnimNotifyState->GetOuter());
	}
	return nullptr;
}

USkeleton* UNipcatBasicGameplayFunctionLibrary::GetAnimAssetSkeleton(const UAnimSequenceBase* AnimSequenceBase)
{
	if (AnimSequenceBase)
	{
		return AnimSequenceBase->GetSkeleton();
	}
	return nullptr;
}

TArray<FName> UNipcatBasicGameplayFunctionLibrary::GetAttachedObjectSocketNames(const USkeleton* Skeleton)
{
	TArray<FName> Result;
#if WITH_EDITOR
	for (auto It = Skeleton->PreviewAttachedAssetContainer.CreateConstIterator(); It; ++It )
	{
		Result.AddUnique(It->AttachedTo);
	}
	Result.Insert(NAME_None, 0);
#endif
	return Result;
}

UObject* UNipcatBasicGameplayFunctionLibrary::GetPreviewAssetAtSocket(const USkeleton* Skeleton, const FName Socket)
{
	UObject* Result = nullptr;
#if WITH_EDITOR
	Result = Skeleton->PreviewAttachedAssetContainer.GetAttachedObjectByAttachName(Socket);
#endif
	return Result;
}

TMap<FName, UObject*> UNipcatBasicGameplayFunctionLibrary::GetAllPreviewAssets(const USkeleton* Skeleton)
{
	TMap<FName, UObject*> Result;
#if WITH_EDITOR
	for (auto It = Skeleton->PreviewAttachedAssetContainer.CreateConstIterator(); It; ++It )
	{
		Result.Add(It->AttachedTo, It->GetAttachedObject());
	}
#endif
	return Result;
}

void UNipcatBasicGameplayFunctionLibrary::AddPreviewAsset(USkeleton* Skeleton, UStaticMesh* StaticMesh,
                                                          FName Socket)
{
#if WITH_EDITOR
	Skeleton->PreviewAttachedAssetContainer.AddUniqueAttachedObject(StaticMesh, Socket);
#endif
}

void UNipcatBasicGameplayFunctionLibrary::AddPreviewAssetForAnimationAsset(UAnimationAsset* AnimationAsset,
	UObject* PreviewAsset, FName Socket)
{

#if WITH_EDITOR
	if (PreviewAsset)
	{
		TArray<IAssetEditorInstance*> AssetEditors = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorsForAsset(AnimationAsset);

		for (IAssetEditorInstance* ExistingEditor : AssetEditors)
		{
			if (ExistingEditor->GetEditorName() == FName("AnimationEditor"))
			{
				if (const IAnimationEditor* AnimEditor = static_cast<IAnimationEditor*>(ExistingEditor))
				{
					if (AnimEditor->GetPersonaToolkit()->GetAnimationAsset() == AnimationAsset)
					{
						// Change the current anim to this one
						AnimEditor->GetPersonaToolkit()->GetPreviewScene().Get()
						.AttachObjectToPreviewComponent(PreviewAsset, Socket);
						break;
					}
				}
			}
		}
	}
#endif
	
}

void UNipcatBasicGameplayFunctionLibrary::RemovePreviewAsset(USkeleton* Skeleton, const UStaticMesh* StaticMesh)
{
#if WITH_EDITOR
	for (auto It = Skeleton->PreviewAttachedAssetContainer.CreateIterator(); It; ++It )
	{
		if (It->GetAttachedObject() == StaticMesh)
		{
			It.RemoveCurrent();
		}
	}
#endif
}

void UNipcatBasicGameplayFunctionLibrary::RemovePreviewAssetAtSocket(USkeleton* Skeleton, FName Socket)
{
#if WITH_EDITOR
	while (const auto ObjectToRemove = Skeleton->PreviewAttachedAssetContainer.GetAttachedObjectByAttachName(Socket))
	{
		Skeleton->PreviewAttachedAssetContainer.RemoveAttachedObject(ObjectToRemove, Socket);
	}
#endif
}

void UNipcatBasicGameplayFunctionLibrary::RemovePreviewAssetForAnimation(UAnimationAsset* AnimationAsset,
	UObject* PreviewAsset, FName Socket)
{
#if WITH_EDITOR
	TArray<IAssetEditorInstance*> AssetEditors = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->FindEditorsForAsset(AnimationAsset);

	for (IAssetEditorInstance* ExistingEditor : AssetEditors)
	{
		if (ExistingEditor->GetEditorName() == FName("AnimationEditor"))
		{
			const IAnimationEditor* AnimEditor = static_cast<IAnimationEditor*>(ExistingEditor);
			if (AnimEditor->GetPersonaToolkit()->GetAnimationAsset() == AnimationAsset)
			{
				AnimEditor->GetPersonaToolkit()->GetPreviewScene().Get()
				.RemoveAttachedObjectFromPreviewComponent(PreviewAsset, Socket);
				break;
			}
		}
	}
#endif
}

void UNipcatBasicGameplayFunctionLibrary::RemoveAllAttachedAssets(USkeleton* Skeleton)
{
#if WITH_EDITOR
	Skeleton->PreviewAttachedAssetContainer.ClearAllAttachedObjects();
#endif
}

USkeletalMesh* UNipcatBasicGameplayFunctionLibrary::GetPreviewMesh(USkeleton* Skeleton)
{
	if (Skeleton)
	{
		return Skeleton->GetPreviewMesh();
	}
	return nullptr;
}

void UNipcatBasicGameplayFunctionLibrary::SetPreviewMesh(USkeleton* Skeleton, USkeletalMesh* PreviewMesh)
{
	if (Skeleton)
	{
		Skeleton->SetPreviewMesh(PreviewMesh);
	}
}

USkeletalMesh* UNipcatBasicGameplayFunctionLibrary::GetAnimationPreviewMesh(UAnimationAsset* AnimationAsset)
{
	if (AnimationAsset)
	{
		return AnimationAsset->GetPreviewMesh();
	}
	return nullptr;
}

void UNipcatBasicGameplayFunctionLibrary::SetAnimationPreviewMesh(UAnimationAsset* AnimationAsset,
	USkeletalMesh* PreviewMesh)
{
	if (AnimationAsset)
	{
		return AnimationAsset->SetPreviewMesh(PreviewMesh);
	}
}

TArray<FName> UNipcatBasicGameplayFunctionLibrary::GetSkeletonSocketNames(USkeleton* Skeleton)
{
	TArray<FName> Result;
	if (Skeleton)
	{
		Result = Skeleton->GetReferenceSkeleton().GetRawRefBoneNames();
		for (const auto& Socket : Skeleton->Sockets)
		{
			Result.Add(Socket.Get()->SocketName);
		}
	}
	return Result;
}

USceneComponent* UNipcatBasicGameplayFunctionLibrary::GetComponentForAttachedObject(USkeletalMeshComponent* MeshComp, UObject* AttachedObject, const FName AttachName)
{
#if WITH_EDITOR
	if (MeshComp)
	{
		if (!AttachedObject)
		{
			AttachedObject = MeshComp->GetSkeletalMeshAsset()->GetSkeleton()->PreviewAttachedAssetContainer.GetAttachedObjectByAttachName(AttachName);
		}
		if (AttachedObject)
		{
			return PersonaUtils::GetComponentForAttachedObject(MeshComp, AttachedObject, AttachName);
		}
	}
	return nullptr;
#else
	return nullptr;
#endif
}

float UNipcatBasicGameplayFunctionLibrary::GetCameraShakeDuration(const TSubclassOf<UCameraShakeBase>& CameraShakeClass)
{
	if (CameraShakeClass)
	{
		if (const UCameraShakeBase* CDO = CameraShakeClass->GetDefaultObject<UCameraShakeBase>())
		{
			const FCameraShakeDuration Duration = CDO->GetCameraShakeDuration();
			switch (Duration.GetDurationType())
			{
			case ECameraShakeDurationType::Fixed:
			case ECameraShakeDurationType::Custom:
				return Duration.Get();
			case ECameraShakeDurationType::Infinite:
				return -1.f;
			}
		}
	}
	return 0.f;
}

void UNipcatBasicGameplayFunctionLibrary::GetCameraShakeBlendTimes(TSubclassOf<UCameraShakeBase> CameraShakeClass,
	float& OutBlendIn, float& OutBlendOut)
{
	if (CameraShakeClass)
	{
		if (const UCameraShakeBase* CDO = CameraShakeClass->GetDefaultObject<UCameraShakeBase>())
		{
			CDO->GetCameraShakeBlendTimes(OutBlendIn, OutBlendOut);
		}
	}
}

void UNipcatBasicGameplayFunctionLibrary::InitializePathFollowingComponent(
	UPathFollowingComponent* PathFollowingComponent)
{
	PathFollowingComponent->Initialize();
}

void UNipcatBasicGameplayFunctionLibrary::GetAdditiveRefFrameIndex(const UAnimSequence* AnimationSequence,
	int32& RefFrameIndex)
{
	if (AnimationSequence)
	{
		RefFrameIndex = AnimationSequence->RefFrameIndex;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Animation Sequence supplied for GetAdditiveRefFrameIndex"));
	}
}

void UNipcatBasicGameplayFunctionLibrary::SetAdditiveRefFrameIndex(UAnimSequence* AnimationSequence,
                                                                   const int32 RefFrameIndex)
{
	if (AnimationSequence)
	{
		AnimationSequence->RefFrameIndex = RefFrameIndex;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Animation Sequence supplied for SetAdditiveRefFrameIndex"));
	}
}

void UNipcatBasicGameplayFunctionLibrary::GetAdditiveRefPoseSeq(const UAnimSequence* AnimationSequence,
	UAnimSequence*& RefPoseSeq)
{
	if (AnimationSequence)
	{
		RefPoseSeq = AnimationSequence->RefPoseSeq;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Animation Sequence supplied for GetAdditiveRefPoseSeq"));
	}
}

void UNipcatBasicGameplayFunctionLibrary::SetAdditiveRefPoseSeq(UAnimSequence* AnimationSequence,
	UAnimSequence* RefPoseSeq)
{
	if (AnimationSequence)
	{
		AnimationSequence->RefPoseSeq = RefPoseSeq;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid Animation Sequence supplied for GetAdditiveRefPoseSeq"));
	}
}

TArray<FName> UNipcatBasicGameplayFunctionLibrary::GetStaticMeshSocketNames(UStaticMesh* StaticMesh)
{
	TArray<FName> Result;
	if (StaticMesh)
	{
		for (auto& Socket : StaticMesh->Sockets)
		{
			Result.Add(Socket.Get()->SocketName);
		}
	}
	return Result;
}

TArray<FName> UNipcatBasicGameplayFunctionLibrary::GetSkeletalMeshSocketNames(USkeletalMesh* SkeletalMesh)
{
	TArray<FName> Result;
	if (SkeletalMesh)
	{
		for (int32 i = 0; i < SkeletalMesh->NumSockets(); ++i)
		{
			Result.Add(SkeletalMesh->GetSocketByIndex(i)->SocketName);
		}
	}
	return Result;
}

float UNipcatBasicGameplayFunctionLibrary::GetPostProcessVolumeLocalWeight(APostProcessVolume* Volume, FVector& OutViewLocation)
{
	if (!Volume)
	{
		return 0;
	}
	const FPostProcessVolumeProperties VolumeProperties = Volume->GetProperties();
	if (!VolumeProperties.bIsEnabled)
	{
		return 0;
	}
	
	FVector ViewLocation;
	
	if (const APlayerCameraManager* PCM = UGameplayStatics::GetPlayerCameraManager(Volume->GetWorld(), 0))
	{
		ViewLocation = PCM->GetCameraLocation();
	}
	#if WITH_EDITOR
	else
	{
		if (const FViewport* ActiveViewport = GEditor->GetActiveViewport())
		{
			if (const FEditorViewportClient* EditorViewClient = (FEditorViewportClient*)ActiveViewport->GetClient())
			{
				ViewLocation =  EditorViewClient->GetViewLocation();
			}
		}
	}
	#endif

	const FString& Message = FString::Printf(TEXT("%s"), *ViewLocation.ToString());
	GEngine->AddOnScreenDebugMessage(114514, 2.0f, FColor::Orange, Message);
	UE_LOG(LogTemp, Log, TEXT("%s"), *ViewLocation.ToString());
	OutViewLocation = ViewLocation;
	/*
	const TArray<FVector>& ViewLocations = Volume->GetWorld()->ViewLocationsRenderedLastFrame;
	if (ViewLocations.IsEmpty())
	{
		return 0;
	}
	const FVector ViewLocation = ViewLocations[0];
	*/
	
	float DistanceToPoint = 0.0f;
	float LocalWeight = FMath::Clamp(VolumeProperties.BlendWeight, 0.0f, 1.0f);

	ensureMsgf((LocalWeight >= 0 && LocalWeight <= 1.0f), TEXT("Invalid post process blend weight retrieved from volume (%f)"), LocalWeight);

	if (!VolumeProperties.bIsUnbound)
	{
		Volume->EncompassesPoint(ViewLocation, 0.0f, &DistanceToPoint);

		if (DistanceToPoint >= 0)
		{
			if (DistanceToPoint > VolumeProperties.BlendRadius)
			{
				// outside
				LocalWeight = 0.0f;
			}
			else
			{
				// to avoid div by 0
				if (VolumeProperties.BlendRadius >= 1.0f)
				{
					LocalWeight *= 1.0f - DistanceToPoint / VolumeProperties.BlendRadius;

					if(!(LocalWeight >= 0 && LocalWeight <= 1.0f))
					{
						// Mitigate crash here by disabling this volume and generating info regarding the calculation that went wrong.
						ensureMsgf(false, TEXT("Invalid LocalWeight after post process volume weight calculation (Local: %f, DtP: %f, Radius: %f, SettingsWeight: %f)"), LocalWeight, DistanceToPoint, VolumeProperties.BlendRadius, VolumeProperties.BlendWeight);
						LocalWeight = 0.0f;
					}
				}
			}
		}
		else
		{
			LocalWeight = 0;
		}
	}
	else
	{
		LocalWeight = 0;
	}
	return LocalWeight;
}

void UNipcatBasicGameplayFunctionLibrary::RemoveInvalidBlendable(APostProcessVolume* PostProcessVolume)
{
	if (PostProcessVolume)
	{
		TArray<FWeightedBlendable>& WeightedBlendables = PostProcessVolume->Settings.WeightedBlendables.Array;
		for (TArray<FWeightedBlendable>::TIterator It(WeightedBlendables); It; ++It)
		{
			if (!It->Object)
			{
				PostProcessVolume->Modify();
				It.RemoveCurrent();
			}
			else if (It->Object->HasAnyFlags(RF_BeginDestroyed))
			{
				PostProcessVolume->Modify();
				It.RemoveCurrent();
			}
		}
	}
}

float UNipcatBasicGameplayFunctionLibrary::GetWeightedAverageFloat(const TArray<float>& Items,
	const TArray<float>& Weights)
{
	float TotalSum = 0;
	float TotalWeight = 0;
	for (int32 i = 0; i < Weights.Num(); i++)
	{
		if (Items.IsValidIndex(i))
		{
			TotalSum += Items[i] * Weights[i];
			TotalWeight += Weights[i];
		}
	}
	if (TotalWeight == 0)
	{
		return 0;
	}
	return TotalSum / TotalWeight;
}

FLinearColor UNipcatBasicGameplayFunctionLibrary::GetWeightedAverageLinearColor(const TArray<FLinearColor>& Items,
	const TArray<float>& Weights)
{
	FLinearColor TotalSum = FLinearColor();
	float TotalWeight = 0;
	for (int32 i = 0; i < Weights.Num(); i++)
	{
		if (Items.IsValidIndex(i))
		{
			TotalSum += Items[i] * Weights[i];
			TotalWeight += Weights[i];
		}
	}
	if (TotalWeight == 0)
	{
		return TotalSum;
	}
	return TotalSum / TotalWeight;
}

FLinearColor UNipcatBasicGameplayFunctionLibrary::GetWeightedAverageLinearColorUsingHSV(const TArray<FLinearColor>& Items,
	const TArray<float>& Weights)
{
	// 转换到HSV空间
	TArray<FLinearColor> HSVColors;
	for(const FLinearColor& Color : Items)
	{
		HSVColors.Add(Color.LinearRGBToHSV());
	}
    
	// 特殊处理色相
	float SumSin = 0.0f, SumCos = 0.0f;
	float SumS = 0.0f, SumV = 0.0f, SumA = 0.0f;
	float SumWeights = 0.0f;
    
	for(int32 i = 0; i < HSVColors.Num(); i++)
	{
		const float Weight = Weights[i];
		const float Hue = HSVColors[i].R * 2.0f * PI; // 转换到弧度
        
		SumSin += FMath::Sin(Hue) * Weight;
		SumCos += FMath::Cos(Hue) * Weight;
		SumS += HSVColors[i].G * Weight;
		SumV += HSVColors[i].B * Weight;
		SumA += HSVColors[i].A * Weight;
		SumWeights += Weight;
	}

	if (SumWeights == 0)
	{
		return FLinearColor();
	}
    
	// 计算平均HSV
	float ResultH = FMath::Atan2(SumSin, SumCos) / (2.0f * PI);
	if(ResultH < 0.0f) ResultH += 1.0f;
	const float ResultS = SumS / SumWeights;
	const float ResultV = SumV / SumWeights;
	const float ResultA = SumA / SumWeights;
    
	// 转回RGB空间
	const FLinearColor Result(ResultH, ResultS, ResultV, ResultA);
	return Result.HSVToLinearRGB();
}

TArray<FName> UNipcatBasicGameplayFunctionLibrary::GetModifiedPropertyNames(UObject* Object, TArray<FName> Properties)
{
	TArray<FName> ModifiedPropertyNames;
	if (!Object)
	{
		return ModifiedPropertyNames;
	}
	for (auto PropertyName : Properties)
	{
		if (const FProperty* ObjectProp = PropertyAccessUtil::FindPropertyByName(PropertyName, Object->GetClass()))
		{
			if (const UObject* ObjectArchetype = Object->GetArchetype())
			{
				const void* ValuePtr = ObjectProp->ContainerPtrToValuePtr<void>(Object);
				const void* ArchetypeValuePtr = ObjectProp->ContainerPtrToValuePtrForDefaults<const void>(ObjectArchetype->GetClass(), ObjectArchetype);
				if (!ObjectProp->Identical(ValuePtr, ArchetypeValuePtr))
				{
					ModifiedPropertyNames.Add(PropertyName);
				}
			}
		}
	}
	return ModifiedPropertyNames;
}

TArray<FName> UNipcatBasicGameplayFunctionLibrary::GetModifiedPropertyNamesDiffAgainstOther(UObject* Object,
	UObject* DiffAgainstObject, TArray<FName> Properties)
{
	TArray<FName> ModifiedPropertyNames;
	if (!Object || !DiffAgainstObject)
	{
		return ModifiedPropertyNames;
	}
	for (auto PropertyName : Properties)
	{
		if (const FProperty* ObjectProp = PropertyAccessUtil::FindPropertyByName(PropertyName, Object->GetClass()))
		{
			if (const FProperty* OtherObjectProp = PropertyAccessUtil::FindPropertyByName(PropertyName, DiffAgainstObject->GetClass()))
			{
				const void* ValuePtr = ObjectProp->ContainerPtrToValuePtr<void>(Object);
				const void* OtherValuePtr = OtherObjectProp->ContainerPtrToValuePtr<void>(DiffAgainstObject);
				if (!ObjectProp->Identical(ValuePtr, OtherValuePtr))
				{
					ModifiedPropertyNames.Add(PropertyName);
				}
			}
		}
	}
	return ModifiedPropertyNames;
}

TArray<FName> UNipcatBasicGameplayFunctionLibrary::GetAllInstanceEditableProperties(const UObject* Object)
{
	TArray<FName> Result;
	if (Object)
	{
		for (TFieldIterator<FProperty> PropertyIt(Object->GetClass()); PropertyIt; ++PropertyIt)
		{
			const FProperty* Property = *PropertyIt;
			
			// skip non-public properties
			const bool bIsPublic = Property->HasAnyPropertyFlags(CPF_Edit | CPF_EditConst) && !Property->HasAnyPropertyFlags(CPF_NativeAccessSpecifierPrivate);
			const bool bIsInstanceEditable = !Property->HasAnyPropertyFlags(CPF_DisableEditOnInstance);
			if(!bIsPublic || !bIsInstanceEditable)
			{
				continue;
			}

			Result.Add(Property->GetFName());
			/*
			const FString CPPType = Property->GetCPPType();
			bool bIsVector;
			if (CPPType == TEXT("FVector"))
			{
				bIsVector = true;
			}
			else if (CPPType == TEXT("FTransform"))
			{
				bIsVector = false;
			}*/
		}
	}
	return Result;
}

TArray<FName> UNipcatBasicGameplayFunctionLibrary::GetAllInstanceNotEditableProperties(const UObject* Object)
{
	TArray<FName> Result;
	if (Object)
	{
		for (TFieldIterator<FProperty> PropertyIt(Object->GetClass()); PropertyIt; ++PropertyIt)
		{
			const FProperty* Property = *PropertyIt;
			
			const bool bIsPublic = Property->HasAnyPropertyFlags(CPF_Edit | CPF_EditConst) && !Property->HasAnyPropertyFlags(CPF_NativeAccessSpecifierPrivate);
			const bool bIsInstanceEditable = !Property->HasAnyPropertyFlags(CPF_DisableEditOnInstance);
			if(!bIsPublic || !bIsInstanceEditable)
			{
				Result.Add(Property->GetFName());
			}
		}
	}
	return Result;
}

TArray<FName> UNipcatBasicGameplayFunctionLibrary::GetAllTransientProperties(const UObject* Object)
{
	TArray<FName> Result;
	if (Object)
	{
		for (TFieldIterator<FProperty> PropertyIt(Object->GetClass()); PropertyIt; ++PropertyIt)
		{
			const FProperty* Property = *PropertyIt;
			
			if(Property->HasAnyPropertyFlags(CPF_Transient))
			{
				Result.Add(Property->GetFName());
			}
		}
	}
	return Result;
}

bool UNipcatBasicGameplayFunctionLibrary::HasActorBegunPlay(AActor* Actor)
{
	if (Actor)
	{
		return Actor->HasActorBegunPlay();
	}
	return false;
}

bool UNipcatBasicGameplayFunctionLibrary::IfWithEditor()
{
#if WITH_EDITOR
	return true;
#else
	return false;
#endif
}

bool UNipcatBasicGameplayFunctionLibrary::IfUEBuildShipping()
{
#if UE_BUILD_SHIPPING
	return true;
#else
	return false;
#endif
}

void UNipcatBasicGameplayFunctionLibrary::SeamlessClientTravel(APlayerController* PlayerController, FString URL)
{
	PlayerController->ClientTravel(URL, TRAVEL_Relative, true);
}

APlayerController* UNipcatBasicGameplayFunctionLibrary::GetPlayerControllerFromActor(AActor* Actor)
{
	if (Actor)
	{
		if (const auto Pawn = Cast<APawn>(Actor))
		{
			if (const auto Controller = Pawn->GetController())
			{
				return Cast<APlayerController>(Controller);
			}
		}
		if (const auto PC = Cast<APlayerController>(Actor))
		{
			return PC;
		}
		if (const auto PS = Cast<APlayerState>(Actor)) 
		{
			return PS->GetPlayerController();
		}
	}
	return nullptr;
}

APlayerController* UNipcatBasicGameplayFunctionLibrary::GetPlayerControllerFromLocalPlayerSubsystem(
	const ULocalPlayerSubsystem* Subsystem)
{
	if (Subsystem)
	{
		if (const auto LocalPlayer = Subsystem->GetLocalPlayer())
		{
			return LocalPlayer->GetPlayerController(nullptr);
		}
	}
	return nullptr;
}

bool UNipcatBasicGameplayFunctionLibrary::GetPrimaryAssetDataList(FPrimaryAssetType PrimaryAssetType,
	TArray<FAssetData>& AssetDataList)
{
	const auto& AssetManager = UAssetManager::Get();
	return AssetManager.GetPrimaryAssetDataList(PrimaryAssetType, AssetDataList);
}

FPrimaryAssetId UNipcatBasicGameplayFunctionLibrary::GetPrimaryAssetIdFromAssetData(const FAssetData& AssetData)
{
	return UAssetManager::Get().GetPrimaryAssetIdForPath(AssetData.GetSoftObjectPath());
}

bool UNipcatBasicGameplayFunctionLibrary::SaveAsset(UObject* Object)
{
#if WITH_EDITOR
	if (!Object)
	{
		return false;
	}

	const FString PackageName = Object->GetPathName();
	UPackage* Package = LoadPackage(nullptr, *PackageName, LOAD_None);
	const FString PackageFileName = FPackageName::LongPackageNameToFilename(Package->GetName(), FPackageName::GetAssetPackageExtension());
#if ENGINE_MAJOR_VERSION == 5
	FSavePackageArgs SavePackageArgs;
	SavePackageArgs.TopLevelFlags = EObjectFlags::RF_Public | EObjectFlags::RF_Standalone;
	SavePackageArgs.Error = GError;
	SavePackageArgs.SaveFlags = SAVE_NoError;
	SavePackageArgs.bWarnOfLongFilename = true;
	return UPackage::SavePackage(Package, Object, *PackageFileName, SavePackageArgs);
#else
	return UPackage::SavePackage(Package, Object, EObjectFlags::RF_Public | EObjectFlags::RF_Standalone, *PackageFileName, GError, nullptr, false, true, SAVE_NoError);
#endif
#else
	return false;
#endif
}

FName UNipcatBasicGameplayFunctionLibrary::GetValidDataTableRowName(const UDataTable* DataTable)
{
	// Base row name to add
	FName NewRowName = DataTableUtils::MakeValidName(TEXT("NewRow"));

	// Keep adding 1 to the postfix number if the name already exists
	while (DataTable->GetRowMap().Contains(NewRowName))
	{
		NewRowName.SetNumber(NewRowName.GetNumber() + 1);
	}

	return NewRowName;
}

FName UNipcatBasicGameplayFunctionLibrary::AddDataTableRow(UDataTable* DataTable, FTableRowBase Row, FName RowName, bool bSave)
{
	// This should never run since the function uses a custom thunk
	check(0);
	return NAME_None;
}

FName UNipcatBasicGameplayFunctionLibrary::Generic_AddDataTableRow(UDataTable* DataTable, void* Row, FName RowName, bool bSave)
{
#if WITH_EDITOR
	if (!DataTable)
	{
		return NAME_None;
	}

	if (Row)
	{
		// Base row name to add
		const FName NewRowName = RowName == NAME_None || RowName == FName("") ? GetValidDataTableRowName(DataTable) : RowName;

		// Cast row to table row base
		const FTableRowBase* TableRowBase = static_cast<FTableRowBase*>(Row);

		// Add row
		DataTable->AddRow(NewRowName, *TableRowBase);

		// Save data table asset
		if (bSave) SaveAsset(DataTable);

		// Update data table view
		FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);

		return NewRowName;
	}

	return NAME_None;

#else
	return false;
#endif
}

DEFINE_FUNCTION(UNipcatBasicGameplayFunctionLibrary::execAddDataTableRow)
{
	P_GET_OBJECT(UDataTable, DataTable);
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* RowPtr = Stack.MostRecentPropertyAddress;
	const FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	P_GET_PROPERTY(FNameProperty, RowName);
	P_GET_UBOOL(bSave);
	P_FINISH;

	if (!DataTable)
	{
		const FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, NSLOCTEXT("AddDataTableRow", "MissingTableInput", "Failed to resolve the table input. Be sure the DataTable is valid."));
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
	else if (StructProp && RowPtr)
	{
		const UScriptStruct* OutputType = StructProp->Struct;
		const UScriptStruct* TableType = DataTable->GetRowStruct();

		const bool bCompatible = (OutputType == TableType) || (OutputType->IsChildOf(TableType) && FStructUtils::TheSameLayout(OutputType, TableType));
		if (bCompatible)
		{
			P_NATIVE_BEGIN;
				*(FName*)RESULT_PARAM = Generic_AddDataTableRow(DataTable, RowPtr, RowName, bSave);
			P_NATIVE_END;
		}
		else
		{
			const FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, NSLOCTEXT("AddDataTableRow", "IncompatibleProperty", "Incompatible output parameter; the data table's type is not the same as the return type."));
			FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
		}
	}
	else
	{
		const FBlueprintExceptionInfo ExceptionInfo(EBlueprintExceptionType::AccessViolation, NSLOCTEXT("AddDataTableRow", "MissingOutputProperty", "Failed to resolve the output parameter for AddDataTableRow."));
		FBlueprintCoreDelegates::ThrowScriptException(P_THIS, Stack, ExceptionInfo);
	}
}

void UNipcatBasicGameplayFunctionLibrary::RemoveDataTableRow(UDataTable* DataTable, FName RowName, bool bSave)
{
#if WITH_EDITOR
	if (DataTable)
	{
		DataTable->RemoveRow(RowName);
		if (bSave)
		{
			SaveAsset(DataTable);
		}
		
		// Update data table view
		FDataTableEditorUtils::FDataTableEditorManager::Get().PostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
	}
#endif
}

void UNipcatBasicGameplayFunctionLibrary::SetDataTableRowStruct(UDataTable* DataTable, UScriptStruct* Struct)
{
	if (DataTable)
	{
		DataTable->RowStruct = Struct;
	}
}

DEFINE_FUNCTION(UNipcatBasicGameplayFunctionLibrary::execMakeScriptStruct)
{
	// 1. 准备读取堆栈中的下一个属性（即 InWildcard 参数）
	Stack.MostRecentProperty = nullptr;
	Stack.MostRecentPropertyAddress = nullptr;

	// 2. 执行步进（Step），这会消耗掉堆栈中的一个参数
	// 对于通配符参数，我们不使用 P_GET_xxx 宏，而是直接 Step
	Stack.Step(Stack.Object, NULL);

	// 3. 获取刚才步进过的属性信息
	FProperty* Prop = Stack.MostRecentProperty;
	void* PropAddr = Stack.MostRecentPropertyAddress;

	// 4. 准备返回值
	UScriptStruct* ResultStruct = nullptr;

	// 5. 检查属性是否存在且是否为结构体属性
	if (Prop)
	{
		// 尝试将其转换为结构体属性
		FStructProperty* StructProp = CastField<FStructProperty>(Prop);
        
		if (StructProp)
		{
			// 如果是结构体，获取其类型对象 (UScriptStruct)
			ResultStruct = StructProp->Struct;
		}
		else
		{
			// 它是其他类型（如 Int, Float, Object 等），这里可以打印警告或处理错误
			// UE_LOG(LogTemp, Warning, TEXT("Input is not a Struct! Type: %s"), *Prop->GetClass()->GetName());
		}
	}

	// 6. 结束参数读取（必须要有这一步，表示参数解析完成）
	P_FINISH;

	// 7. 设置返回值
	*(UScriptStruct**)RESULT_PARAM = (UScriptStruct*)ResultStruct;
}

void UNipcatBasicGameplayFunctionLibrary::Generic_GetRandomItemByWeight(void* ItemsAddr, FArrayProperty* ItemsProperty,
                                                                        const TArray<float>& Weights, void* ReturnValueAddr, FProperty* ReturnValueProperty, int32& OutIndex)
{
	FScriptArrayHelper ArrayHelper(ItemsProperty, ItemsAddr);
	if (ArrayHelper.Num() == 0)
	{
		return;
	}
	
	if (ArrayHelper.Num() == 1)
	{
		OutIndex = 0;
		ReturnValueProperty->CopySingleValue(ReturnValueAddr, ArrayHelper.GetRawPtr(0));
		return;
	}
	
	TArray<float> AccumulateWeight;
	for (int i = 0; i < ArrayHelper.Num(); i++)
	{
		AccumulateWeight.Emplace(AccumulateWeight.IsEmpty() ? Weights[i] : AccumulateWeight.Last() + Weights[i]);
	}
	 
	const float RandomFloat = FMath::RandRange(0.0f, AccumulateWeight.Last());
	for (int i = 0; i < AccumulateWeight.Num(); i++)
	{
		if (RandomFloat <= AccumulateWeight[i])
		{
			OutIndex = i;
			ReturnValueProperty->CopySingleValue(ReturnValueAddr, ArrayHelper.GetRawPtr(i));
			return;
		}
	}
}

#if WITH_EDITOR
void UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(FString& CommentString,
	const TSoftClassPtr<>& ClassObject, const bool& bCommentModified)
{
#if WITH_EDITORONLY_DATA
	if (!bCommentModified)
	{
		CommentString = GetClassObjectName(ClassObject);
	}
#endif
}

void UNipcatBasicGameplayFunctionLibrary::TryUpdateComment(FString& CommentString, const FGameplayTag& Tag,
	const bool& bCommentModified)
{
#if WITH_EDITORONLY_DATA
	if (!bCommentModified)
	{
		CommentString = Tag.ToString();
	}
#endif
}
#endif

FString UNipcatBasicGameplayFunctionLibrary::GetClassObjectName(const TSoftClassPtr<UObject>& ClassObject)
{
	FString AbilityClassPath = ClassObject.ToSoftObjectPath().GetAssetPathString();
	AbilityClassPath.RemoveFromEnd(TEXT("_C"));
	FString ClassName;
	AbilityClassPath.Split(TEXT("."), nullptr, &ClassName);
	return ClassName;
}

FString UNipcatBasicGameplayFunctionLibrary::GetActorLevelName(const AActor* Actor)
{
	return Actor->GetLevel()->GetOuter()->GetName();
}

FVector UNipcatBasicGameplayFunctionLibrary::GetFeetLocation(const UNavMovementComponent* NavMovementComponent)
{
	return NavMovementComponent->GetFeetLocation();
}

UNipcatCustomMovementMode* UNipcatBasicGameplayFunctionLibrary::FindCustomMovementMode(
	const UCharacterMovementComponent* CharacterMovementComponent, uint8 InCustomMovementMode)
{
	if (CharacterMovementComponent)
	{
		if (const auto NipcatCMC = Cast<UNipcatCharacterMovementComponent>(CharacterMovementComponent))
		{
			return NipcatCMC->FindCustomMovementMode(InCustomMovementMode);
		}
	}
	return nullptr;
}

UNipcatCustomMovementMode* UNipcatBasicGameplayFunctionLibrary::FindCustomMovementModeByTag(
	const UCharacterMovementComponent* CharacterMovementComponent, FGameplayTag InCustomMovementModeTag)
{
	if (CharacterMovementComponent)
	{
		if (const auto NipcatCMC = Cast<UNipcatCharacterMovementComponent>(CharacterMovementComponent))
		{
			return NipcatCMC->FindCustomMovementModeByTag(InCustomMovementModeTag);
		}
	}
	return nullptr;
}

void UNipcatBasicGameplayFunctionLibrary::SetCustomMovementMode(UCharacterMovementComponent* CharacterMovementComponent,
	FGameplayTag InCustomMovementModeTag)
{
	if (CharacterMovementComponent)
	{
		for (auto [Index, Tag] : UNipcatBasicGameplayDeveloperSettings::Get()->CustomMovementModeDefinitions)
		{
			if (Tag == InCustomMovementModeTag)
			{
				CharacterMovementComponent->SetMovementMode(MOVE_Custom, Index);
				return;
			}
		}
	}
}

ACharacter* UNipcatBasicGameplayFunctionLibrary::GetCharacter(const AController* Controller)
{
	if (Controller)
	{
		return Controller->GetCharacter();
	}
	return nullptr;
}

void UNipcatBasicGameplayFunctionLibrary::FilterObjectArray(const TArray<UObject*>& TargetArray,
	TSubclassOf<UObject> FilterClass, TArray<UObject*>& FilteredArray)
{
	FilteredArray.Empty();
	for (auto It = TargetArray.CreateConstIterator(); It; It++)
	{
		UObject* TargetElement = (*It);
		if (TargetElement && TargetElement->IsA(FilterClass))
		{
			FilteredArray.Add(TargetElement);
		}
	}
}

// Parser property access implementations
#include "UObject/PropertyAccessUtil.h"
#include "Kismet/KismetSystemLibrary.h"

// Define General Log Category for Parser functions
#define LOCTEXT_NAMESPACE "NipcatBasicGameplayFunctionLibrary"

// Generic property access implementation
bool UNipcatBasicGameplayFunctionLibrary::Generic_GetPropertyByReflection(UObject* OwnerObject, FName PropertyName, void*& OutPropertyAddr, FProperty*& OutProperty)
{
	if (!OwnerObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters in GetPropertyByReflection"));
		return false;
	}

	// Use PropertyAccessUtil for more robust property finding
	OutProperty = PropertyAccessUtil::FindPropertyByName(PropertyName, OwnerObject->GetClass());
	if (!OutProperty)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find property '%s' in object '%s'"), 
			*PropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
		return false;
	}

	// Get operation: copy from FoundProp to OutProperty
	void* Src = OutProperty->ContainerPtrToValuePtr<void>(OwnerObject);
	if (OutPropertyAddr)
	{
		OutProperty->CopySingleValue(OutPropertyAddr, Src);
	}
	else
	{
		OutPropertyAddr = Src;
	}
	UE_LOG(LogTemp, Verbose, TEXT("Get property '%s' from object '%s'"), 
		*PropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
	
	return true;
}

bool UNipcatBasicGameplayFunctionLibrary::Generic_SetPropertyByReflection(UObject* OwnerObject, FName PropertyName, const void* SrcPropertyAddr)
{
	if (!OwnerObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters in SetPropertyByReflection"));
		return false;
	}

	// Use PropertyAccessUtil for more robust property finding
	FProperty* FoundProp = PropertyAccessUtil::FindPropertyByName(PropertyName, OwnerObject->GetClass());
	if (!FoundProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find property '%s' in object '%s'"), 
			*PropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
		return false;
	}

	// Set operation: copy from Src to FoundProp
	void* Dest = FoundProp->ContainerPtrToValuePtr<void>(OwnerObject);
	FoundProp->CopySingleValue(Dest, SrcPropertyAddr);
	UE_LOG(LogTemp, Verbose, TEXT("Set property '%s' in object '%s'"), 
		*PropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
	
	return true;
}

// Generic array access implementation
bool UNipcatBasicGameplayFunctionLibrary::Generic_GetArrayByReflection(UObject* OwnerObject, FName ArrayPropertyName, void*& OutArrayAddr, FArrayProperty*& OutArrayProperty)
{
	if (!OwnerObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters in GetArrayByReflection"));
		return false;
	}

	OutArrayProperty = CastField<FArrayProperty>(PropertyAccessUtil::FindPropertyByName(ArrayPropertyName, OwnerObject->GetClass()));
	if (!OutArrayProperty)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find array property '%s' in object '%s'"), 
			*ArrayPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
		return false;
	}

	// Get operation: copy entire array
	void* Src = OutArrayProperty->ContainerPtrToValuePtr<void>(OwnerObject);
	if (OutArrayAddr)
	{
		OutArrayProperty->CopyValuesInternal(OutArrayAddr, Src, 1);
	}
	else
	{
		OutArrayAddr = Src;
	}
	UE_LOG(LogTemp, Verbose, TEXT("Get array property '%s' from object '%s'"), 
		*ArrayPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
	
	return true;
}

bool UNipcatBasicGameplayFunctionLibrary::Generic_SetArrayByReflection(UObject* OwnerObject, FName ArrayPropertyName, const void* SrcArrayAddr)
{
	if (!OwnerObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters in SetArrayByReflection"));
		return false;
	}

	FArrayProperty* ArrayProp = CastField<FArrayProperty>(PropertyAccessUtil::FindPropertyByName(ArrayPropertyName, OwnerObject->GetClass()));
	if (!ArrayProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find array property '%s' in object '%s'"), 
			*ArrayPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
		return false;
	}

	// Set operation: copy entire array
	void* Dest = ArrayProp->ContainerPtrToValuePtr<void>(OwnerObject);
	ArrayProp->CopyValuesInternal(Dest, SrcArrayAddr, 1);
	UE_LOG(LogTemp, Verbose, TEXT("Set array property '%s' in object '%s'"), 
		*ArrayPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
	
	return true;
}

// Generic set access implementation
bool UNipcatBasicGameplayFunctionLibrary::Generic_GetSetByReflection(UObject* OwnerObject, FName SetPropertyName, void*& OutSetAddr, FSetProperty*& OutSetProperty)
{
	if (!OwnerObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters in GetSetByReflection"));
		return false;
	}

	OutSetProperty = CastField<FSetProperty>(PropertyAccessUtil::FindPropertyByName(SetPropertyName, OwnerObject->GetClass()));
	if (!OutSetProperty)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find set property '%s' in object '%s'"), 
			*SetPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
		return false;
	}

	// Get operation: copy entire set
	void* Src = OutSetProperty->ContainerPtrToValuePtr<void>(OwnerObject);
	if (OutSetAddr)
	{
		OutSetProperty->CopyValuesInternal(OutSetAddr, Src, 1);
	}
	else
	{
		OutSetAddr = Src;
	}
	UE_LOG(LogTemp, Verbose, TEXT("Get set property '%s' from object '%s'"), 
		*SetPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
	
	return true;
}

bool UNipcatBasicGameplayFunctionLibrary::Generic_SetSetByReflection(UObject* OwnerObject, FName SetPropertyName, const void* SrcSetAddr)
{
	if (!OwnerObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters in SetSetByReflection"));
		return false;
	}

	FSetProperty* SetProp = CastField<FSetProperty>(PropertyAccessUtil::FindPropertyByName(SetPropertyName, OwnerObject->GetClass()));
	if (!SetProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find set property '%s' in object '%s'"), 
			*SetPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
		return false;
	}
	
	// Set operation: copy entire set
	void* Dest = SetProp->ContainerPtrToValuePtr<void>(OwnerObject);
	SetProp->CopyValuesInternal(Dest, SrcSetAddr, 1);
	UE_LOG(LogTemp, Verbose, TEXT("Set set property '%s' in object '%s'"), 
		*SetPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
	
	return true;
}

// Generic map access implementation
bool UNipcatBasicGameplayFunctionLibrary::Generic_GetMapByReflection(UObject* OwnerObject, FName MapPropertyName, void*& OutMapAddr, FMapProperty*& OutMapProperty)
{
	if (!OwnerObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters in GetMapByReflection"));
		return false;
	}

	OutMapProperty = CastField<FMapProperty>(PropertyAccessUtil::FindPropertyByName(MapPropertyName, OwnerObject->GetClass()));
	if (!OutMapProperty)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find map property '%s' in object '%s'"), 
			*MapPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
		return false;
	}
	
	// Get operation: copy entire map
	void* Src = OutMapProperty->ContainerPtrToValuePtr<void>(OwnerObject);
	if (OutMapAddr)
	{
		OutMapProperty->CopyValuesInternal(OutMapAddr, Src, 1);
	}
	else
	{
		OutMapAddr = Src;
	}
	UE_LOG(LogTemp, Verbose, TEXT("Get map property '%s' from object '%s'"), 
		*MapPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
	
	return true;
}

bool UNipcatBasicGameplayFunctionLibrary::Generic_SetMapByReflection(UObject* OwnerObject, FName MapPropertyName, const void* SrcMapAddr)
{
	if (!OwnerObject)
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid parameters in SetMapByReflection"));
		return false;
	}

	FMapProperty* MapProp = CastField<FMapProperty>(PropertyAccessUtil::FindPropertyByName(MapPropertyName, OwnerObject->GetClass()));
	if (!MapProp)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find map property '%s' in object '%s'"), 
			*MapPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
		return false;
	}
	
	// Set operation: copy entire map
	void* Dest = MapProp->ContainerPtrToValuePtr<void>(OwnerObject);
	MapProp->CopyValuesInternal(Dest, SrcMapAddr, 1);
	UE_LOG(LogTemp, Verbose, TEXT("Set map property '%s' in object '%s'"), 
		*MapPropertyName.ToString(), *UKismetSystemLibrary::GetDisplayName(OwnerObject));
	
	return true;
}

// Safe property access implementations
bool UNipcatBasicGameplayFunctionLibrary::ParserSafeGetProperty(UObject* Object, FName PropertyName, int32& OutValue)
{
	if (!Object)
	{
		return false;
	}

	FProperty* FoundProp = PropertyAccessUtil::FindPropertyByName(PropertyName, Object->GetClass());
	if (!FoundProp)
	{
		return false;
	}

	// Check if it's an int32 property
	if (!FoundProp->IsA<FIntProperty>())
	{
		return false;
	}

	void* ValuePtr = FoundProp->ContainerPtrToValuePtr<void>(Object);
	OutValue = *static_cast<int32*>(ValuePtr);
	return true;
}

bool UNipcatBasicGameplayFunctionLibrary::ParserSafeSetProperty(UObject* Object, FName PropertyName, const int32& Value)
{
	if (!Object)
	{
		return false;
	}

	FProperty* FoundProp = PropertyAccessUtil::FindPropertyByName(PropertyName, Object->GetClass());
	if (!FoundProp)
	{
		return false;
	}

	// Check if it's an int32 property
	if (!FoundProp->IsA<FIntProperty>())
	{
		return false;
	}

	void* ValuePtr = FoundProp->ContainerPtrToValuePtr<void>(Object);
	*static_cast<int32*>(ValuePtr) = Value;
	return true;
}

bool UNipcatBasicGameplayFunctionLibrary::ParserGetPropertyInfo(UObject* Object, FName PropertyName, FString& OutTypeName, int32& OutSize)
{
	if (!Object)
	{
		return false;
	}

	FProperty* FoundProp = PropertyAccessUtil::FindPropertyByName(PropertyName, Object->GetClass());
	if (!FoundProp)
	{
		return false;
	}

	OutTypeName = FoundProp->GetClass()->GetName();
	OutSize = FoundProp->GetSize();
	return true;
}

#undef LOCTEXT_NAMESPACE
