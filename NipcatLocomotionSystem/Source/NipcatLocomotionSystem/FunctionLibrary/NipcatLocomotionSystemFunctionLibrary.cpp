// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatLocomotionSystemFunctionLibrary.h"


bool UNipcatLocomotionSystemFunctionLibrary::IsAnimNodeEquals(FAnimNodeReference NodeA, FAnimNodeReference NodeB)
{
	return NodeA.GetAnimNodePtr<FAnimNode_Base>() == NodeB.GetAnimNodePtr<FAnimNode_Base>();
}

FVector UNipcatLocomotionSystemFunctionLibrary::CalculateStopLocation(const FVector& Velocity, const bool bUseSeparateBrakingFriction,
	const float BrakingFriction, const float GroundFriction, const float BrakingFrictionFactor,
	const float BrakingDecelerationWalking, const float DeltaTime)
{
	/** Select the appropriate friction values to use. */
	const float AppliedFriction = bUseSeparateBrakingFriction ? BrakingFriction : GroundFriction;
	const float Friction = FMath::Max(0.0f, AppliedFriction * BrakingFrictionFactor);
	const float BrakingDeceleration = FMath::Max(0.0f, BrakingDecelerationWalking);
	const FVector BrakingAcceleration = BrakingDeceleration * Velocity.GetSafeNormal(); 

	FVector PredictedAcceleration;
	FVector PredictedVelocity = Velocity;
	FVector PredictedStopLocation = FVector::ZeroVector;
	float PredictionTime = 0.0f;

	while (PredictionTime < 2.0f)
	{
		PredictedAcceleration = -Friction * PredictedVelocity - BrakingAcceleration;
		PredictedVelocity = PredictedVelocity + PredictedAcceleration * DeltaTime;

		/** If any component of the PredictedVelocity vector is antiparallel to the Velocity, immediately return true before
		the character is "simulated" moving backwards. (The owning character can't move in reverse if there is no input acceleration.) */
		if ((PredictedVelocity | Velocity) <= 0.0f)
		{
			return PredictedStopLocation;
		}

		PredictedStopLocation = PredictedStopLocation + PredictedVelocity * DeltaTime;
		PredictionTime += DeltaTime;
	}

	return PredictedStopLocation;
}

FVector UNipcatLocomotionSystemFunctionLibrary::CalculateStopSpeed(const float& StopDistance,
	const bool bUseSeparateBrakingFriction, const float BrakingFriction, const float GroundFriction,
	const float BrakingFrictionFactor, const float BrakingDecelerationWalking, const float DeltaTime)
{
	// 如果目标距离接近0，直接返回0速度
	if (FMath::IsNearlyZero(StopDistance, 1.0f))
	{
		return FVector::ZeroVector;
	}

	// 设置二分查找的速度范围
	float MinSpeed = 0.0f;
	float MaxSpeed = 1000.0f;  // 设置一个合理的最大速度上限
	float CurrentSpeed = 0.0f;
    
	// 设置误差容忍度
	const float Tolerance = 1.0f;
	const int32 MaxIterations = 20;  // 最大迭代次数
	int32 IterationCount = 0;

	while (IterationCount < MaxIterations)
	{
		// 取中间速度值
		CurrentSpeed = (MinSpeed + MaxSpeed) * 0.5f;
        
		// 使用当前速度计算停止位置
		FVector CurrentVelocity = FVector(CurrentSpeed, 0.0f, 0.0f);
		FVector PredictedStopLocation = CalculateStopLocation(CurrentVelocity, 
			bUseSeparateBrakingFriction, 
			BrakingFriction, 
			GroundFriction, 
			BrakingFrictionFactor, 
			BrakingDecelerationWalking, 
			DeltaTime);
        
		// 计算预测距离与目标距离的差值
		float PredictedDistance = PredictedStopLocation.Size();
		float DistanceError = PredictedDistance - StopDistance;
        
		// 如果误差在容忍范围内，返回结果
		if (FMath::Abs(DistanceError) <= Tolerance)
		{
			return FVector(CurrentSpeed, 0.0f, 0.0f);
		}
        
		// 调整搜索范围
		if (PredictedDistance < StopDistance)
		{
			MinSpeed = CurrentSpeed;
		}
		else
		{
			MaxSpeed = CurrentSpeed;
		}
        
		IterationCount++;
	}
    
	// 达到最大迭代次数后返回最后的结果
	return FVector(CurrentSpeed, 0.0f, 0.0f);
}
