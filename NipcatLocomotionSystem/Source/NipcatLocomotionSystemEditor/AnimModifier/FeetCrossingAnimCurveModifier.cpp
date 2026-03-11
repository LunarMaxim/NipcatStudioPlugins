// Copyright Nipcat Studio All Rights Reserved.


#include "FeetCrossingAnimCurveModifier.h"

#include "AnimPose.h"


void UFeetCrossingAnimCurveModifier::OnApply_Implementation(UAnimSequence* InAnimation)
{
	/*
	 *
	const FAnimationCurveIdentifier CurveId(CurveName, ERawCurveTrackTypes::RCT_Float);
	// 不重复添加
	if (InAnimation->GetDataModel()->FindCurve(CurveId))
	{
		return;
	}
	
	IAnimationDataController& Controller = InAnimation->GetController();
	
	// 获取所需的骨骼transform
	FTransform SpineTransform = UAnimPoseExtensions::GetBonePose(AnimPose, SpineBoneName, EAnimPoseSpaces::World);
	FTransform RootTransform = UAnimPoseExtensions::GetBonePose(AnimPose, RootBoneName, EAnimPoseSpaces::World);
	FVector LeftFootPos = UAnimPoseExtensions::GetBonePose(AnimPose, LeftBallBoneName, EAnimPoseSpaces::World).GetLocation();
	FVector RightFootPos = UAnimPoseExtensions::GetBonePose(AnimPose, RightBallBoneName, EAnimPoseSpaces::World).GetLocation();

	// 获取朝向
	FVector SpineDirection;
	switch (SpineBoneForwardAxis)
	{
	case EAxis::X:
		SpineDirection = SpineTransform.GetRotation().GetForwardVector();
		break;
	case EAxis::Y:
		SpineDirection = SpineTransform.GetRotation().GetRightVector();
		break;
	case EAxis::Z:
		SpineDirection = SpineTransform.GetRotation().GetUpVector();
		break;
	default:
		SpineDirection = SpineTransform.GetRotation().GetRightVector();
		break;
	}
	bool bFacingRight = (SpineDirection.X > 0.0f);

	float CurveValue = bCurrentCrossing ? 1.0f : 0.0f;
	FRichCurveKey Key;
	Key.Time = Time;
	Key.Value = CurveValue;
	Key.InterpMode = RCIM_Constant;
	Controller.SetCurveKey(CurveId, Key);
	*/
	Super::OnApply_Implementation(InAnimation);
    
    if (!InAnimation)
    {
        return;
    }
	
	const FAnimationCurveIdentifier CurveId(CurveName, ERawCurveTrackTypes::RCT_Float);
	// 不重复添加
	if (InAnimation->GetDataModel()->FindCurve(CurveId))
	{
		return;
	}
	
    IAnimationDataController& Controller = InAnimation->GetController();

    UAnimationBlueprintLibrary::AddCurve(InAnimation, CurveName, ERawCurveTrackTypes::RCT_Float);

    const float SequenceLength = InAnimation->GetPlayLength();
    const float SampleStep = 1.0f/static_cast<float>(SampleRate);
    const int32 SampleNum = FMath::TruncToInt(SequenceLength/SampleStep);

    const FAnimPoseEvaluationOptions EvalOptions{EAnimDataEvalType::Raw, true, false, false, nullptr, true, false};

	// 获取首帧数据
	FAnimPose FirstFramePose;
	UAnimPoseExtensions::GetAnimPoseAtTime(InAnimation, 0.0f, EvalOptions, FirstFramePose);

	FTransform FirstFrameSpineTransform = UAnimPoseExtensions::GetBonePose(FirstFramePose, SpineBoneName, EAnimPoseSpaces::World);
	FVector FirstFrameRootPos = UAnimPoseExtensions::GetBonePose(FirstFramePose, RootBoneName, EAnimPoseSpaces::World).GetLocation();
	FVector FirstFrameLeftFootPos = UAnimPoseExtensions::GetBonePose(FirstFramePose, LeftBallBoneName, EAnimPoseSpaces::World).GetLocation();
	FVector FirstFrameRightFootPos = UAnimPoseExtensions::GetBonePose(FirstFramePose, RightBallBoneName, EAnimPoseSpaces::World).GetLocation();

	// 获取第二帧数据用于确定初始运动趋势
	FAnimPose SecondFramePose;
	UAnimPoseExtensions::GetAnimPoseAtTime(InAnimation, SampleStep, EvalOptions, SecondFramePose);
	FVector SecondFrameRootPos = UAnimPoseExtensions::GetBonePose(SecondFramePose, RootBoneName, EAnimPoseSpaces::World).GetLocation();
	FVector SecondFrameLeftFootPos = UAnimPoseExtensions::GetBonePose(SecondFramePose, LeftBallBoneName, EAnimPoseSpaces::World).GetLocation();
	FVector SecondFrameRightFootPos = UAnimPoseExtensions::GetBonePose(SecondFramePose, RightBallBoneName, EAnimPoseSpaces::World).GetLocation();

	// 确定初始状态
	FVector FirstFrameSpineDirection;
	switch (SpineBoneForwardAxis)
	{
	case EAxis::X:
		FirstFrameSpineDirection = FirstFrameSpineTransform.GetRotation().GetForwardVector();
		break;
	case EAxis::Y:
		FirstFrameSpineDirection = FirstFrameSpineTransform.GetRotation().GetRightVector();
		break;
	case EAxis::Z:
		FirstFrameSpineDirection = FirstFrameSpineTransform.GetRotation().GetUpVector();
		break;
	default:
		FirstFrameSpineDirection = FirstFrameSpineTransform.GetRotation().GetRightVector();
		break;
	}
	bool bFirstFrameFacingRight = (FirstFrameSpineDirection.X < 0.0f);
	float FirstFrameFootDistance = FVector::DistXY(FirstFrameLeftFootPos, FirstFrameRightFootPos);
	float SecondFrameFootDistance = FVector::DistXY(SecondFrameLeftFootPos, SecondFrameRightFootPos);
	bool bInitiallyIncreasingDistance = (SecondFrameFootDistance > FirstFrameFootDistance);

	// 根据朝向和脚的位置确定初始crossing状态
	bool bPreviousCrossing;
	if(bFirstFrameFacingRight)
	{
		// 朝右时，判断左脚是否在root点右边
		// 或是左脚正在朝右运动 且 右脚在向左移动
		bPreviousCrossing = (FirstFrameLeftFootPos.X < FirstFrameRootPos.X) ||
			((SecondFrameLeftFootPos - SecondFrameRootPos) - (FirstFrameLeftFootPos - FirstFrameRootPos)).X < 0 && // run loop fr
			((SecondFrameRightFootPos - SecondFrameRootPos) - (FirstFrameRightFootPos - FirstFrameRootPos)).X > 0 ;
	}
	else if(!bFirstFrameFacingRight)
	{
		// 朝左时，判断右脚是否在root点左边
		// 或是左脚正在朝右运动 且 右脚在向左移动
		bPreviousCrossing = (FirstFrameRightFootPos.X > FirstFrameRootPos.X) ||
			((SecondFrameLeftFootPos - SecondFrameRootPos) - (FirstFrameLeftFootPos - FirstFrameRootPos)).X < 0 &&
			((SecondFrameRightFootPos - SecondFrameRootPos) - (FirstFrameRightFootPos - FirstFrameRootPos)).X > 0 ;
	}

	// 添加首帧关键帧
	FRichCurveKey FirstKey;
	FirstKey.Time = 0.0f;
	FirstKey.Value = bPreviousCrossing ? 1.0f : 0.0f;
	FirstKey.InterpMode = RCIM_Constant;
	Controller.SetCurveKey(CurveId, FirstKey);
	
	float PrevFootDistance = FirstFrameFootDistance;
	FVector PrevLeftFootPos = FirstFrameLeftFootPos;
	FVector PrevRightFootPos = FirstFrameRightFootPos;
	bool bIsIncreasingDistance = bInitiallyIncreasingDistance;

	// 从第二帧开始的主循环
	for(int32 SampleIndex = 1; SampleIndex < SampleNum; ++SampleIndex)
	{
		const float Time = FMath::Clamp(static_cast<float>(SampleIndex) * SampleStep, 0.0f, SequenceLength);
        
		FAnimPose AnimPose;
		UAnimPoseExtensions::GetAnimPoseAtTime(InAnimation, Time, EvalOptions, AnimPose);

        FTransform SpineTransform = UAnimPoseExtensions::GetBonePose(AnimPose, SpineBoneName, EAnimPoseSpaces::World);
        FTransform RootTransform = UAnimPoseExtensions::GetBonePose(AnimPose, RootBoneName, EAnimPoseSpaces::World);
        FVector LeftFootPos = UAnimPoseExtensions::GetBonePose(AnimPose, LeftBallBoneName, EAnimPoseSpaces::World).GetLocation();
        FVector RightFootPos = UAnimPoseExtensions::GetBonePose(AnimPose, RightBallBoneName, EAnimPoseSpaces::World).GetLocation();

        // 获取朝向
        FVector SpineDirection;
        switch (SpineBoneForwardAxis)
        {
        case EAxis::X:
            SpineDirection = SpineTransform.GetRotation().GetForwardVector();
            break;
        case EAxis::Y:
            SpineDirection = SpineTransform.GetRotation().GetRightVector();
            break;
        case EAxis::Z:
            SpineDirection = SpineTransform.GetRotation().GetUpVector();
            break;
        default:
            SpineDirection = SpineTransform.GetRotation().GetRightVector();
            break;
        }
        bool bFacingRight = (SpineDirection.X < 0.0f);

		float CurrentFootDistance = FVector::DistXY(LeftFootPos, RightFootPos);
		bool bCurrentlyIncreasingDistance = (CurrentFootDistance > PrevFootDistance);
        
		bool bLeftFootPassedRoot = 
			((bFacingRight && LeftFootPos.X < RootTransform.GetLocation().X && PrevLeftFootPos.X >= RootTransform.GetLocation().X) ||
			 (!bFacingRight && LeftFootPos.X > RootTransform.GetLocation().X && PrevLeftFootPos.X <= RootTransform.GetLocation().X));
             
		bool bRightFootPassedRoot = 
			((bFacingRight && RightFootPos.X < RootTransform.GetLocation().X && PrevRightFootPos.X >= RootTransform.GetLocation().X) ||
			 (!bFacingRight && RightFootPos.X > RootTransform.GetLocation().X && PrevRightFootPos.X <= RootTransform.GetLocation().X));

		bool bCurrentCrossing = bPreviousCrossing;
        
		if (bIsIncreasingDistance && !bCurrentlyIncreasingDistance)
		{
			bCurrentCrossing = true;
		}
        
		if ((bFacingRight && bRightFootPassedRoot) || (!bFacingRight && bLeftFootPassedRoot))
		{
			bCurrentCrossing = false;
		}

		if (bCurrentCrossing != bPreviousCrossing || SampleIndex == SampleNum - 1)
		{
			float CurveValue = bCurrentCrossing ? 1.0f : 0.0f;
			FRichCurveKey Key;
			Key.Time = Time;
			Key.Value = CurveValue;
			Key.InterpMode = RCIM_Constant;
			Controller.SetCurveKey(CurveId, Key);
		}
		bPreviousCrossing = bCurrentCrossing;
		bIsIncreasingDistance = bCurrentlyIncreasingDistance;
		PrevFootDistance = CurrentFootDistance;
		PrevLeftFootPos = LeftFootPos;
		PrevRightFootPos = RightFootPos;
	}
}

void UFeetCrossingAnimCurveModifier::OnRevert_Implementation(UAnimSequence* AnimationSequence)
{
	if (AnimationSequence->HasCurveData(CurveName, false))
	{
		UAnimationBlueprintLibrary::RemoveCurve(AnimationSequence, CurveName);
	}
}
