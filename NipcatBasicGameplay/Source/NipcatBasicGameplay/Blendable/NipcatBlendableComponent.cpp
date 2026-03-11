// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatBlendableComponent.h"


void UNipcatBlendableComponent::OverrideBlendableSettings(FSceneView& View, float Weight) const
{
	K2_OverrideBlendableSettings(View.ViewLocation, Weight);
}
