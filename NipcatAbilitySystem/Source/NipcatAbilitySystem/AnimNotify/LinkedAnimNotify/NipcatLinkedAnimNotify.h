// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "NipcatAbilitySystem/AbilityNotify/AbilityNotifyInterface.h"
#include "NipcatLinkedAnimNotify.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FAnimNotifyData
{
	GENERATED_BODY()

	FAnimNotifyData(){}

	FAnimNotifyData(const FAnimNotifyEvent& AnimNotifyEvent)
	{
		Notify = AnimNotifyEvent.Notify.Get();
		NotifyState = AnimNotifyEvent.NotifyStateClass.Get();
		StartTime = AnimNotifyEvent.GetTime();
		EndTime = NotifyState ? AnimNotifyEvent.EndLink.GetTime() : StartTime;
		if (Notify && Notify->GetClass()->ImplementsInterface(UAbilityNotifyInterface::StaticClass()))
		{
			AlwaysTriggerBeforeStartTimeSeconds = IAbilityNotifyInterface::Execute_GetAlwaysTriggerBeforeStartTimeSeconds(Notify);
			AlwaysTriggerAfterInterrupt = IAbilityNotifyInterface::Execute_GetAlwaysTriggerAfterInterrupt(Notify);
		}
	}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UAnimNotify* Notify = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced)
	UAnimNotifyState* NotifyState = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float StartTime = 0.f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float EndTime = 0.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AlwaysTriggerBeforeStartTimeSeconds = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool AlwaysTriggerAfterInterrupt = false;
};

USTRUCT(BlueprintType)
struct NIPCATABILITYSYSTEM_API FNipcatLinkedAnimNotify
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool IsNotifyState = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(EditCondition="!IsNotifyState"))
	UAnimNotify* Notify = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Instanced, meta=(EditCondition="IsNotifyState"))
	UAnimNotifyState* NotifyState = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float TriggerTimeOffset = -0.2f;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition="IsNotifyState"))
	float EndTriggerTimeOffset = 0.2f;
	
	static TArray<FAnimNotifyData> ConvertLinkedNotifyData(TArray<FNipcatLinkedAnimNotify> LinkedAnimNotifies, float SourceStartTime, float SourceEndTime, const UAnimSequenceBase* AnimSequenceBase);
};