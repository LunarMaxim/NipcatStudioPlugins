// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CharacterRotationModifier.generated.h"

/**
 * 
 */
UCLASS(meta=(ShowWorldContextPin), EditInlineNew, Abstract, Blueprintable, BlueprintType, CollapseCategories)
class NIPCATBASICGAMEPLAY_API UCharacterRotationModifier : public UObject
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Priority = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabledDuringAnimRootMotion = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float RotationRateScale = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(EditCondition = "bCanEditYawOffset", EditConditionHides, HideEditConditionToggle))
	float YawOffset = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta=(InlineEditConditionToggle))
	bool bCanEditYawOffset = true;
	
	UFUNCTION(BlueprintNativeEvent)
	void Initialize(ACharacter* Character);
	
	UFUNCTION()
	FRotator GetDesiredRotation(ACharacter* Character, FRotator CurrentRotation, float DeltaTime);

	UFUNCTION(BlueprintNativeEvent, DisplayName="GetDesiredRotation")
	FRotator BlueprintGetDesiredRotation(ACharacter* Character, FRotator CurrentRotation, float DeltaTime);

	UFUNCTION(BlueprintNativeEvent)
	float GetRotationRateScale(ACharacter* Character, FRotator CurrentRotation, float DeltaTime) const;

	UFUNCTION(BlueprintNativeEvent)
	float GetYawOffset(ACharacter* Character, FRotator CurrentRotation, float DeltaTime) const;

	virtual UWorld* GetWorld() const override;
};

USTRUCT(BlueprintType)
struct NIPCATBASICGAMEPLAY_API FInstancedRotationModifier
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Instanced)
	TObjectPtr<UCharacterRotationModifier> RotationModifier;
};
