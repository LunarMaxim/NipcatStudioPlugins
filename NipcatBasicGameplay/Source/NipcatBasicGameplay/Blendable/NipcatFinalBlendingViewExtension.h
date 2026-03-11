// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "SceneViewExtension.h"


class UNipcatFinalBlendingComponent;

class FNipcatFinalBlendingViewExtension : public ISceneViewExtension, public TSharedFromThis<FNipcatFinalBlendingViewExtension, ESPMode::ThreadSafe>
{
public:
	TSoftObjectPtr<UNipcatFinalBlendingComponent> FinalBlendingComponent;

	virtual void SetupViewFamily(FSceneViewFamily& InViewFamily) override;
	virtual void SetupView(FSceneViewFamily& InViewFamily, FSceneView& InView) override;
	virtual void BeginRenderViewFamily(FSceneViewFamily& InViewFamily) override;

	virtual ~FNipcatFinalBlendingViewExtension() = default;
};
