// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AnimationModifier.h"
#include "FeetCrossingAnimCurveModifier.generated.h"



UCLASS()
class NIPCATLOCOMOTIONSYSTEMEDITOR_API UFeetCrossingAnimCurveModifier : public UAnimationModifier
{
	GENERATED_BODY()

	/** Rate used to sample the animation */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (Units="Hz", UIMin=1, AllowPrivateAccess=true))
	int SampleRate = 120;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess=true))
	FName LeftBallBoneName = TEXT("ball_l");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess=true))
	FName RightBallBoneName = TEXT("ball_r");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess=true))
	FName SpineBoneName = TEXT("spine_01");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess=true))
	FName RootBoneName = TEXT("root");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess=true))
	TEnumAsByte<EAxis::Type> SpineBoneForwardAxis = EAxis::Y;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Settings", meta = (AllowPrivateAccess=true))
	FName CurveName = TEXT("Feet_Crossing");

public:
	virtual void OnApply_Implementation(UAnimSequence* InAnimation) override;
	virtual void OnRevert_Implementation(UAnimSequence* AnimationSequence) override;

};
