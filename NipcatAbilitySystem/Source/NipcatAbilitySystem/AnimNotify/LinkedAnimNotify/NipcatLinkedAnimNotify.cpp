// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatLinkedAnimNotify.h"

#include "MathUtil.h"
#include "NipcatBasicGameplay/FunctionLibrary/NipcatBasicGameplayFunctionLibrary.h"


TArray<FAnimNotifyData> FNipcatLinkedAnimNotify::ConvertLinkedNotifyData(
	TArray<FNipcatLinkedAnimNotify> LinkedAnimNotifies, float SourceStartTime, float SourceEndTime, const UAnimSequenceBase* AnimSequenceBase)
{
	TArray<FAnimNotifyData> Result;
	
	if (!AnimSequenceBase || LinkedAnimNotifies.IsEmpty())
	{
		return Result;
	}
	
	const FFloatCurve* PlayRateCurve = nullptr;
	
	for (const FFloatCurve& FloatCurve : AnimSequenceBase->GetCurveData().FloatCurves)
	{
		if (FloatCurve.GetName() == TEXT("PlayRate"))
		{
			PlayRateCurve = &FloatCurve;
		}
	}
	
	float SourceActualStartTime = SourceStartTime;
	float SourceActualEndTime = SourceEndTime;
	
	if (PlayRateCurve)
	{
		SourceActualStartTime = UNipcatBasicGameplayFunctionLibrary::GetScaledTimeStep(*PlayRateCurve, 0.f, SourceStartTime);
		const float SourceActualDuration = UNipcatBasicGameplayFunctionLibrary::GetScaledTimeStep(*PlayRateCurve, SourceStartTime, SourceEndTime - SourceStartTime);
		SourceActualEndTime = SourceActualStartTime + SourceActualDuration;
	}
	
	for (const auto LinkedAnimNotify : LinkedAnimNotifies)
	{
		FAnimNotifyData AnimNotifyData;
		if (LinkedAnimNotify.IsNotifyState)
		{
			AnimNotifyData.NotifyState = LinkedAnimNotify.NotifyState;
		}
		else
		{
			AnimNotifyData.Notify = LinkedAnimNotify.Notify;
		}
		
		const float ActualStartTime = FMathf::Max(0, SourceActualStartTime + LinkedAnimNotify.TriggerTimeOffset);
		const float ActualEndTime = SourceActualEndTime + LinkedAnimNotify.EndTriggerTimeOffset;

		float OriginStartTime = ActualStartTime;
		float OriginEndTime = ActualEndTime;

		if (PlayRateCurve)
		{
			OriginStartTime = UNipcatBasicGameplayFunctionLibrary::GetOriginalTimeStep(*PlayRateCurve, 0, ActualStartTime);
			OriginEndTime = FMathf::Min(AnimSequenceBase->GetPlayLength(), UNipcatBasicGameplayFunctionLibrary::GetOriginalTimeStep(*PlayRateCurve, 0, ActualEndTime));
		}
		
		AnimNotifyData.StartTime = OriginStartTime;
		AnimNotifyData.EndTime = OriginEndTime;
		
		Result.Add(AnimNotifyData);
	}
	return Result;
}
