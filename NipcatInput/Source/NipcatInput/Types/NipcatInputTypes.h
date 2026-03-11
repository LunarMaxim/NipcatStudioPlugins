// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "NipcatInputTypes.generated.h"


struct FGameplayTag;
class UInputAction;

USTRUCT(BlueprintType)
struct NIPCATINPUT_API FNipcatInputActionEvent
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool CanBeBlocked = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UInputAction*> BufferToCancelOnTriggered;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition = "CanBeBlocked", ClampMin = 0))
	int32 BlockLevel = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition = "CanBeBlocked"))
	bool CanBeBuffered = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition = "CanBeBlocked && CanBeBuffered"))
	int32 BufferPriority = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition = "CanBeBlocked && CanBeBuffered"))
	bool CanTriggerBufferedInput = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FGameplayTagRequirements TriggerRequirements;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition = "CanBeBlocked && CanBeBuffered"))
	bool BufferStartedEvent = false;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(EditCondition = "CanBeBlocked && CanBeBuffered"))
	bool BufferTriggeredEvent = true;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories = "GameplayEvent.InputAction"))
	FGameplayTag StartedEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories = "GameplayEvent.InputAction"))
	FGameplayTag TriggeredEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories = "GameplayEvent.InputAction"))
	FGameplayTag OngoingEventTag;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ForceInlineRow, Categories = "GameplayEvent.InputAction"))
	FGameplayTag CompletedEventTag;
	
};

UENUM(BlueprintType)
enum class ENipcatInputBlockState : uint8
{
	Unlocked,
	Blocked
};

USTRUCT(BlueprintType)
struct NIPCATINPUT_API FNipcatInputMappingContextSetting
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 Priority = 0;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FModifyContextOptions Options;
	
};


USTRUCT(BlueprintType)
struct NIPCATINPUT_API FNipcatInputBufferStack
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UInputAction*> InputActionsForBuffer;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<UInputAction*> BufferedInputActions;	
};


