// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "RootMotionModifier.h"

#include "RootMotionModifier_Additive.generated.h"

/**
 * 
 */
UCLASS(meta = (DisplayName = "Additive"))
class NIPCATBASICGAMEPLAY_API URootMotionModifier_Additive : public URootMotionModifier
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "- Config- ", meta = (EditCondition = "!FromWarpTarget", EditConditionHides, ExposeOnSpawn))
	FVector AdditiveVector;
	
	/** Easing function used when adding translation. Only relevant when there is no translation in the animation */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "- Config -", meta = (ExposeOnSpawn))
	EAlphaBlendOption AddTranslationEasingFunc = EAlphaBlendOption::Linear;

	/** Custom curve used to add translation when there is none to warp. Only relevant when AddTranslationEasingFunc is set to Custom */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "- Config -", meta = (EditCondition = "AddTranslationEasingFunc == EAlphaBlendOption::Custom", EditConditionHides, ExposeOnSpawn))
	TObjectPtr<UCurveFloat> AddTranslationEasingCurve = nullptr;

public:
	URootMotionModifier_Additive(const FObjectInitializer& ObjectInitializer);

	virtual FTransform ProcessRootMotion(const FTransform& InRootMotion, float DeltaSeconds) override;
};
