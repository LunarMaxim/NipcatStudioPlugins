// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatLocomotionSystemEditorFunctionLibrary.h"

#include "AssetToolsModule.h"
#include "AnimationModifier.h"
#include "Kismet/KismetMathLibrary.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"

#if WITH_EDITOR

#define LOCTEXT_NAMESPACE "UNipcatLocomotionSystemEditorFunctionLibrary"

void UNipcatLocomotionSystemEditorFunctionLibrary::ApplyAnimationModifierToAnimSequence(UAnimSequence* AnimSequence, TSubclassOf<UAnimationModifier> AnimationModifier)
{
	if (AnimSequence && IsValid(AnimationModifier))
	{
		AnimationModifier.GetDefaultObject()->ApplyToAnimationSequence(AnimSequence);
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::SetCurveCompressionSettings(UAnimSequence* AnimSequence, UAnimCurveCompressionSettings* CurveCompressionSettings)
{
	AnimSequence->CurveCompressionSettings = CurveCompressionSettings;
}

void UNipcatLocomotionSystemEditorFunctionLibrary::AddNewVirtualBone(USkeleton* Skeleton, const FName SourceBoneName, const FName TargetBoneName, FName NewVirtualBoneName)
{
	FName OutVirtualBoneName;
	Skeleton->AddNewVirtualBone(SourceBoneName, TargetBoneName, OutVirtualBoneName);
	Skeleton->RenameVirtualBone(OutVirtualBoneName, NewVirtualBoneName);
}

void UNipcatLocomotionSystemEditorFunctionLibrary::RemoveVirtualBone(USkeleton* Skeleton, const FName VirtualBoneName)
{
	TArray<FName> BoneNameArray;
	BoneNameArray.Add(VirtualBoneName);
	Skeleton->RemoveVirtualBones(BoneNameArray);
}

void UNipcatLocomotionSystemEditorFunctionLibrary::ExportMontageTrackAnimation(UAnimMontage* AnimMontage)
{
	for (FSlotAnimationTrack& SlotAnimationTrack : AnimMontage->SlotAnimTracks)
	{
		FAnimTrack& AnimTrack = SlotAnimationTrack.AnimTrack;
		if (AnimTrack.AnimSegments.Num() <= 1)
		{
			continue;
		}
		TArray<FAnimSegment> AnimSegmentStack;
		for (FAnimSegment& AnimSegment : AnimTrack.AnimSegments)
		{
			if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(AnimSegment.GetAnimReference()))
			{
				AnimSegmentStack.Insert(AnimSegment, 0);
			}
		}
		while (AnimSegmentStack.Num() > 1)
		{
			const FAnimSegment& AnimSegment1 = AnimSegmentStack.Pop();
			const FAnimSegment& AnimSegment2 = AnimSegmentStack.Pop();
			const FName DuplicatedObjectName1 = MakeUniqueObjectName(GetTransientPackage(), UAnimSequence::StaticClass());
			const FName DuplicatedObjectName2 = MakeUniqueObjectName(GetTransientPackage(), UAnimSequence::StaticClass());
			UAnimSequence* DuplicatedAnimSequence1 = Cast<UAnimSequence>(StaticDuplicateObject(AnimSegment1.GetAnimReference(), GetTransientPackage(), DuplicatedObjectName1));
			UAnimSequence* DuplicatedAnimSequence2 = Cast<UAnimSequence>(StaticDuplicateObject(AnimSegment2.GetAnimReference(), GetTransientPackage(), DuplicatedObjectName2));

			const IAnimationDataModel* Model1 = DuplicatedAnimSequence1->GetDataModel();
			const IAnimationDataModel* Model2 = DuplicatedAnimSequence2->GetDataModel();
			FFrameRate FrameRate1 = Model1->GetFrameRate();
			FFrameRate FrameRate2 = Model2->GetFrameRate();
			TrimAnimation(DuplicatedAnimSequence1,
				FrameRate1.AsFrameTime(AnimSegment1.AnimStartTime).FrameNumber.Value,
				FrameRate1.AsFrameTime(AnimSegment1.AnimEndTime).FrameNumber.Value);
			TrimAnimation(DuplicatedAnimSequence2,
				FrameRate2.AsFrameTime(AnimSegment2.AnimStartTime).FrameNumber.Value,
				FrameRate2.AsFrameTime(AnimSegment2.AnimEndTime).FrameNumber.Value);
			
			AdjustAnimationSpeed(DuplicatedAnimSequence1, AnimSegment1.AnimPlayRate);
			AdjustAnimationSpeed(DuplicatedAnimSequence2, AnimSegment2.AnimPlayRate);
			
			AppendAnimation(DuplicatedAnimSequence1, DuplicatedAnimSequence2);
			FAnimSegment NewSegment = FAnimSegment();
			NewSegment.SetAnimReference(DuplicatedAnimSequence1);
			AnimSegmentStack.Emplace(NewSegment);
		}
		
		if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(AnimSegmentStack.Pop().GetAnimReference()))
		{
			const FAssetToolsModule& AssetToolsModule = FModuleManager::Get().LoadModuleChecked<FAssetToolsModule>("AssetTools");
			FString AssetName = AnimMontage->GetName();
			AssetName = AssetName.Replace(TEXT("_Montage"), TEXT(""));
			AssetName = AssetName.Replace(TEXT("AM_"), TEXT(""));
			AssetName.Append(TEXT("_") + SlotAnimationTrack.SlotName.ToString());
			UAnimSequence* NewAnimSequence = Cast<UAnimSequence>(AssetToolsModule.Get().DuplicateAsset(AssetName,
						FPackageName::GetLongPackagePath(AnimMontage->GetOutermost()->GetName()), AnimSequence));
			FAnimSegment NewSegment;
			NewSegment.SetAnimReference(NewAnimSequence);
			SlotAnimationTrack.AnimTrack.AnimSegments[0].SetAnimReference(NewAnimSequence, true);
			for (auto It = SlotAnimationTrack.AnimTrack.AnimSegments.CreateIterator(); It; ++It)
			{
				if (It.GetIndex() == 0)
				{
					continue;
				}
				It.RemoveCurrent();
			}
		}
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::TrimAnimation(UAnimSequence* AnimSequence, int StartFrame,
	int EndFrame)
{
	if (IsValid(AnimSequence) == false)
	{
		return;
	}

	USkeleton* Skeleton = AnimSequence->GetSkeleton();
	const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
	int32 NumBones = RefSkeleton.GetNum();

	int32 NumKeys = AnimSequence->GetDataModel()->GetNumberOfKeys();
	if (NumKeys == 0)
	{
		return;
	}

	EndFrame = FMath::Min(EndFrame, NumKeys - 1);
	if (EndFrame <= 0)
	{
		return;
	}

	StartFrame = FMath::Min(StartFrame, EndFrame - 1);
	if (StartFrame < 0)
	{
		return;
	}

	UAnimSequence* AnimOrig = NewObject<UAnimSequence>();
	AnimOrig->SetSkeleton(AnimSequence->GetSkeleton());
	AnimOrig->CreateAnimation(AnimSequence);

	{
		FScopedTransaction Transation(LOCTEXT("TrimAnimation", "TrimAnimation"));
		AnimSequence->Modify();
		AnimSequence->GetController().OpenBracket(FText::FromString(TEXT("TrimAnimation")));

		double IntervalTime = AnimSequence->GetDataModel()->GetFrameRate().AsInterval();

		int TotalFrames = (EndFrame + 1) - StartFrame;

		AnimSequence->GetController().SetNumberOfFrames(TotalFrames - 1);

		{
			int32 RootBoneIndex = 0;
			FName RootBoneName = RefSkeleton.GetBoneName(RootBoneIndex);
			
			if (AnimSequence->GetDataModel()->IsValidBoneTrackName(RootBoneName))
			{
				TArray< FVector > AnimTrackPosKeys;
				TArray< FQuat > AnimTrackRotKeys;
				TArray< FVector > AnimTrackScaleKeys;
				AnimTrackPosKeys.Empty(TotalFrames);
				AnimTrackRotKeys.Empty(TotalFrames);
				AnimTrackScaleKeys.Empty(TotalFrames);

				FTransform StartKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, StartFrame, AnimOrig->Interpolation);
				
				for (int32 key = StartFrame; key <= EndFrame; key++)
				{
					FTransform AnimatedLocalKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, key, AnimOrig->Interpolation);
					
					AnimatedLocalKey = AnimatedLocalKey * StartKey.Inverse();
					AnimatedLocalKey.NormalizeRotation();

					AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
					AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
					AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());
				}

				AnimSequence->GetController().SetBoneTrackKeys(RootBoneName, AnimTrackPosKeys, AnimTrackRotKeys, AnimTrackScaleKeys);
			}
		}

		for (int32 BoneIndex = 1; BoneIndex < NumBones; BoneIndex++)
		{
			FName BoneName = RefSkeleton.GetBoneName(BoneIndex);
			
			if (AnimSequence->GetDataModel()->IsValidBoneTrackName(BoneName))
			{
				TArray< FVector > AnimTrackPosKeys;
				TArray< FQuat > AnimTrackRotKeys;
				TArray< FVector > AnimTrackScaleKeys;
				AnimTrackPosKeys.Empty(TotalFrames);
				AnimTrackRotKeys.Empty(TotalFrames);
				AnimTrackScaleKeys.Empty(TotalFrames);

				for (int32 key = StartFrame; key <= EndFrame; key++)
				{
					FTransform AnimatedLocalKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(BoneName, key, AnimOrig->Interpolation);

					AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
					AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
					AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());
				}

				AnimSequence->GetController().SetBoneTrackKeys(BoneName, AnimTrackPosKeys, AnimTrackRotKeys, AnimTrackScaleKeys);
			}
		}
		AnimSequence->GetController().CloseBracket();
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::ApplyPlayRateCurve(UAnimSequenceBase* AnimSequenceBase, const FName PlayRateCurveName)
{
	const IAnimationDataModel* Model = AnimSequenceBase->GetDataModel();
	const FFloatCurve* PlayRateCurve = Model->FindFloatCurve(FAnimationCurveIdentifier(PlayRateCurveName, ERawCurveTrackTypes::RCT_Float));

	if (!PlayRateCurve)	{ return; }

	if (UAnimMontage* AnimMontage = Cast<UAnimMontage>(AnimSequenceBase))
	{
		if (!AnimMontage->SlotAnimTracks.IsEmpty())
		{
			FAnimTrack& AnimTrack = AnimMontage->SlotAnimTracks[0].AnimTrack;
			if (!AnimTrack.AnimSegments.IsEmpty())
			{
				if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(AnimTrack.AnimSegments[0].GetAnimReference()))
				{
					for (FAnimNotifyEvent& NotifyEvent : AnimMontage->Notifies)
					{
						const float StartTime = NotifyEvent.GetTime();
						const float Duration = NotifyEvent.GetDuration();
						const float NewStartTime = UNipcatBasicGameplayFunctionLibrary::GetScaledTimeStep(*PlayRateCurve, 0.f, StartTime);
						const float NewDuration = UNipcatBasicGameplayFunctionLibrary::GetScaledTimeStep(*PlayRateCurve, StartTime, Duration);
						/*
						UE_LOG(LogTemp, Warning, TEXT("Notify Name %s: Start Time %f -> %f, Duration %f -> %f."), *NotifyEvent.GetNotifyEventName().ToString(),
							StartTime, NewStartTime, Duration, NewDuration);*/
						NotifyEvent.SetTime(NewStartTime);
						NotifyEvent.SetDuration(NewDuration);
						NotifyEvent.Update();
					}
					TimeManipulation(AnimSequence->GetDataModel(), AnimSequence->GetController(), PlayRateCurve->FloatCurve);
					AnimTrack.AnimSegments[0].SetAnimReference(AnimSequence, true);
					AnimMontage->GetController().RemoveCurve(FAnimationCurveIdentifier(PlayRateCurveName, ERawCurveTrackTypes::RCT_Float));
				}
			}
		}
	}
	else if (UAnimSequence* AnimSequence = Cast<UAnimSequence>(AnimSequenceBase))
	{
		TimeManipulation(AnimSequence->GetDataModel(), AnimSequence->GetController(), PlayRateCurve->FloatCurve);
		
		for (FAnimNotifyEvent& NotifyEvent : AnimSequence->Notifies)
		{
			const float StartTime = NotifyEvent.GetTime();
			const float Duration = NotifyEvent.GetDuration();
			NotifyEvent.SetTime(UNipcatBasicGameplayFunctionLibrary::GetScaledTimeStep(*PlayRateCurve, 0.f, StartTime));
			NotifyEvent.SetDuration(UNipcatBasicGameplayFunctionLibrary::GetScaledTimeStep(*PlayRateCurve, StartTime, Duration));
		}
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::TimeManipulation(const IAnimationDataModel* Model,
	IAnimationDataController& Controller, const FRichCurve& TimeManipulationCurve)
{
	TArray<FName> TrackNames;
	Model->GetBoneTrackNames(TrackNames);

	TArray<FTransform> BoneTransforms;
	const int32 Num = Model->GetNumberOfKeys();
	BoneTransforms.Reserve(Num);

	const FFrameRate& FrameRate = Model->GetFrameRate();
	const double SecondsInFrame = FrameRate.AsSeconds(1);

	constexpr int32 Subdivision = 100;
	const double Step = SecondsInFrame / Subdivision;

	double NewPlayLength = 0;
	TMap<int32, double> CurveValues;

	for (int32 i = 0; i < Num; i++)
	{
		for (int32 j = 0; j < Subdivision; j++)
		{
			float CurveValue = TimeManipulationCurve.Eval(SecondsInFrame * i + SecondsInFrame * j / Subdivision);
			if (CurveValue <= 0)
			{
				CurveValue = 1.f;
			}
			const double Value = 1 / CurveValue;
			NewPlayLength += Value * Step;
			CurveValues.Add(Subdivision * i + j, NewPlayLength);
		}
	}

	const double secondsInFrameInv = 1 / SecondsInFrame;
	const int32 NewNum = FMath::FloorToInt(NewPlayLength * secondsInFrameInv) + 1;

	TArray<FVector> PosKeys;
	PosKeys.SetNum(NewNum);
	TArray<FQuat> RotKeys;
	RotKeys.SetNum(NewNum);
	TArray<FVector> ScaleKeys;
	ScaleKeys.SetNum(NewNum);

	TArray<bool> SetKeys;
	SetKeys.SetNum(NewNum);
	TArray<double> SetKeysDistance;
	SetKeysDistance.SetNum(NewNum);

	for (const FName& TrackName : TrackNames)
	{
		BoneTransforms.Reset();
		Model->GetBoneTrackTransforms(TrackName, BoneTransforms);

		for (size_t i = 0; i < NewNum; i++)
		{
			SetKeys[i] = false;
			SetKeysDistance[i] = 100000;
		}

		for (size_t i = 0; i < Num; i++)
		{
			const int32 minFrame = FMath::FloorToInt(CurveValues[Subdivision * i] * secondsInFrameInv);
			const int32 maxFrame = FMath::FloorToInt(CurveValues[Subdivision * i + Subdivision - 1] * secondsInFrameInv);
			const int32 j = i == Num - 1 ? maxFrame : minFrame;

			if (!SetKeys[j])
			{
				PosKeys[j] = BoneTransforms[i].GetLocation();
				RotKeys[j] = BoneTransforms[i].GetRotation();
				ScaleKeys[j] = BoneTransforms[i].GetScale3D();

				SetKeys[j] = true;
			}
		}

		int32 firstSetKey = INDEX_NONE;
		int32 prevSetKey = INDEX_NONE;

		for (size_t i = 0; i < NewNum; i++)
		{
			if (SetKeys[i])
			{
				if (firstSetKey == INDEX_NONE)
				{
					firstSetKey = i;
				}

				if (prevSetKey != INDEX_NONE && i > prevSetKey + 1)
				{
					for (size_t k = prevSetKey + 1; k < i; k++)
					{
						const float alpha = (k - prevSetKey) * 1.0 / (i - prevSetKey);

						PosKeys[k] = FMath::Lerp(PosKeys[prevSetKey], PosKeys[i], alpha);
						RotKeys[k] = FQuat::Slerp(RotKeys[prevSetKey], RotKeys[i], alpha);
						ScaleKeys[k] = FMath::Lerp(ScaleKeys[prevSetKey], ScaleKeys[i], alpha);
					}
				}

				prevSetKey = i;
			}
		}

		if (!SetKeys[0])
		{
			for (size_t i = 0; i < firstSetKey; i++)
			{
				PosKeys[i] = PosKeys[firstSetKey];
				RotKeys[i] = RotKeys[firstSetKey];
				ScaleKeys[i] = ScaleKeys[firstSetKey];
			}
		}

		if (!SetKeys[NewNum - 1])
		{
			for (size_t i = prevSetKey + 1; i < NewNum - 1; i++)
			{
				PosKeys[i] = PosKeys[prevSetKey];
				RotKeys[i] = RotKeys[prevSetKey];
				ScaleKeys[i] = ScaleKeys[prevSetKey];
			}
		}

		Controller.SetBoneTrackKeys(TrackName, PosKeys, RotKeys, ScaleKeys);
	}

	Controller.ResizeInFrames(NewNum, 0, NewNum - 1);

	/*
	const FAnimationCurveIdentifier PlayRateCurveId(TEXT("PlayRate"), ERawCurveTrackTypes::RCT_Float);
	Controller.AddCurve(PlayRateCurveId);
	Controller.SetCurveKeys(PlayRateCurveId, TimeManipulationCurve.Keys);*/
}

void UNipcatLocomotionSystemEditorFunctionLibrary::AdjustAnimationSpeed(UAnimSequence* AnimSequence, float Speed, int32 StartFrame, int32 EndFrame, int32 BlendInFrames, int32 BlendOutFrames)
{
	if (IsValid(AnimSequence) == false)
	{
		return;
	}

	USkeleton* Skeleton = AnimSequence->GetSkeleton();
	const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
	int32 NumBones = RefSkeleton.GetNum();

	int32 NumKeys = AnimSequence->GetDataModel()->GetNumberOfKeys();
	if (NumKeys == 0)
	{
		return;
	}

	if (Speed <= 0.0f)
	{
		return;
	}

	double IntervalTime = AnimSequence->GetDataModel()->GetFrameRate().AsInterval();

	if(EndFrame <= 0)
	{
		EndFrame = NumKeys - 1;
	}
	
	StartFrame = FMath::Clamp(StartFrame, 0, NumKeys - 1);
	EndFrame = FMath::Clamp(EndFrame, 0, NumKeys - 1);
	EndFrame = FMath::Max(StartFrame, EndFrame);

	float TotalAdjustTime = (EndFrame - StartFrame) * IntervalTime;

	if (TotalAdjustTime <= 0.0f)
	{
		return;
	}

	int AdjustFrames = -1;

	while (AdjustFrames < 0)
	{
		float BlendInTime = 0.0f;
		for (int32 key = 0; key < BlendInFrames; key++)
		{
			float Alpha = (float)key / (float)BlendInFrames;
			float BlendSpeed = FMath::Lerp(1.0f, Speed, Alpha);
			BlendInTime += IntervalTime * BlendSpeed;
		}

		float BlendOutTime = 0.0f;
		for (int32 key = 0; key < BlendOutFrames; key++)
		{
			float Alpha = (float)key / (float)BlendOutFrames;
			float BlendSpeed = FMath::Lerp(Speed, 1.0f, Alpha);
			BlendOutTime += IntervalTime * BlendSpeed;
		}

		if (TotalAdjustTime < (BlendInTime + BlendOutTime))
		{
			BlendInFrames = FMath::Max(BlendInFrames - 1, 0);
			BlendOutFrames = FMath::Max(BlendOutFrames - 1, 0);
		}
		else
		{
			float AdjustTime = TotalAdjustTime - (BlendInTime + BlendOutTime);
			float AdjustIntervalTime = IntervalTime * Speed;
			AdjustFrames = FMath::FloorToInt(AdjustTime / AdjustIntervalTime);
		}
	}

	int TotalFrames = StartFrame + BlendInFrames + AdjustFrames + BlendOutFrames + (NumKeys - EndFrame);

	UAnimSequence* AnimOrig = NewObject<UAnimSequence>();
	AnimOrig->SetSkeleton(AnimSequence->GetSkeleton());
	AnimOrig->CreateAnimation(AnimSequence);

	{

		

		FScopedTransaction Transation(LOCTEXT("AdjustAnimationSpeed", "AdjustAnimationSpeed"));
		AnimSequence->Modify();
		AnimSequence->GetController().OpenBracket(FText::FromString(TEXT("AdjustAnimationSpeed")));


		/*
		* Ű ������ ������ ������ �׷��� NumberOfFrames = NumberOfKeys - 1
		* TotalFrames �� Ű�� ���� �̱� ������ TotalFrames - 1�� NumberOfFrames �� ����
		*/
		//AnimSequence->GetController().SetPlayLength((TotalFrames > 1) ? (TotalFrames - 1) * IntervalTime : MINIMUM_ANIMATION_LENGTH);
		AnimSequence->GetController().SetNumberOfFrames(FFrameNumber(TotalFrames - 1));

		for (int32 BoneIndex = 0; BoneIndex < NumBones; BoneIndex++)
		{
			FName BoneName = RefSkeleton.GetBoneName(BoneIndex);

			if (AnimOrig->GetDataModel()->IsValidBoneTrackName(BoneName))
			{
				TArray< FVector > AnimTrackPosKeys;
				TArray< FQuat > AnimTrackRotKeys;
				TArray< FVector > AnimTrackScaleKeys;
				AnimTrackPosKeys.Empty(TotalFrames);
				AnimTrackRotKeys.Empty(TotalFrames);
				AnimTrackScaleKeys.Empty(TotalFrames);

				for (int32 Key = 0; Key < StartFrame; Key++)
				{
					FTransform AnimatedLocalKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(BoneName, Key, AnimOrig->Interpolation);


					AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
					AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
					AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());
				}

				double AnimationTime = IntervalTime * StartFrame;

				for (int32 Key = 0; Key < BlendInFrames; Key++)
				{
					int32 KeyIndex1, KeyIndex2;
					float SubFrame;
					FAnimationRuntime::GetKeyIndicesFromTime(KeyIndex1, KeyIndex2, SubFrame, AnimationTime, AnimOrig->GetDataModel()->GetFrameRate(), AnimOrig->GetDataModel()->GetNumberOfKeys());
					
					FTransform AnimatedLocalKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(BoneName, FFrameTime(KeyIndex1, SubFrame), AnimOrig->Interpolation);

					AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
					AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
					AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

					float Alpha = (float)Key / (float)BlendInFrames;
					float BlendSpeed = FMath::Lerp(1.0f, Speed, Alpha);
					AnimationTime += IntervalTime * BlendSpeed;
				}

				double AdjustIntervalTime = IntervalTime * Speed;

				for (int32 Key = 0; Key < AdjustFrames; Key++)
				{
					int32 KeyIndex1, KeyIndex2;
					float SubFrame;
					FAnimationRuntime::GetKeyIndicesFromTime(KeyIndex1, KeyIndex2, SubFrame, AnimationTime, AnimOrig->GetDataModel()->GetFrameRate(), AnimOrig->GetDataModel()->GetNumberOfKeys());

					FTransform AnimatedLocalKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(BoneName, FFrameTime(KeyIndex1, SubFrame), AnimOrig->Interpolation);
					
					AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
					AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
					AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

					AnimationTime += AdjustIntervalTime;
				}

				for (int32 Key = 0; Key < BlendOutFrames; Key++)
				{
					int32 KeyIndex1, KeyIndex2;
					float SubFrame;
					FAnimationRuntime::GetKeyIndicesFromTime(KeyIndex1, KeyIndex2, SubFrame, AnimationTime, AnimOrig->GetDataModel()->GetFrameRate(), AnimOrig->GetDataModel()->GetNumberOfKeys());

					FTransform AnimatedLocalKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(BoneName, FFrameTime(KeyIndex1, SubFrame), AnimOrig->Interpolation);
					
					AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
					AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
					AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

					float Alpha = (float)Key / (float)BlendOutFrames;
					float BlendSpeed = FMath::Lerp(Speed, 1.0f, Alpha);
					AnimationTime += IntervalTime * BlendSpeed;
				}

				for (int32 Key = StartFrame + BlendInFrames + AdjustFrames + BlendOutFrames; Key < TotalFrames; Key++)
				{
					int32 KeyIndex1, KeyIndex2;
					float SubFrame;
					FAnimationRuntime::GetKeyIndicesFromTime(KeyIndex1, KeyIndex2, SubFrame, AnimationTime, AnimOrig->GetDataModel()->GetFrameRate(), AnimOrig->GetDataModel()->GetNumberOfKeys());

					FTransform AnimatedLocalKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(BoneName, FFrameTime(KeyIndex1, SubFrame), AnimOrig->Interpolation);
					
					AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
					AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
					AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

					AnimationTime += IntervalTime;
				}

				AnimSequence->GetController().SetBoneTrackKeys(BoneName, AnimTrackPosKeys, AnimTrackRotKeys, AnimTrackScaleKeys);
			}
		}

		AnimSequence->GetController().CloseBracket();

	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::AppendAnimation(UAnimSequence* AnimSequence,
	UAnimSequence* NextAnimSequence, int32 NextStartTime, int32 NextBlendFrames)
{
	if (IsValid(AnimSequence) == false)
	{
		return;
	}

	if (IsValid(NextAnimSequence) == false)
	{
		return;
	}

	USkeleton* Skeleton = AnimSequence->GetSkeleton();
	const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
	int32 NumBones = RefSkeleton.GetNum();

	int32 NumKeys = AnimSequence->GetDataModel()->GetNumberOfKeys();
	if (NumKeys == 0)
	{
		return;
	}

	int32 RootBoneIndex = 0;
	FName RootBoneName = RefSkeleton.GetBoneName(RootBoneIndex);
	if (AnimSequence->GetDataModel()->IsValidBoneTrackName(RootBoneName) == false)
	{
		return;
	}

	if (NextAnimSequence->GetSkeleton() != Skeleton)
	{
		return;
	}

	if (NextAnimSequence->GetDataModel()->GetNumberOfKeys() == 0)
	{
		return;
	}

	double IntervalTime = AnimSequence->GetDataModel()->GetFrameRate().AsInterval();

	int32 TotalFrames = NumKeys;
	NextStartTime = FMath::Min(NextStartTime, NextAnimSequence->GetDataModel()->GetPlayLength());
	int32 NextFrames = FMath::FloorToInt((NextAnimSequence->GetDataModel()->GetPlayLength() - NextStartTime) / IntervalTime) + 1;
	TotalFrames += NextFrames;

	

	UAnimSequence* AnimOrig = NewObject<UAnimSequence>();
	AnimOrig->SetSkeleton(AnimSequence->GetSkeleton());
	AnimOrig->CreateAnimation(AnimSequence);

	UAnimSequence* NextOrig = NewObject<UAnimSequence>();
	NextOrig->SetSkeleton(NextAnimSequence->GetSkeleton());
	NextOrig->CreateAnimation(NextAnimSequence);

	{
		

		FScopedTransaction Transaction(LOCTEXT("AppendAnimation", "AppendAnimation"));
		AnimSequence->Modify();
		AnimSequence->GetController().OpenBracket(FText::FromString(TEXT("AppendAnimation")));

		AnimSequence->GetController().SetNumberOfFrames(FFrameNumber(TotalFrames - 1));

		{
			TArray< FVector > AnimTrackPosKeys;
			TArray< FQuat > AnimTrackRotKeys;
			TArray< FVector > AnimTrackScaleKeys;
			AnimTrackPosKeys.Empty(TotalFrames);
			AnimTrackRotKeys.Empty(TotalFrames);
			AnimTrackScaleKeys.Empty(TotalFrames);

			FTransform LastKey = FTransform::Identity;

			for (int32 key = 0; key < NumKeys; key++)
			{
				FTransform AnimatedLocalKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, key, AnimOrig->Interpolation);


				AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
				AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
				AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

				LastKey = AnimatedLocalKey;
			}


			{
				if (NextOrig->GetDataModel()->IsValidBoneTrackName(RootBoneName))
				{
					int32 KeyIndex1, KeyIndex2;
					float SubFrame;
					FAnimationRuntime::GetKeyIndicesFromTime(KeyIndex1, KeyIndex2, SubFrame, NextStartTime, NextOrig->GetDataModel()->GetFrameRate(), NextOrig->GetDataModel()->GetNumberOfKeys());

					FTransform NextStartKey = NextOrig->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, FFrameTime(KeyIndex1, SubFrame), NextOrig->Interpolation);
					

					for (int32 key = 0; key < NextFrames; key++)
					{
						FAnimationRuntime::GetKeyIndicesFromTime(KeyIndex1, KeyIndex2, SubFrame, (key* IntervalTime) + NextStartTime, NextOrig->GetDataModel()->GetFrameRate(), NextOrig->GetDataModel()->GetNumberOfKeys());

						FTransform AnimatedLocalKey = NextOrig->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, FFrameTime(KeyIndex1, SubFrame), NextOrig->Interpolation);
						
						AnimatedLocalKey = AnimatedLocalKey * NextStartKey.Inverse();
						AnimatedLocalKey.SetLocation(LastKey.GetRotation().RotateVector(AnimatedLocalKey.GetLocation()));


						FTransform NextLocalKey = LastKey;
						NextLocalKey.Accumulate(AnimatedLocalKey);
						NextLocalKey.NormalizeRotation();

						AnimTrackPosKeys.Add(NextLocalKey.GetLocation());
						AnimTrackRotKeys.Add(NextLocalKey.GetRotation());
						AnimTrackScaleKeys.Add(NextLocalKey.GetScale3D());
					}
				}
				else
				{
					for (int32 key = 0; key < NextFrames; key++)
					{
						AnimTrackPosKeys.Add(LastKey.GetLocation());
						AnimTrackRotKeys.Add(LastKey.GetRotation());
						AnimTrackScaleKeys.Add(LastKey.GetScale3D());
					}
				}
			}

			AnimSequence->GetController().SetBoneTrackKeys(RootBoneName, AnimTrackPosKeys, AnimTrackRotKeys, AnimTrackScaleKeys);
		}

		for (int32 BoneIndex = 1; BoneIndex < NumBones; BoneIndex++)
		{
			FName BoneName = RefSkeleton.GetBoneName(BoneIndex);
			
			if (AnimOrig->GetDataModel()->IsValidBoneTrackName(BoneName) == false)
			{
				bool bAddNewTrack = false;

				if (NextOrig->GetDataModel()->IsValidBoneTrackName(BoneName))
				{
					bAddNewTrack = true;
				}

				if (bAddNewTrack)
				{
					FTransform LocalTransform = RefSkeleton.GetRefBonePose()[BoneIndex];
					FRawAnimSequenceTrack AnimTrack;
					AnimTrack.PosKeys.Add(FVector3f(LocalTransform.GetLocation()));
					AnimTrack.RotKeys.Add(FQuat4f(LocalTransform.GetRotation()));
					AnimTrack.ScaleKeys.Add(FVector3f(LocalTransform.GetScale3D()));

					AnimSequence->GetController().AddBoneCurve(BoneName);
					AnimSequence->GetController().SetBoneTrackKeys(BoneName, AnimTrack.PosKeys, AnimTrack.RotKeys, AnimTrack.ScaleKeys);
				}
			}

			if (AnimSequence->GetDataModel()->IsValidBoneTrackName(BoneName))
			{
				TArray< FVector > AnimTrackPosKeys;
				TArray< FQuat > AnimTrackRotKeys;
				TArray< FVector > AnimTrackScaleKeys;
				AnimTrackPosKeys.Empty(TotalFrames);
				AnimTrackRotKeys.Empty(TotalFrames);
				AnimTrackScaleKeys.Empty(TotalFrames);

				FTransform LastKey = FTransform::Identity;

				for (int32 key = 0; key < NumKeys; key++)
				{
					FTransform AnimatedLocalKey = AnimOrig->GetDataModel()->EvaluateBoneTrackTransform(BoneName, key, AnimOrig->Interpolation);


					AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
					AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
					AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

					if (key == (NumKeys - 1))
					{
						LastKey = AnimatedLocalKey;
					}
				}


				{
					if (NextOrig->GetDataModel()->IsValidBoneTrackName(BoneName))
					{
						for (int32 key = 0; key < NextFrames; key++)
						{
							int32 KeyIndex1, KeyIndex2;
							float SubFrame;
							FAnimationRuntime::GetKeyIndicesFromTime(KeyIndex1, KeyIndex2, SubFrame, (key* IntervalTime) + NextStartTime, NextOrig->GetDataModel()->GetFrameRate(), NextOrig->GetDataModel()->GetNumberOfKeys());

							FTransform AnimatedLocalKey = NextOrig->GetDataModel()->EvaluateBoneTrackTransform(BoneName, FFrameTime(KeyIndex1, SubFrame), NextOrig->Interpolation);
							
							float Alpha = FMath::Clamp((float)(NextBlendFrames - key) / (float)(NextBlendFrames + 1), 0.0f, 1.0f);
							if (Alpha > 0.0f)
							{
								AnimatedLocalKey.BlendWith(LastKey, Alpha);
								AnimatedLocalKey.NormalizeRotation();
							}

							AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
							AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
							AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());
						}
					}
					else
					{
						for (int32 key = 0; key < NextFrames; key++)
						{
							AnimTrackPosKeys.Add(LastKey.GetLocation());
							AnimTrackRotKeys.Add(LastKey.GetRotation());
							AnimTrackScaleKeys.Add(LastKey.GetScale3D());
						}
					}
				}


				AnimSequence->GetController().SetBoneTrackKeys(BoneName, AnimTrackPosKeys, AnimTrackRotKeys, AnimTrackScaleKeys);
			}
		}
		AnimSequence->GetController().CloseBracket();
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::NormalizeAnimationRootMotion(UAnimSequence* AnimSequence, bool AutoDetectNormalizeDirection, FVector Scale, int32 StartFrame , int32 EndFrame, bool KeepRootMotionAfterEndFrame)
{
	int32 AddRootMotionStartFrame = StartFrame;
	int32 AddRootMotionEndFrame = EndFrame >= 0 ? EndFrame : AnimSequence->GetDataModel()->GetNumberOfFrames() + 1 + EndFrame;
	
	
	if (AnimSequence)
	{
		USkeleton* Skeleton = AnimSequence->GetSkeleton();
		const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
		int32 NumBones = RefSkeleton.GetNum();

		int32 NumKeys = AnimSequence->GetDataModel()->GetNumberOfKeys();
		if (NumKeys == 0)
		{
			return;
		}

		int32 RootBoneIndex = 0;
		FName RootBoneName = RefSkeleton.GetBoneName(RootBoneIndex);

		if (AnimSequence->GetDataModel()->IsValidBoneTrackName(RootBoneName) == false)
		{
			return;
		}

		// Adjust
		{
			FScopedTransaction Transaction(LOCTEXT("NormalizeRootMotion", "NormalizeRootMotion"));
			AnimSequence->Modify();

			AnimSequence->GetController().OpenBracket(FText::FromString(TEXT("NormalizeRootMotion")));
			

			AddRootMotionStartFrame = FMath::Max(AddRootMotionStartFrame, 0);
			AddRootMotionEndFrame = FMath::Max(AddRootMotionStartFrame + 1, AddRootMotionEndFrame);
			
			// Normalize Root
			const FTransform StartFrameTransform = AnimSequence->GetDataModel()->GetBoneTrackTransform(RootBoneName, FFrameNumber(AddRootMotionStartFrame));
			const FTransform EndFrameTransform = AnimSequence->GetDataModel()->GetBoneTrackTransform(RootBoneName, FFrameNumber(AddRootMotionEndFrame));
			FVector AddRootMotionValue = EndFrameTransform.GetRelativeTransform(StartFrameTransform).GetTranslation();

			if (AutoDetectNormalizeDirection)
			{
				const float X = FMath::Abs(AddRootMotionValue.X);
				const float Y = FMath::Abs(AddRootMotionValue.Y);
				const float Z = FMath::Abs(AddRootMotionValue.Z);
				if (X > Y && X > Z)
				{
					AddRootMotionValue.Y = 0;
					AddRootMotionValue.Z = 0;
				}
				else if (Y > X && Y > Z)
				{
					AddRootMotionValue.X = 0;
					AddRootMotionValue.Z = 0;
				}
				else if (Z > X && Z > Y)
				{
					AddRootMotionValue.X = 0;
					AddRootMotionValue.Y = 0;
				}
			}
			
			AddRootMotionValue *= Scale;

			TArray<FVector> RootAnimTrackPosKeys;
			TArray<FQuat> RootAnimTrackRotKeys;
			TArray<FVector> RootAnimTrackScaleKeys;
			RootAnimTrackPosKeys.Empty(NumKeys);
			RootAnimTrackRotKeys.Empty(NumKeys);
			RootAnimTrackScaleKeys.Empty(NumKeys);

			FVector PrevLocation = StartFrameTransform.GetLocation();

			FVector AddMoveDelta = AddRootMotionValue / (float)(AddRootMotionEndFrame - AddRootMotionStartFrame);

			TArray<FTransform> CachedRootTransforms;

			for (int32 key = 0; key < NumKeys; key++)
			{
				
				FTransform AnimatedLocalKey = AnimSequence->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, FFrameTime(key), AnimSequence->Interpolation);

				CachedRootTransforms.Emplace(AnimatedLocalKey);

				FVector AfterEndFrameOffset;
				
				if (key >= AddRootMotionStartFrame && key <= AddRootMotionEndFrame)
				{
					AnimatedLocalKey.SetLocation(PrevLocation);
					if (key < AddRootMotionEndFrame)
					{
						PrevLocation += AddMoveDelta;
					}
				}
				else if (key > AddRootMotionEndFrame)
				{
					if (!KeepRootMotionAfterEndFrame)
					{
						if (key == AddRootMotionEndFrame + 1)
						{
							AfterEndFrameOffset = PrevLocation - AnimatedLocalKey.GetLocation();
						}
						AnimatedLocalKey.SetLocation(AnimatedLocalKey.GetLocation() + AfterEndFrameOffset);
					}
				}
				
				RootAnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
				RootAnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
				RootAnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());
			}


			AnimSequence->GetController().SetBoneTrackKeys(RootBoneName, RootAnimTrackPosKeys, RootAnimTrackRotKeys, RootAnimTrackScaleKeys);


			// Reset Other Bones
			for (int32 BoneIndex = 1; BoneIndex < NumBones; BoneIndex++)
			{
				// Ignore ik root bones
				FName BoneName = RefSkeleton.GetBoneName(BoneIndex);
				
				if (!IsDirectChildOfRootBone(Skeleton, BoneName))
				{
					continue;
				}
				
				if (AnimSequence->GetDataModel()->IsValidBoneTrackName(BoneName))
				{
					TArray<FVector> AnimTrackPosKeys;
					TArray<FQuat> AnimTrackRotKeys;
					TArray<FVector> AnimTrackScaleKeys;
					AnimTrackPosKeys.Empty(NumKeys);
					AnimTrackRotKeys.Empty(NumKeys);
					AnimTrackScaleKeys.Empty(NumKeys);
					
					for (int32 key = 0; key < NumKeys; key++)
					{
						FTransform RootAnimatedLocalKey = AnimSequence->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, FFrameTime(key), AnimSequence->Interpolation);
						FTransform CachedRootAnimatedLocalKey = CachedRootTransforms[key];

						FVector RootDelta = CachedRootAnimatedLocalKey.GetLocation() - RootAnimatedLocalKey.GetLocation();
						
						FTransform AnimatedLocalKey = AnimSequence->GetDataModel()->EvaluateBoneTrackTransform(BoneName, FFrameTime(key), AnimSequence->Interpolation);
						
						AnimatedLocalKey.SetLocation(AnimatedLocalKey.GetLocation() + RootDelta);

						AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
						AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
						AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

					}

					AnimSequence->GetController().SetBoneTrackKeys(BoneName, AnimTrackPosKeys, AnimTrackRotKeys, AnimTrackScaleKeys);
				}
			}

			
			AnimSequence->GetController().CloseBracket();
		}
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::NormalizeAnimationRotation(UAnimSequence* AnimSequence)
{
	int32 AddRootMotionStartFrame = 0;
	int32 AddRootMotionEndFrame = AnimSequence->GetDataModel()->GetNumberOfFrames();
	
	
	if (AnimSequence)
	{
		USkeleton* Skeleton = AnimSequence->GetSkeleton();
		const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
		int32 NumBones = RefSkeleton.GetNum();

		int32 NumKeys = AnimSequence->GetDataModel()->GetNumberOfKeys();
		if (NumKeys == 0)
		{
			return;
		}

		int32 RootBoneIndex = 0;
		FName RootBoneName = RefSkeleton.GetBoneName(RootBoneIndex);

		if (AnimSequence->GetDataModel()->IsValidBoneTrackName(RootBoneName) == false)
		{
			return;
		}

		// Adjust
		{
			FScopedTransaction Transaction(LOCTEXT("NormalizeRotation", "NormalizeRotation"));
			AnimSequence->Modify();

			AnimSequence->GetController().OpenBracket(FText::FromString(TEXT("NormalizeRotation")));
			

			AddRootMotionStartFrame = FMath::Max(AddRootMotionStartFrame, 0);
			AddRootMotionEndFrame = FMath::Max(AddRootMotionStartFrame + 1, AddRootMotionEndFrame);
			
			// Normalize Root
			const FTransform StartFrameTransform = AnimSequence->GetDataModel()->GetBoneTrackTransform(RootBoneName, FFrameNumber(AddRootMotionStartFrame));
			const FTransform EndFrameTransform = AnimSequence->GetDataModel()->GetBoneTrackTransform(RootBoneName, FFrameNumber(AddRootMotionEndFrame));
			FQuat AddRootMotionValue = StartFrameTransform.GetRelativeTransform(EndFrameTransform).GetRotation();

			TArray<FVector> RootAnimTrackPosKeys;
			TArray<FQuat> RootAnimTrackRotKeys;
			TArray<FVector> RootAnimTrackScaleKeys;
			RootAnimTrackPosKeys.Empty(NumKeys);
			RootAnimTrackRotKeys.Empty(NumKeys);
			RootAnimTrackScaleKeys.Empty(NumKeys);

			FRotator PrevRotation = StartFrameTransform.Rotator();

			FQuat AddMoveDelta = FQuat::Slerp(FQuat::Identity, AddRootMotionValue, 1.f / (float)(AddRootMotionEndFrame - AddRootMotionStartFrame)).Inverse();

			TArray<FTransform> CachedRootTransforms;

			for (int32 key = 0; key < NumKeys; key++)
			{
				
				FTransform AnimatedLocalKey = AnimSequence->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, FFrameTime(key), AnimSequence->Interpolation);

				CachedRootTransforms.Emplace(AnimatedLocalKey);
				
				AnimatedLocalKey.SetRotation(PrevRotation.Quaternion());

				RootAnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
				RootAnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
				RootAnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

				PrevRotation = UKismetMathLibrary::ComposeRotators(AnimatedLocalKey.GetRotation().Rotator(), AddMoveDelta.Rotator());
			}


			AnimSequence->GetController().SetBoneTrackKeys(RootBoneName, RootAnimTrackPosKeys, RootAnimTrackRotKeys, RootAnimTrackScaleKeys);


			// Reset Other Bones
			for (int32 BoneIndex = 1; BoneIndex < NumBones; BoneIndex++)
			{
				// Ignore ik root bones
				FName BoneName = RefSkeleton.GetBoneName(BoneIndex);
				
				if (!IsDirectChildOfRootBone(Skeleton, BoneName))
				{
					continue;
				}
				
				if (AnimSequence->GetDataModel()->IsValidBoneTrackName(BoneName))
				{
					TArray<FVector> AnimTrackPosKeys;
					TArray<FQuat> AnimTrackRotKeys;
					TArray<FVector> AnimTrackScaleKeys;
					AnimTrackPosKeys.Empty(NumKeys);
					AnimTrackRotKeys.Empty(NumKeys);
					AnimTrackScaleKeys.Empty(NumKeys);
					
					for (int32 key = 0; key < NumKeys; key++)
					{
						FTransform RootAnimatedLocalKey = AnimSequence->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, FFrameTime(key), AnimSequence->Interpolation);
						FTransform CachedRootAnimatedLocalKey = CachedRootTransforms[key];

						FRotator RootDelta = CachedRootAnimatedLocalKey.GetRotation().Rotator() - RootAnimatedLocalKey.GetRotation().Rotator();
						RootDelta.Normalize();
						
						FTransform AnimatedLocalKey = AnimSequence->GetDataModel()->EvaluateBoneTrackTransform(BoneName, FFrameTime(key), AnimSequence->Interpolation);
						
						AnimatedLocalKey.SetRotation(UKismetMathLibrary::ComposeRotators(AnimatedLocalKey.GetRotation().Rotator(), RootDelta).Quaternion());

						AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
						AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
						AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

					}

					AnimSequence->GetController().SetBoneTrackKeys(BoneName, AnimTrackPosKeys, AnimTrackRotKeys, AnimTrackScaleKeys);
				}
			}

			
			AnimSequence->GetController().CloseBracket();
		}
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::AddRootRotation(UAnimSequence* AnimSequence, FRotator DeltaRotation)
{
	int32 AddRootMotionStartFrame = 0;
	int32 AddRootMotionEndFrame = AnimSequence->GetDataModel()->GetNumberOfFrames();
	
	
	if (AnimSequence)
	{
		USkeleton* Skeleton = AnimSequence->GetSkeleton();
		const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
		int32 NumBones = RefSkeleton.GetNum();

		int32 NumKeys = AnimSequence->GetDataModel()->GetNumberOfKeys();
		if (NumKeys == 0)
		{
			return;
		}

		int32 RootBoneIndex = 0;
		FName RootBoneName = RefSkeleton.GetBoneName(RootBoneIndex);

		if (AnimSequence->GetDataModel()->IsValidBoneTrackName(RootBoneName) == false)
		{
			return;
		}

		// Adjust
		{
			FScopedTransaction Transaction(LOCTEXT("AddRootRotation", "AddRootRotation"));
			AnimSequence->Modify();

			AnimSequence->GetController().OpenBracket(FText::FromString(TEXT("AddRootRotation")));
			

			AddRootMotionStartFrame = FMath::Max(AddRootMotionStartFrame, 0);
			AddRootMotionEndFrame = FMath::Max(AddRootMotionStartFrame + 1, AddRootMotionEndFrame);
			
			// Normalize Root
			const FTransform StartFrameTransform = AnimSequence->GetDataModel()->GetBoneTrackTransform(RootBoneName, FFrameNumber(AddRootMotionStartFrame));
			const FTransform EndFrameTransform = AnimSequence->GetDataModel()->GetBoneTrackTransform(RootBoneName, FFrameNumber(AddRootMotionEndFrame));
			FQuat AddRootMotionValue = StartFrameTransform.GetRelativeTransform(EndFrameTransform).GetRotation();

			TArray<FVector> RootAnimTrackPosKeys;
			TArray<FQuat> RootAnimTrackRotKeys;
			TArray<FVector> RootAnimTrackScaleKeys;
			RootAnimTrackPosKeys.Empty(NumKeys);
			RootAnimTrackRotKeys.Empty(NumKeys);
			RootAnimTrackScaleKeys.Empty(NumKeys);

			TArray<FTransform> CachedRootTransforms;

			for (int32 key = 0; key < NumKeys; key++)
			{
				
				FTransform AnimatedLocalKey = AnimSequence->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, FFrameTime(key), AnimSequence->Interpolation);

				CachedRootTransforms.Emplace(AnimatedLocalKey);
				
				AnimatedLocalKey.SetRotation(UKismetMathLibrary::ComposeRotators(AnimatedLocalKey.GetRotation().Rotator(), DeltaRotation).Quaternion());

				RootAnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
				RootAnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
				RootAnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());
			}


			AnimSequence->GetController().SetBoneTrackKeys(RootBoneName, RootAnimTrackPosKeys, RootAnimTrackRotKeys, RootAnimTrackScaleKeys);


			// Reset Other Bones
			for (int32 BoneIndex = 1; BoneIndex < NumBones; BoneIndex++)
			{
				// Ignore ik root bones
				FName BoneName = RefSkeleton.GetBoneName(BoneIndex);
				
				if (!IsDirectChildOfRootBone(Skeleton, BoneName))
				{
					continue;
				}
				
				if (AnimSequence->GetDataModel()->IsValidBoneTrackName(BoneName))
				{
					TArray<FVector> AnimTrackPosKeys;
					TArray<FQuat> AnimTrackRotKeys;
					TArray<FVector> AnimTrackScaleKeys;
					AnimTrackPosKeys.Empty(NumKeys);
					AnimTrackRotKeys.Empty(NumKeys);
					AnimTrackScaleKeys.Empty(NumKeys);
					
					for (int32 key = 0; key < NumKeys; key++)
					{
						FTransform RootAnimatedLocalKey = AnimSequence->GetDataModel()->EvaluateBoneTrackTransform(RootBoneName, FFrameTime(key), AnimSequence->Interpolation);
						FTransform CachedRootAnimatedLocalKey = CachedRootTransforms[key];

						FRotator RootDelta = CachedRootAnimatedLocalKey.GetRotation().Rotator() - RootAnimatedLocalKey.GetRotation().Rotator();
						RootDelta.Normalize();
						
						FTransform AnimatedLocalKey = AnimSequence->GetDataModel()->EvaluateBoneTrackTransform(BoneName, FFrameTime(key), AnimSequence->Interpolation);
						
						AnimatedLocalKey.SetRotation(UKismetMathLibrary::ComposeRotators(AnimatedLocalKey.GetRotation().Rotator(), RootDelta).Quaternion());

						AnimTrackPosKeys.Add(AnimatedLocalKey.GetLocation());
						AnimTrackRotKeys.Add(AnimatedLocalKey.GetRotation());
						AnimTrackScaleKeys.Add(AnimatedLocalKey.GetScale3D());

					}

					AnimSequence->GetController().SetBoneTrackKeys(BoneName, AnimTrackPosKeys, AnimTrackRotKeys, AnimTrackScaleKeys);
				}
			}

			
			AnimSequence->GetController().CloseBracket();
		}
	}
}

