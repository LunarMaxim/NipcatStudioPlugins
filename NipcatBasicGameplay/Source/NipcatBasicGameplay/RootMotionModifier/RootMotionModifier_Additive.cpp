// Copyright Nipcat Studio All Rights Reserved.


#include "RootMotionModifier_Additive.h"

#include "MotionWarpingComponent.h"
#include "GameFramework/Character.h"

URootMotionModifier_Additive::URootMotionModifier_Additive(const FObjectInitializer& ObjectInitializer) :
	URootMotionModifier(ObjectInitializer)
{
}

FTransform URootMotionModifier_Additive::ProcessRootMotion(const FTransform& InRootMotion, const float DeltaSeconds)
{
	float LastAlpha = FMath::Clamp((PreviousPosition - ActualStartTime) / (EndTime - ActualStartTime), 0.f, 1.f);
	LastAlpha = FAlphaBlend::AlphaToBlendOption(LastAlpha, AddTranslationEasingFunc, AddTranslationEasingCurve);
	float Alpha = FMath::Clamp((CurrentPosition - ActualStartTime) / (EndTime - ActualStartTime), 0.f, 1.f);
	Alpha = FAlphaBlend::AlphaToBlendOption(Alpha, AddTranslationEasingFunc, AddTranslationEasingCurve);

	return FTransform(InRootMotion.GetRotation(), InRootMotion.GetLocation() + AdditiveVector * (Alpha - LastAlpha),
					  InRootMotion.GetScale3D());
}
