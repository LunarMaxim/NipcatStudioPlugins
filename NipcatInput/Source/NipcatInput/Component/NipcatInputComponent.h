// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "NipcatInput/Types/NipcatInputTypes.h"
#include "Components/ActorComponent.h"
#include "GameFramework/InputSettings.h"
#include "NipcatInputComponent.generated.h"


class UInputAction;
class UEnhancedInputLocalPlayerSubsystem;
class UInputMappingContext;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct NIPCATINPUT_API FNipcatBlockInputObjectCountMap
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ForceInlineRow))
	TMap<UObject*, int32> Map;
};

UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent), BlueprintType, Blueprintable)
class NIPCATINPUT_API UNipcatInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	UNipcatInputComponent();
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ForceInlineRow), Category="- Runtime -")
	TMap<UInputAction*, FNipcatInputActionEvent> InputActionEventMapping;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ForceInlineRow), Category="- Runtime -")
	TMap<UInputAction*, int32> InputBlockStack;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ForceInlineRow), Category="- Runtime -")
	TMap<UInputAction*, FNipcatBlockInputObjectCountMap> BlockInputObjects;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(ForceInlineRow), Category="- Runtime -")
	TMap<FName, FNipcatInputBufferStack> InputBufferStacks;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category="- Runtime -")
	TArray<const UInputAction*> OngoingInputs;

	UFUNCTION(BlueprintCallable)
	void BlockInput(UInputAction* InputAction, int32 Count = 1, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable)
	void BlockInputs(const TArray<UInputAction*>& InputActions, int32 Count = 1, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="WithoutInputActions"))
	void BlockAllInputs(const TArray<UInputAction*>& WithoutInputActions, int32 BlockLevel = 0, int32 Count = 1, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable)
	void UnlockInput(UInputAction* InputAction, int32 Count = 1, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable)
	void UnlockInputs(const TArray<UInputAction*>& InputActions, int32 Count = 1, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="WithoutInputActions"))
	void UnlockAllInputs(const TArray<UInputAction*>& WithoutInputActions, int32 BlockLevel = 0, int32 Count = 1, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable)
	void StartBuffer(const TArray<UInputAction*>& InputActions, const TArray<UInputAction*>& InitialBufferedInputActions, FName ID);

	UFUNCTION(BlueprintCallable, meta=(AutoCreateRefTerm="WithoutInputActions,InitialBufferedInputActions"))
	void StartBufferForAllInputs(FName ID, const TArray<UInputAction*>& WithOutInputActions, const TArray<UInputAction*>& InitialBufferedInputActions, int32 BlockLevel = 0);

	UFUNCTION(BlueprintPure, meta=(AutoCreateRefTerm="WithoutInputActions"))
	TArray<UInputAction*> GetAllInputActionsUnderBlockLevel(const TArray<UInputAction*>& WithoutInputActions, int32 BlockLevel = 0) const;
	
	UFUNCTION(BlueprintPure)
	TArray<UInputAction*> FilterInputActionsForBuffer(const TArray<UInputAction*>& InputActions) const;
	
	UFUNCTION(BlueprintCallable)
	void EndBuffer(FName ID, bool TryTrigger = true, bool ShouldUnlockInputs = true, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable)
	bool CheckInputCompleted(UInputAction* InputAction, bool TryTrigger = true, bool ShouldUnlockInputs = true, UObject* SourceObject = nullptr);

	UFUNCTION(BlueprintCallable)
	void AddInputMappingContext(TMap<TSoftObjectPtr<UInputMappingContext>, FNipcatInputMappingContextSetting> InputMappingContexts);

	UFUNCTION(BlueprintCallable)
	void RemoveInputMappingContext(TMap<TSoftObjectPtr<UInputMappingContext>, FNipcatInputMappingContextSetting> InputMappingContexts);

	UFUNCTION()
	virtual void OnInputHardwareDeviceChanged(const FPlatformUserId UserId, const FInputDeviceId DeviceId);

	UFUNCTION(BlueprintPure)
	static EHardwareDevicePrimaryType GetHardwareDeviceType(const FInputDeviceId DeviceId = FInputDeviceId());
	
protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	UAbilitySystemComponent* GetAbilitySystem() const;

	UPROPERTY(BlueprintReadOnly)
	APlayerController* PlayerController;

	UPROPERTY()
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem;

	void OnInputStarted(const UInputAction* InputAction);
	void OnInputTriggered(const UInputAction* InputAction);
	void OnInputOngoing(const UInputAction* InputAction);
	void OnInputCompleted(const UInputAction* InputAction);
	void OnInputCanceled(const UInputAction* InputAction);

	void SendInputTriggerEvent(const UInputAction* InputAction, ETriggerEvent TriggerEvent);
};