bool UNipcatLocomotionSystemEditorFunctionLibrary::IsDirectChildOfRootBone(USkeleton* Skeleton, FName BoneName)
{
	const FReferenceSkeleton RefSkeleton = Skeleton->GetReferenceSkeleton();
	
	int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneName);

	const FVector BoneLocation = RefSkeleton.GetRefBonePose()[BoneIndex].GetLocation();
	if (BoneLocation.IsNearlyZero())
	{
		return false;
	}
	
	while (BoneIndex > 0)
	{
		const int32 ParentBoneIndex = RefSkeleton.GetParentIndex(BoneIndex);
	
		const FVector ParentBoneLocation = RefSkeleton.GetRefBonePose()[ParentBoneIndex].GetLocation();

		if (!ParentBoneLocation.IsNearlyZero())
		{
			return false;
		}
	
		BoneIndex = ParentBoneIndex;
	}

	return true;
}

void UNipcatLocomotionSystemEditorFunctionLibrary::Snap(UAnimSequence* AnimSequence, const TMap<FName, FName>& SnapToBone)
{
	if (!AnimSequence)
	{
		return;
	}
	
	IAnimationDataController& Controller = AnimSequence->GetController();

	IAnimationDataController::FScopedBracket ScopedBracket(Controller, LOCTEXT("ScopedBracket_AddOffset", "AddOffset"));

	IAnimationDataModel* Model = AnimSequence->GetDataModel();

	const int32 Num = Model->GetNumberOfKeys();

	TArray<FName> TrackNames;
	Model->GetBoneTrackNames(TrackNames);

	for (const TPair<FName, FName>& Pair : SnapToBone)
	{
		if (Pair.Key != NAME_None && Pair.Value != NAME_None)
		{
			const FName SnapBoneName = Pair.Key;
			const FName TargetBoneName = Pair.Value;

			if (SnapBoneName == TargetBoneName) continue;

			if (TrackNames.Contains(SnapBoneName) && TrackNames.Contains(TargetBoneName))
			{
				TArray<FTransform> SnapBoneLocalTransforms;
				TArray<FTransform> SnapBoneParentCompSpaceTransforms;
				FTransform SnapParentRefTransform;
				TArray<FTransform> TargetBoneLocalTransforms;
				TArray<FTransform> TargetBoneParentCompSpaceTransforms;
				FTransform TargetParentRefTransform;
				TArray<FTransform> CommonParentCompSpaceTransforms;
				
				CalculateHierarchyTransforms(AnimSequence, SnapBoneName, TargetBoneName, Num
					, SnapBoneLocalTransforms, SnapBoneParentCompSpaceTransforms, SnapParentRefTransform
					, TargetBoneLocalTransforms, TargetBoneParentCompSpaceTransforms, TargetParentRefTransform, CommonParentCompSpaceTransforms);

				TArray<FVector> PosKeys;
				PosKeys.SetNum(Num);
				TArray<FQuat> RotKeys;
				RotKeys.SetNum(Num);
				TArray<FVector> ScaleKeys;
				ScaleKeys.SetNum(Num);

				for (int32 i = 0; i < Num; i++)
				{
					const FTransform Transform = TargetBoneLocalTransforms[i] * TargetBoneParentCompSpaceTransforms[i] * SnapBoneParentCompSpaceTransforms[i].Inverse();

					PosKeys[i] = Transform.GetLocation();
					RotKeys[i] = Transform.GetRotation();
					ScaleKeys[i] = Transform.GetScale3D();
				}

				Controller.SetBoneTrackKeys(SnapBoneName, PosKeys, RotKeys, ScaleKeys);
			}
		}
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::CalculateHierarchyTransforms(const IAnimationDataModel* Model, const int32 j, const TArray<FName>& hierarchy, TArray<FTransform>& hierarchyTransforms, TArray<FTransform>& localTransforms, TArray<FTransform>& parentCompSpaceTransforms)
{
	for (size_t i = 0; i < hierarchy.Num(); i++)
	{
		hierarchyTransforms[i] = Model->GetBoneTrackTransform(hierarchy[i], j);
	}

	localTransforms[j] = hierarchyTransforms[0];
	
	parentCompSpaceTransforms[j] = FTransform::Identity;
	
	if (hierarchy.Num() > 0)
	{
		for (int32 i = hierarchy.Num() - 1; i > 0; i--)
		{
			parentCompSpaceTransforms[j] = hierarchyTransforms[i] * parentCompSpaceTransforms[j];
		}
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::CalculateHierarchyTransforms(const UAnimSequence* AnimSequence, const FName snapBoneName, const FName targetBoneName, const int32 Num
	, TArray<FTransform>& snapLocalTransforms, TArray<FTransform>& snapParentCompSpaceTransforms, FTransform& snapParentRefTransform
	, TArray<FTransform>& targetLocalTransforms, TArray<FTransform>& targetParentCompSpaceTransforms, FTransform& targetParentRefTransform, TArray<FTransform>& commonParentCompSpaceTransforms)
{
	if (!AnimSequence)
	{
		return;
	}
	const FReferenceSkeleton& RefSkeleton = AnimSequence->GetSkeleton()->GetReferenceSkeleton();
	const IAnimationDataModel* Model = AnimSequence->GetDataModel();
	
	FName commonParentBoneName = RefSkeleton.GetBoneName(0); // Root Bone Name by default

	TArray<FName> hierarchy;
	FName currentBoneName;

	const TArray<FTransform>& RefSkeletonPose = RefSkeleton.GetRefBonePose();

	// Snap

	snapParentRefTransform = FTransform::Identity;

	hierarchy.Empty();
	
	currentBoneName = snapBoneName;
	while (currentBoneName != NAME_None)
	{
		hierarchy.Add(currentBoneName); // boneName, parentBoneName, parentParentBoneName, ...
		const int32 parentBoneIndex = RefSkeleton.GetParentIndex(RefSkeleton.FindBoneIndex(currentBoneName));
		currentBoneName = RefSkeleton.IsValidIndex(parentBoneIndex) ? RefSkeleton.GetBoneName(parentBoneIndex) : NAME_None;
		snapParentRefTransform = snapParentRefTransform * (RefSkeleton.IsValidIndex(parentBoneIndex) ? RefSkeletonPose[parentBoneIndex].GetRotation() : FQuat::Identity);
	}

	if (hierarchy.Contains(targetBoneName))
	{
		commonParentBoneName = targetBoneName;
	}

	TArray<FTransform> hierarchyTransforms;
	hierarchyTransforms.SetNum(hierarchy.Num());
	
	snapLocalTransforms.SetNum(Num);	
	snapParentCompSpaceTransforms.SetNum(Num);

	for (int32 j = 0; j < Num; j++)
	{
		CalculateHierarchyTransforms(Model, j, hierarchy, hierarchyTransforms, snapLocalTransforms, snapParentCompSpaceTransforms);
	}

	// Target

	targetParentRefTransform = FTransform::Identity;

	hierarchy.Empty();

	currentBoneName = targetBoneName;
	while (currentBoneName != NAME_None)
	{
		hierarchy.Add(currentBoneName); // boneName, parentBoneName, parentParentBoneName, ...
		const int32 parentBoneIndex = RefSkeleton.GetParentIndex(RefSkeleton.FindBoneIndex(currentBoneName));
		currentBoneName = RefSkeleton.IsValidIndex(parentBoneIndex) ? RefSkeleton.GetBoneName(parentBoneIndex) : NAME_None;
		targetParentRefTransform = targetParentRefTransform * (RefSkeleton.IsValidIndex(parentBoneIndex) ? RefSkeletonPose[parentBoneIndex].GetRotation() : FQuat::Identity);
	}

	if (hierarchy.Contains(snapBoneName))
	{
		commonParentBoneName = snapBoneName;
	}
	
	hierarchyTransforms.SetNum(hierarchy.Num());

	targetLocalTransforms.SetNum(Num);
	targetParentCompSpaceTransforms.SetNum(Num);
	
	for (int32 j = 0; j < Num; j++)
	{
		CalculateHierarchyTransforms(Model, j, hierarchy, hierarchyTransforms, targetLocalTransforms, targetParentCompSpaceTransforms);
	}

	if (commonParentBoneName != RefSkeleton.GetBoneName(0))
	{
		hierarchy.Empty();

		currentBoneName = targetBoneName;
		while (currentBoneName != NAME_None)
		{
			hierarchy.Add(currentBoneName); // boneName, parentBoneName, parentParentBoneName, ...
			const int32 parentBoneIndex = RefSkeleton.GetParentIndex(RefSkeleton.FindBoneIndex(currentBoneName));
			currentBoneName = RefSkeleton.IsValidIndex(parentBoneIndex) ? RefSkeleton.GetBoneName(parentBoneIndex) : NAME_None;
		}

		hierarchyTransforms.SetNum(hierarchy.Num());

		TArray<FTransform> commonLocalTransforms;
		commonLocalTransforms.SetNum(Num);
		commonParentCompSpaceTransforms.SetNum(Num);

		for (int32 j = 0; j < Num; j++)
		{
			CalculateHierarchyTransforms(Model, j, hierarchy, hierarchyTransforms, commonLocalTransforms, commonParentCompSpaceTransforms);
		}
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::FixedGetBoneTrackTransforms(const UAnimSequence* AnimSequence,
	FName TrackName, TArray<FTransform>& OutTransforms)
{
	if (AnimSequence)
	{
		if (const auto DataModel = AnimSequence->GetDataModel())
		{
			if (DataModel->IsValidBoneTrackName(TrackName))
			{
				DataModel->GetBoneTrackTransforms(TrackName, OutTransforms);
				if (OutTransforms.Num() > 0)
				{
					return;
				}
			}
			
			if (const auto Controller = DataModel->GetController())
			{
				Controller->AddBoneCurve(TrackName);
			}
			
			const auto Skeleton = AnimSequence->GetSkeleton();
			if (DataModel && Skeleton)
			{
				const auto& RefSkeleton = Skeleton->GetReferenceSkeleton();
				OutTransforms.Init(RefSkeleton.GetRefBonePose()[RefSkeleton.FindBoneIndex(TrackName)], DataModel->GetNumberOfKeys());
			}
		}
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::ModifyBoneTransformAndKeepChildren(
    UAnimSequence* AnimSequence, 
    FName BoneName,
    const TArray<FTransform>& NewBoneTransforms, 
    bool bShouldTransact)
{
    if (!AnimSequence || !AnimSequence->GetDataModel()) return;

    const IAnimationDataModel* DataModel = AnimSequence->GetDataModel();
    IAnimationDataController& Controller = AnimSequence->GetController();
    int32 NumKeys = DataModel->GetNumberOfKeys();
    
    if (NewBoneTransforms.Num() != NumKeys) return;

    USkeleton* Skeleton = AnimSequence->GetSkeleton();
    if (!Skeleton) return;

    const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
    int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneName);
    if (BoneIndex == INDEX_NONE) return;

	// =================================================================
	// FIX: 自动修复 Translation Retargeting Mode
	// =================================================================
	if (Skeleton->GetBoneTranslationRetargetingMode(BoneIndex) == EBoneTranslationRetargetingMode::Skeleton)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Fix] Force changing Retargeting Mode for '%s' to Animation."), *BoneName.ToString());
        
		// 必须开启 Skeleton 的事务，否则这个修改无法撤销
		if (bShouldTransact) Skeleton->Modify(); 
        
		Skeleton->SetBoneTranslationRetargetingMode(BoneIndex, EBoneTranslationRetargetingMode::Animation);
		Skeleton->PostEditChange();
	}
	
    if (bShouldTransact) 
    {
        Controller.OpenBracket(FText::FromString("Modify Bone Transform And Keep Children"));
    }

	
    // 1. 缓存旧数据用于计算子骨骼补偿
    // -----------------------------------------------------------------
    TArray<FTransform> OldParentTransforms;
    FixedGetBoneTrackTransforms(AnimSequence, BoneName, OldParentTransforms);

    // 2. 准备子骨骼的新数据（先算好，暂时不写）
    // -----------------------------------------------------------------
    struct FChildBoneData
    {
        FName Name;
        TArray<FVector> Pos;
        TArray<FQuat> Rot;
        TArray<FVector> Scale;
    };
    TArray<FChildBoneData> ChildrenDataToUpdate;

    TArray<int32> ChildBoneIndices;
    for (int32 i = BoneIndex + 1; i < RefSkeleton.GetNum(); ++i)
    {
        if (RefSkeleton.GetParentIndex(i) == BoneIndex) 
            ChildBoneIndices.Add(i);
    }

    for (int32 ChildIndex : ChildBoneIndices)
    {
        FName ChildBoneName = RefSkeleton.GetBoneName(ChildIndex);
        
        // 确保子骨骼有轨道，没有就跳过或者添加（这里选择如果没轨道就暂不处理，或者你可以加上AddBoneCurve）

        TArray<FTransform> OldChildTransforms;
    	FixedGetBoneTrackTransforms(AnimSequence, ChildBoneName, OldChildTransforms);
    	
        FChildBoneData NewChildData;
        NewChildData.Name = ChildBoneName;
        NewChildData.Pos.Reserve(NumKeys);
        NewChildData.Rot.Reserve(NumKeys);
        NewChildData.Scale.Reserve(NumKeys);

        for (int32 f = 0; f < NumKeys; f++)
        {
            FTransform ChildGlobal = OldChildTransforms[f] * OldParentTransforms[f];
            FTransform NewChildLocal = ChildGlobal.GetRelativeTransform(NewBoneTransforms[f]);
            
            // NaN 安全检查！！
            if (NewChildLocal.ContainsNaN())
            {
                UE_LOG(LogTemp, Error, TEXT("NaN detected in child bone calculation: %s at frame %d. Resetting to Identity."), *ChildBoneName.ToString(), f);
                NewChildLocal = FTransform::Identity;
            }

            NewChildLocal.NormalizeRotation();
            NewChildData.Pos.Add(NewChildLocal.GetLocation());
            NewChildData.Rot.Add(NewChildLocal.GetRotation());
            NewChildData.Scale.Add(NewChildLocal.GetScale3D());
        }
        ChildrenDataToUpdate.Add(NewChildData);
    }

    // 3. 应用目标骨骼的新变换 (核弹级写入)
    // -----------------------------------------------------------------
    {
        TArray<FVector> P; TArray<FQuat> R; TArray<FVector> S;
        P.Reserve(NumKeys); R.Reserve(NumKeys); S.Reserve(NumKeys);
        
        bool bHasNaN = false;
        for (int32 i = 0; i < NewBoneTransforms.Num(); i++)
        {
            const FTransform& T = NewBoneTransforms[i];
            if (T.ContainsNaN())
            {
                bHasNaN = true;
                UE_LOG(LogTemp, Error, TEXT("NaN detected in NewBoneTransforms for %s at frame %d!"), *BoneName.ToString(), i);
                // 遇到NaN回退到旧数据，防止整个骨骼崩坏
                P.Add(OldParentTransforms[i].GetLocation());
                R.Add(OldParentTransforms[i].GetRotation());
                S.Add(OldParentTransforms[i].GetScale3D());
            }
            else
            {
                P.Add(T.GetLocation()); 
                R.Add(T.GetRotation()); 
                S.Add(T.GetScale3D());
            }
        }

        if (!bHasNaN)
        {
            // 关键步骤：为了保证写入绝对成功，先移除旧轨道，再添加新轨道
            // 这可以解决一切“轨道存在但数据被锁”的问题
            if (DataModel->IsValidBoneTrackName(BoneName))
            {
                Controller.RemoveBoneTrack(BoneName);
            }
            Controller.AddBoneCurve(BoneName);
            
            Controller.SetBoneTrackKeys(BoneName, P, R, S);
            UE_LOG(LogTemp, Log, TEXT("Force updated track for %s. Frame Count: %d"), *BoneName.ToString(), NumKeys);
        }
    }

    // 4. 写入所有子骨骼
    // -----------------------------------------------------------------
    for (const FChildBoneData& ChildData : ChildrenDataToUpdate)
    {
        // 同样，为了稳妥，对子骨骼也可以执行移除重建（可选，这里先直接Set）
        if (DataModel->IsValidBoneTrackName(ChildData.Name))
        {
             Controller.RemoveBoneTrack(ChildData.Name);
        }
        Controller.AddBoneCurve(ChildData.Name);

        Controller.SetBoneTrackKeys(ChildData.Name, ChildData.Pos, ChildData.Rot, ChildData.Scale);
    }

    if (bShouldTransact) 
    {
        Controller.CloseBracket();
    }
}

struct FFlyingSegment
{
	int32 StartFrame;
	int32 PeakFrame; // 区间内 Alpha 值最大的一帧
	int32 EndFrame;
	float PeakAlpha; // 记录该区间的最大 Alpha 值，用于归一化

	bool IsValid() const { return StartFrame < PeakFrame && PeakFrame < EndFrame && PeakAlpha > KINDA_SMALL_NUMBER; }
};

bool UNipcatLocomotionSystemEditorFunctionLibrary::CalculateCircumcenter(const FVector& A, const FVector& B, const FVector& C, FVector& OutCenter, FVector& OutNormal)
{
	FVector AC = C - A;
	FVector AB = B - A;
	FVector ABxAC = AB ^ AC; // 叉积向量，即平面法线
    
	// 模长平方太小说明三点共线，无法定圆
	if (ABxAC.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		OutNormal = FVector::UpVector; // 默认防崩
		return false; 
	}

	OutNormal = ABxAC.GetSafeNormal();

	// 外接圆圆心公式
	FVector ToCenter = (ABxAC ^ AB) * AC.SizeSquared() + (AC ^ ABxAC) * AB.SizeSquared();
	ToCenter /= (2.0f * ABxAC.SizeSquared());

	OutCenter = A + ToCenter;
	return true;
}

float UNipcatLocomotionSystemEditorFunctionLibrary::CalculateSignedAngle(const FVector& From, const FVector& To, const FVector& Axis)
{
	FVector FromDir = From.GetSafeNormal();
	FVector ToDir = To.GetSafeNormal();
    
	float CosAngle = FromDir | ToDir; // Dot Product
	float SinAngle = (FromDir ^ ToDir) | Axis; // Cross Product projected on Axis
    
	return FMath::Atan2(SinAngle, CosAngle);
}


void UNipcatLocomotionSystemEditorFunctionLibrary::ModifyBoneTrajectory(UAnimSequence* AnimSequence, FName BoneName,
	FVector TargetLocation, FName AlphaCurveName, bool Circular)
{
	if (Circular)
	{
		if (!AnimSequence || !AnimSequence->GetDataModel()) return;

	    const IAnimationDataModel* Model = AnimSequence->GetDataModel();
	    USkeleton* Skeleton = AnimSequence->GetSkeleton();
	    const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
	    int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneName);
	    if (BoneIndex == INDEX_NONE) return;
		
		if (!Model->IsValidBoneTrackName(BoneName)) 
		{
			AnimSequence->GetController().AddBoneCurve(BoneName);
		}
		
	    // 获取曲线
	    const FFloatCurve* CurveData = Model->FindFloatCurve(FAnimationCurveIdentifier(AlphaCurveName, ERawCurveTrackTypes::RCT_Float));
	    if (!CurveData) 
	    {
	        UE_LOG(LogTemp, Warning, TEXT("Curve %s not found."), *AlphaCurveName.ToString());
	        return;
	    }

	    AnimSequence->GetController().OpenBracket(FText::FromString("Modify Flying Sword Trajectory"));
	    AnimSequence->Modify();

	    int32 NumKeys = Model->GetNumberOfKeys();
	    double FrameRate = Model->GetFrameRate().AsDecimal();

	    // 1. 预计算 Component Space 变换
	    TArray<int32> ParentIndices;
	    int32 Current = RefSkeleton.GetParentIndex(BoneIndex);
	    while (Current != INDEX_NONE)
	    {
	        ParentIndices.Insert(Current, 0);
	        Current = RefSkeleton.GetParentIndex(Current);
	    }

	    TArray<TArray<FTransform>> CachedParentTracks; 
	    for (int32 PID : ParentIndices)
	    {
	        TArray<FTransform> Track;
	        FixedGetBoneTrackTransforms(AnimSequence, RefSkeleton.GetBoneName(PID), Track);
	        CachedParentTracks.Add(Track);
	    }

	    TArray<FTransform> ParentCSTransforms;
	    ParentCSTransforms.SetNum(NumKeys);
	    for (int32 f = 0; f < NumKeys; f++)
	    {
	        FTransform AccumTM = FTransform::Identity;
	        for (const TArray<FTransform>& Track : CachedParentTracks)
	            if (Track.IsValidIndex(f)) AccumTM = Track[f] * AccumTM;
	        ParentCSTransforms[f] = AccumTM;
	    }
		
	    TArray<FTransform> RootBoneTransforms;
	    FixedGetBoneTrackTransforms(AnimSequence, RefSkeleton.GetBoneName(0), RootBoneTransforms);
	    TArray<FTransform> NewBoneTransforms;
	    FixedGetBoneTrackTransforms(AnimSequence, BoneName, NewBoneTransforms);

	    // 2. 扫描区间 (Start -> Peak -> End)
	    TArray<FFlyingSegment> Segments;
	    {
	        int32 StartF = -1;
	        int32 TempMaxFrame = -1;
	        float TempMaxAlpha = -1.0f;
	        bool bIsTracking = false;

	        for (int32 f = 0; f < NumKeys; f++)
	        {
	            float Alpha = CurveData->FloatCurve.Eval((float)f / FrameRate);
	            if (Alpha > 0.001f)
	            {
	                if (!bIsTracking)
	                {
	                    StartF = FMath::Max(0, f - 1);
	                    bIsTracking = true;
	                    TempMaxFrame = f;
	                    TempMaxAlpha = Alpha;
	                }
	                else if (Alpha > TempMaxAlpha)
	                {
	                    TempMaxAlpha = Alpha;
	                    TempMaxFrame = f;
	                }
	            }
	            else if (bIsTracking)
	            {
	                int32 EndF = f;
	                // 确保有明显的起飞过程
	                if (StartF != -1 && TempMaxFrame > StartF && EndF > TempMaxFrame && TempMaxAlpha > 0.1f)
	                {
	                    Segments.Add({StartF, TempMaxFrame, EndF, TempMaxAlpha});
	                }
	                bIsTracking = false;
	                StartF = -1; TempMaxFrame = -1; TempMaxAlpha = -1.0f;
	            }
	        }
	    }

	    // 3. 计算轨迹
	    for (const FFlyingSegment& Seg : Segments)
	    {
	        // A. 提取关键点 (Component Space)
	        FTransform StartParentCS = ParentCSTransforms[Seg.StartFrame];
	        FVector P_Start = StartParentCS.TransformPosition(NewBoneTransforms[Seg.StartFrame].GetLocation());

	        FTransform EndParentCS = ParentCSTransforms[Seg.EndFrame];
	        FVector P_End = EndParentCS.TransformPosition(NewBoneTransforms[Seg.EndFrame].GetLocation());

	        FTransform PeakRootCS = RootBoneTransforms.IsValidIndex(Seg.PeakFrame) ? RootBoneTransforms[Seg.PeakFrame] : FTransform::Identity;
	        FVector P_Target = PeakRootCS.TransformPosition(TargetLocation);

	        // B. 设定 Pivot (关键修改：使用 Start 和 End 的中点，通常就在角色身上)
	        // 这样可以确保轨迹是围绕角色的，能够形成漂亮的弧线
	        FVector Pivot = (P_Start + P_End) * 0.5f;

	        // C. 计算相对于 Pivot 的向量
	        FVector V_Start = P_Start - Pivot;
	        FVector V_Target = P_Target - Pivot;
	        FVector V_End = P_End - Pivot;

	        // D. 提取方向 (Quat) 和 半径 (Length)
	        // 注意：如果 Start 就在 Pivot 上 (V_Start接近0)，ToOrientationQuat 会不稳定
	        // 这种情况下我们用 Target 的方向作为起始方向的退化处理
	        FQuat Q_Target = V_Target.ToOrientationQuat();
	        
	        FQuat Q_Start = V_Start.SizeSquared() > 1.0f ? V_Start.ToOrientationQuat() : Q_Target;
	        FQuat Q_End = V_End.SizeSquared() > 1.0f ? V_End.ToOrientationQuat() : Q_Target;

	        float R_Start = V_Start.Size();
	        float R_Target = V_Target.Size();
	        float R_End = V_End.Size();

	        // E. 遍历插值
	        for (int32 f = Seg.StartFrame; f <= Seg.EndFrame; f++)
	        {
	            float CurrentTime = (float)f / FrameRate;
	            float RawAlpha = CurveData->FloatCurve.Eval(CurrentTime);
	            
	            // 归一化 Alpha (0 ~ 1.0)
	            float NormalizedAlpha = FMath::Clamp(RawAlpha / Seg.PeakAlpha, 0.0f, 1.0f);

	            FTransform CurrentParentCS = ParentCSTransforms[f];
	            FVector NewPosCS;

	            // 分段插值
	            if (f <= Seg.PeakFrame)
	            {
	                // --- 去程 (Start -> Target) ---
	                // Slerp 处理方向 (绕 Pivot 旋转)
	                FQuat CurrentQ = FQuat::Slerp(Q_Start, Q_Target, NormalizedAlpha);
	                // Lerp 处理距离 (螺旋变长)
	                float CurrentR = FMath::Lerp(R_Start, R_Target, NormalizedAlpha);
	                
	                NewPosCS = Pivot + (CurrentQ.GetForwardVector() * CurrentR);
	            }
	            else
	            {
	                // --- 回程 (Target -> End) ---
	                // 插值系数 T: 0.0 (Target) -> 1.0 (End)
	                float T = 1.0f - NormalizedAlpha;
	                
	                FQuat CurrentQ = FQuat::Slerp(Q_Target, Q_End, T);
	                float CurrentR = FMath::Lerp(R_Target, R_End, T);
	                
	                NewPosCS = Pivot + (CurrentQ.GetForwardVector() * CurrentR);
	            }

	            // 转换回 Local 并写入
	            FVector NewPosLocal = CurrentParentCS.InverseTransformPosition(NewPosCS);
	            NewBoneTransforms[f].SetLocation(NewPosLocal);
	        }
	    }

	    ModifyBoneTransformAndKeepChildren(AnimSequence, BoneName, NewBoneTransforms, false);

	    AnimSequence->GetController().CloseBracket();
	    AnimSequence->PostEditChange();
	}
	else
	{
		if (!AnimSequence || !AnimSequence->GetDataModel()) return;

	    const IAnimationDataModel* Model = AnimSequence->GetDataModel();
	    USkeleton* Skeleton = AnimSequence->GetSkeleton();
	    if (!Skeleton) return;

	    const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
	    int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneName);
	    if (BoneIndex == INDEX_NONE) return;

	    // 1. 修正：使用 FindFloatCurve 返回指针
	    const FFloatCurve* CurveData = Model->FindFloatCurve(FAnimationCurveIdentifier(AlphaCurveName, ERawCurveTrackTypes::RCT_Float));
	    if (!CurveData) 
	    {
	        UE_LOG(LogTemp, Warning, TEXT("Curve %s not found!"), *AlphaCurveName.ToString());
	        return;
	    }

	    AnimSequence->GetController().OpenBracket(FText::FromString("Modify Flying Sword Trajectory"));
	    AnimSequence->Modify();

	    int32 NumKeys = Model->GetNumberOfKeys();
	    double FrameRate = Model->GetFrameRate().AsDecimal();

	    // 2. 预计算父级世界变换 (Component Space)
	    TArray<int32> ParentIndices;
	    int32 Current = RefSkeleton.GetParentIndex(BoneIndex);
	    while (Current != INDEX_NONE)
	    {
	        ParentIndices.Insert(Current, 0);
	        Current = RefSkeleton.GetParentIndex(Current);
	    }

	    TArray<FTransform> ParentCSTransforms;
	    ParentCSTransforms.SetNum(NumKeys);
	    
	    TArray<TArray<FTransform>> CachedParentTracks; 
	    for (int32 PID : ParentIndices)
	    {
	    	TArray<FTransform> Track;
	        FixedGetBoneTrackTransforms(AnimSequence, RefSkeleton.GetBoneName(PID), Track);
	        CachedParentTracks.Add(Track);
	    }

	    for (int32 f = 0; f < NumKeys; f++)
	    {
	        FTransform AccumTM = FTransform::Identity;
	        for (const TArray<FTransform>& Track : CachedParentTracks)
	        {
	            if (Track.IsValidIndex(f)) AccumTM = Track[f] * AccumTM;
	        }
	        ParentCSTransforms[f] = AccumTM;
	    }
		
	    TArray<FTransform> RootBoneTransforms;
	    FixedGetBoneTrackTransforms(AnimSequence, RefSkeleton.GetBoneName(0), RootBoneTransforms);
	    TArray<FTransform> NewBoneTransforms;
	    FixedGetBoneTrackTransforms(AnimSequence, BoneName, NewBoneTransforms);

	    // 3. 扫描区间：只关心 Start 和 End，不管 Peak
	    struct FFlyingRange { int32 Start; int32 End; };
	    TArray<FFlyingRange> Ranges;
	    
	    {
	        int32 StartF = -1;
	        bool bTracking = false;
	        for (int32 f = 0; f < NumKeys; f++)
	        {
	            float Time = (float)f / FrameRate;
	            float Alpha = CurveData->FloatCurve.Eval(Time);
	            
	            if (Alpha > KINDA_SMALL_NUMBER)
	            {
	                if (!bTracking)
	                {
	                    StartF = FMath::Max(0, f - 1); // 向前取一帧作为起点
	                    bTracking = true;
	                }
	            }
	            else if (bTracking)
	            {
	                // 飞行结束
	                int32 EndF = f;
	                if (StartF != -1 && EndF > StartF)
	                {
	                    Ranges.Add({StartF, EndF});
	                }
	                bTracking = false;
	                StartF = -1;
	            }
	        }
	        // 处理曲线在最后一帧结束的情况
	        if (bTracking && StartF != -1)
	        {
	            Ranges.Add({StartF, NumKeys - 1});
	        }
	    }

	    // 4. 轨迹计算 (加法混合)
	    for (const FFlyingRange& Range : Ranges)
	    {
	        // 提取 Start 和 End 的世界位置
	        FTransform StartParentCS = ParentCSTransforms[Range.Start];
	        FVector P_Start = StartParentCS.TransformPosition(NewBoneTransforms[Range.Start].GetLocation());

	        FTransform EndParentCS = ParentCSTransforms[Range.End];
	        FVector P_End = EndParentCS.TransformPosition(NewBoneTransforms[Range.End].GetLocation());

	        // 目标位置需要根据每一帧的 Root 计算（处理 Root Motion）
	        // 或者简单点：如果 Target 相对 Root 静止，取每一帧的 Root 变换
	        
	        for (int32 f = Range.Start; f <= Range.End; f++)
	        {
	            float CurrentTime = (float)f / FrameRate;
	            float Alpha = CurveData->FloatCurve.Eval(CurrentTime);
	            
	            // A. 计算时间归一化进度 (0.0 ~ 1.0)
	            float TimeProgress = (float)(f - Range.Start) / (float)(Range.End - Range.Start);
	            
	            // B. 计算当前帧的基础位置 (Base Position)
	            // 在 Start 和 End 之间做插值。Lerp 是直线，Slerp 是大圆。
	            // 既然 Start 和 End 通常很近（都在手上），Lerp 足够了。
	            FVector P_Base = FMath::Lerp(P_Start, P_End, TimeProgress);
	            
	            // C. 计算当前帧的目标位置 (Target Position)
	            // Target 是相对于 Root 的
	            FTransform RootCS = RootBoneTransforms.IsValidIndex(f) ? RootBoneTransforms[f] : FTransform::Identity;
	            FVector P_Target = RootCS.TransformPosition(TargetLocation);

	            // D. 核心逻辑：加法混合
	            // 最终位置 = 从 Base 插值向 Target，权重为 Alpha
	            FVector P_Final = FMath::Lerp(P_Base, P_Target, Alpha);

	            // E. 转回 Local 并写入
	            FTransform CurrentParentCS = ParentCSTransforms[f];
	            FVector NewPosLocal = CurrentParentCS.InverseTransformPosition(P_Final);
	            
	            NewBoneTransforms[f].SetLocation(NewPosLocal);
	        }
	    }

	    // 5. 提交
	    ModifyBoneTransformAndKeepChildren(AnimSequence, BoneName, NewBoneTransforms, false);

	    AnimSequence->GetController().CloseBracket();
	    AnimSequence->PostEditChange();
		
	}
}

void UNipcatLocomotionSystemEditorFunctionLibrary::ModifyBoneTrajectory_Bilinear(UAnimSequence* AnimSequence,
	FName BoneName, FName DistanceCurveName, FVector ForwardDirection)
{
	if (!AnimSequence || !AnimSequence->GetDataModel()) return;

    const IAnimationDataModel* Model = AnimSequence->GetDataModel();
    USkeleton* Skeleton = AnimSequence->GetSkeleton();
    if (!Skeleton) return;

    const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
    int32 BoneIndex = RefSkeleton.FindBoneIndex(BoneName);
    if (BoneIndex == INDEX_NONE) return;

    // 1. 获取距离曲线
    const FFloatCurve* CurveData = Model->FindFloatCurve(FAnimationCurveIdentifier(DistanceCurveName, ERawCurveTrackTypes::RCT_Float));
    if (!CurveData) 
    {
        UE_LOG(LogTemp, Warning, TEXT("Distance Curve %s not found!"), *DistanceCurveName.ToString());
        return;
    }

    AnimSequence->GetController().OpenBracket(FText::FromString("Modify Flying Sword Trajectory"));
    AnimSequence->Modify();

    int32 NumKeys = Model->GetNumberOfKeys();
    double FrameRate = Model->GetFrameRate().AsDecimal();

    // 2. 预计算父级世界变换 (Component Space)
    TArray<int32> ParentIndices;
    int32 Current = RefSkeleton.GetParentIndex(BoneIndex);
    while (Current != INDEX_NONE)
    {
        ParentIndices.Insert(Current, 0);
        Current = RefSkeleton.GetParentIndex(Current);
    }

    TArray<FTransform> ParentCSTransforms;
    ParentCSTransforms.SetNum(NumKeys);
    
    TArray<TArray<FTransform>> CachedParentTracks; 
    for (int32 PID : ParentIndices)
    {
        TArray<FTransform> Track;
        FixedGetBoneTrackTransforms(AnimSequence, RefSkeleton.GetBoneName(PID), Track);
        CachedParentTracks.Add(Track);
    }

    for (int32 f = 0; f < NumKeys; f++)
    {
        FTransform AccumTM = FTransform::Identity;
        for (const TArray<FTransform>& Track : CachedParentTracks)
        {
            if (Track.IsValidIndex(f)) AccumTM = Track[f] * AccumTM;
        }
        ParentCSTransforms[f] = AccumTM;
    }

    // 获取 Root 数据 (用于计算 ForwardDirection 的实际朝向)
    TArray<FTransform> RootBoneTransforms;
    FixedGetBoneTrackTransforms(AnimSequence, RefSkeleton.GetBoneName(0), RootBoneTransforms);
    
    // 获取骨骼原始数据
    TArray<FTransform> NewBoneTransforms;
    FixedGetBoneTrackTransforms(AnimSequence, BoneName, NewBoneTransforms);

    // 3. 扫描曲线区间 (Distance > 0 的部分)
    // 支持多段飞行
    struct FFlyingRange { int32 Start; int32 End; };
    TArray<FFlyingRange> Ranges;
    
    {
        int32 StartF = -1;
        bool bTracking = false;
        for (int32 f = 0; f < NumKeys; f++)
        {
            float Time = (float)f / FrameRate;
            float Dist = CurveData->FloatCurve.Eval(Time);
            
            // 设定一个小的阈值
            if (FMath::Abs(Dist) > 0.1f)
            {
                if (!bTracking)
                {
                    StartF = FMath::Max(0, f - 1); // 包含起飞前一帧以保证连续
                    bTracking = true;
                }
            }
            else if (bTracking)
            {
                int32 EndF = f;
                if (StartF != -1 && EndF > StartF)
                {
                    Ranges.Add({StartF, EndF});
                }
                bTracking = false;
                StartF = -1;
            }
        }
        if (bTracking && StartF != -1)
        {
            Ranges.Add({StartF, NumKeys - 1});
        }
    }

    // 4. 轨迹计算 (加法偏移)
    // Normalize ForwardDirection
    FVector NormalizedForward = ForwardDirection.GetSafeNormal();
    if (NormalizedForward.IsZero()) NormalizedForward = FVector(0, 1, 0); // Default Forward

    for (const FFlyingRange& Range : Ranges)
    {
        // 提取区间的 Start 和 End 位置 (Component Space)
        FTransform StartParentCS = ParentCSTransforms[Range.Start];
        FVector P_Start = StartParentCS.TransformPosition(NewBoneTransforms[Range.Start].GetLocation());

        FTransform EndParentCS = ParentCSTransforms[Range.End];
        FVector P_End = EndParentCS.TransformPosition(NewBoneTransforms[Range.End].GetLocation());

        for (int32 f = Range.Start; f <= Range.End; f++)
        {
            float CurrentTime = (float)f / FrameRate;
            float Distance = CurveData->FloatCurve.Eval(CurrentTime);
            
            // A. 计算当前的基础位置 (Base Position)
            // 随着时间推移，Base 从 P_Start 移动到 P_End
            // 这样当 Curve=0 时，飞剑位置等于 Base，保证不跳变
            float Progress = (float)(f - Range.Start) / (float)(Range.End - Range.Start);
            FVector P_Base = FMath::Lerp(P_Start, P_End, Progress);

            // B. 计算偏移向量 (Offset)
            // 偏移方向 = Root的旋转 * 指定的ForwardDirection
            // 这样如果角色转身，飞剑轨迹也会跟着转
            FTransform RootCS = RootBoneTransforms.IsValidIndex(f) ? RootBoneTransforms[f] : FTransform::Identity;
            FVector CurrentDir = RootCS.TransformVectorNoScale(NormalizedForward);
            
            FVector Offset = CurrentDir * Distance;

            // C. 最终位置
            // Final = 动点Origin + 方向 * 距离
            FVector P_Final = P_Base + Offset;

            // D. 转回 Local Space 并写入
            FTransform CurrentParentCS = ParentCSTransforms[f];
            FVector NewPosLocal = CurrentParentCS.InverseTransformPosition(P_Final);
            
            NewBoneTransforms[f].SetLocation(NewPosLocal);
        }
    }

    // 5. 提交修改
    ModifyBoneTransformAndKeepChildren(AnimSequence, BoneName, NewBoneTransforms, false);

    AnimSequence->GetController().CloseBracket();
    AnimSequence->PostEditChange();
}

void UNipcatLocomotionSystemEditorFunctionLibrary::CookBoneLocationToCurve(UAnimSequence* AnimSequence, FName BoneName,
                                                                           FName XCurveName, FName YCurveName, FName ZCurveName)
{
	if (!AnimSequence || !AnimSequence->GetDataModel()) return;

    USkeleton* Skeleton = AnimSequence->GetSkeleton();
    if (!Skeleton) return;

    const FReferenceSkeleton& RefSkeleton = Skeleton->GetReferenceSkeleton();
    int32 TargetBoneIndex = RefSkeleton.FindBoneIndex(BoneName);
    if (TargetBoneIndex == INDEX_NONE) 
    {
        UE_LOG(LogTemp, Warning, TEXT("CookBoneLocationToCurve: Bone %s not found."), *BoneName.ToString());
        return;
    }

    // 开启事务 (Undo/Redo)
    AnimSequence->GetController().OpenBracket(FText::FromString("Cook Bone Location To Curve"));
    AnimSequence->Modify();

    int32 NumKeys = AnimSequence->GetDataModel()->GetNumberOfKeys();

    // -------------------------------------------------------------------------
    // 1. 计算目标骨骼的 Component Space 变换
    // -------------------------------------------------------------------------
    
    // 获取从 Root 到 TargetBone 的父级链 (不包含 Root 本身，因为 Root 的 ParentIndex 是 -1)
    // 顺序：Root(0) -> Spine -> ... -> TargetBone
    TArray<int32> BoneChain;
    {
        int32 Current = TargetBoneIndex;
        while (Current != INDEX_NONE)
        {
            BoneChain.Insert(Current, 0); // 插入头部
            Current = RefSkeleton.GetParentIndex(Current);
        }
    }

    // 缓存链条上所有骨骼的 Local Transforms
    TArray<TArray<FTransform>> CachedChainTracks;
    CachedChainTracks.Reserve(BoneChain.Num());

    for (int32 BoneIdx : BoneChain)
    {
        FName CurrentBoneName = RefSkeleton.GetBoneName(BoneIdx);
        TArray<FTransform> Track;
        // 使用修复版函数获取数据
        FixedGetBoneTrackTransforms(AnimSequence, CurrentBoneName, Track);
        CachedChainTracks.Add(Track);
    }

    // -------------------------------------------------------------------------
    // 2. 准备 Root Bone 数据 (用于计算相对坐标)
    // -------------------------------------------------------------------------
    TArray<FTransform> RootTransforms;
    FName RootBoneName = RefSkeleton.GetBoneName(0);
    FixedGetBoneTrackTransforms(AnimSequence, RootBoneName, RootTransforms);

    // -------------------------------------------------------------------------
    // 3. 逐帧计算相对位置并记录
    // -------------------------------------------------------------------------
    TArray<FRichCurveKey> ValuesX, ValuesY, ValuesZ;
    ValuesX.SetNumUninitialized(NumKeys);
    ValuesY.SetNumUninitialized(NumKeys);
    ValuesZ.SetNumUninitialized(NumKeys);

    for (int32 f = 0; f < NumKeys; f++)
    {
        // A. 计算 Target Bone 在 Component Space 的变换
        FTransform TargetBoneCS = FTransform::Identity;
        for (const TArray<FTransform>& Track : CachedChainTracks)
        {
            // 链式乘法：ChildLocal * ParentCS
            // 注意：CachedChainTracks 是从 Root 开始存的，所以顺序是从父到子
            // Transform 乘法顺序：Local * Parent (UE 顺序)
            TargetBoneCS = Track[f] * TargetBoneCS;
        }

        // B. 获取 Root Bone 当前帧的变换 (Local 即 Component Space，因为 Root 无父级)
        FTransform RootBoneCS = RootTransforms[f];

        // C. 计算相对变换 (Target 相对于 Root 的位置)
        // Relative = TargetCS * RootCS^(-1)
        FTransform RelativeTM = TargetBoneCS.GetRelativeTransform(RootBoneCS);
        FVector RelativeLoc = RelativeTM.GetLocation();

    	float t = AnimSequence->GetTimeAtFrame(f);
    	ValuesX[f].Time = t;
        ValuesX[f].Value = RelativeLoc.X;
    	ValuesY[f].Time = t;
        ValuesY[f].Value = RelativeLoc.Y;
    	ValuesZ[f].Time = t;
        ValuesZ[f].Value = RelativeLoc.Z;
    }

    // -------------------------------------------------------------------------
    // 4. 写入曲线数据
    // -------------------------------------------------------------------------
    auto WriteCurve = [&](FName CurveName, const TArray<FRichCurveKey>& Values)
    {
        if (CurveName == NAME_None) return;

        FAnimationCurveIdentifier CurveId(CurveName, ERawCurveTrackTypes::RCT_Float);
        
        // 如果曲线不存在，则添加
        if (!AnimSequence->GetDataModel()->FindFloatCurve(CurveId))
        {
            AnimSequence->GetController().AddCurve(CurveId);
        }

        // 写入所有帧的数据 (SetCurveKeys 更高效，自动处理 Time)
        AnimSequence->GetController().SetCurveKeys(CurveId, Values);
    };

    WriteCurve(XCurveName, ValuesX);
    WriteCurve(YCurveName, ValuesY);
    WriteCurve(ZCurveName, ValuesZ);

    AnimSequence->GetController().CloseBracket();
    AnimSequence->PostEditChange();
}

#undef LOCTEXT_NAMESPACE

#endif
