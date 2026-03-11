// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RootMotionModifier.h"
#include "RootMotionModifier_InterpWithAlpha.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Interp With Alpha"))
class NIPCATBASICGAMEPLAY_API URootMotionModifier_InterpWithAlpha : public URootMotionModifier
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn))
	bool UseFixedTransform;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "!UseFixedTransform", EditConditionHides))
	FName WarpTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "!UseFixedTransform", EditConditionHides))
	bool UseWarpTargetAsAdditive;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "UseFixedTransform", EditConditionHides))
	FVector FixedTranslation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "UseFixedTransform", EditConditionHides))
	FVector FixedRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn))
	bool WarpTranslation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "WarpTranslation"))
	float TranslationInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "WarpTranslation"))
	bool UseFixedTranslationInterpAlpha = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "WarpTranslation && UseFixedTranslationInterpAlpha", UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1))
	float FixedTranslationInterpAlpha = 1.f;
	
	/** Easing function used when adding translation. Only relevant when there is no translation in the animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "WarpTranslation && !UseFixedTranslationInterpAlpha"))
	EAlphaBlendOption TranslationEasingFunc = EAlphaBlendOption::Linear;

	/** Custom curve used to add translation when there is none to warp. Only relevant when AddTranslationEasingFunc is set to Custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend", meta = (EditCondition = "TranslationEasingFunc==EAlphaBlendOption::Custom && WarpTranslation && !UseFixedTranslationInterpAlpha", EditConditionHides, ExposeOnSpawn))
	TObjectPtr<UCurveFloat> TranslationEasingCurve = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "WarpTranslation"))
	bool IgnoreZ;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn))
	bool WarpRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "WarpRotation"))
	float RotationInterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "WarpRotation"))
	bool UseFixedRotationInterpAlpha = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "WarpTranslation && UseFixedRotationInterpAlpha", UIMin = 0, UIMax = 1, ClampMin = 0, ClampMax = 1))
	float FixedRotationInterpAlpha = 1.f;
	
	/** Easing function used when adding translation. Only relevant when there is no translation in the animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Config", meta = (ExposeOnSpawn, EditCondition = "WarpRotation && !UseFixedRotationInterpAlpha"))
	EAlphaBlendOption RotationEasingFunc = EAlphaBlendOption::Linear;

	/** Custom curve used to add translation when there is none to warp. Only relevant when AddTranslationEasingFunc is set to Custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Blend", meta = (EditCondition = "RotationEasingFunc==EAlphaBlendOption::Custom && WarpRotation && !UseFixedRotationInterpAlpha", EditConditionHides, ExposeOnSpawn))
	TObjectPtr<UCurveFloat> RotationEasingCurve = nullptr;

	FTransform WarpTargetTransform;
	bool HasTarget;

public:
	URootMotionModifier_InterpWithAlpha(const FObjectInitializer& ObjectInitializer);

	virtual FTransform ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds) override;
	virtual void Update(const FMotionWarpingUpdateContext& Context) override;

	const FName& GetWarpTarget() const;
	bool IsWarpTranslation() const;
	bool IsWarpRotation() const;
};

FORCEINLINE const FName& URootMotionModifier_InterpWithAlpha::GetWarpTarget() const
{
	return WarpTarget;
}

FORCEINLINE bool URootMotionModifier_InterpWithAlpha::IsWarpTranslation() const
{
	return WarpTranslation;
}

FORCEINLINE bool URootMotionModifier_InterpWithAlpha::IsWarpRotation() const
{
	return WarpRotation;
}
