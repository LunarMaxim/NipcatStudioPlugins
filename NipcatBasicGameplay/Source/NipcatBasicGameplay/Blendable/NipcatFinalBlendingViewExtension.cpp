// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatFinalBlendingViewExtension.h"

#include "NipcatFinalBlendingComponent.h"


void FNipcatFinalBlendingViewExtension::SetupViewFamily(FSceneViewFamily& InViewFamily)
{
}

void FNipcatFinalBlendingViewExtension::SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView)
{
	if (FinalBlendingComponent)
	{
		FinalBlendingComponent->K2_SetUpVeiw(InView.ViewLocation);
	}
}

void FNipcatFinalBlendingViewExtension::BeginRenderViewFamily(FSceneViewFamily& InViewFamily)
{
}

