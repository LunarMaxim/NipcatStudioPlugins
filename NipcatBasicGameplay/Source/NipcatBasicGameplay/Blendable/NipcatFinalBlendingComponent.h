// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatFinalBlendingComponent.generated.h"


class FNipcatFinalBlendingViewExtension;

UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent), Blueprintable, Abstract)
class NIPCATBASICGAMEPLAY_API UNipcatFinalBlendingComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNipcatFinalBlendingComponent();
	
	TSharedPtr<FNipcatFinalBlendingViewExtension, ESPMode::ThreadSafe> FinalBlendingViewExt;
	
	virtual void OnRegister() override;
	virtual void OnUnregister() override;
	
	void OnBeginPostProcessSettings(FVector ViewLocation, FSceneView* SceneView);
	
	UFUNCTION(BlueprintImplementableEvent, CallInEditor, DisplayName = "OnBeginPostProcessSettings", meta=(CallableWithoutWorldContext))
	void K2_OnBeginPostProcessSettings(FVector ViewLocation);

	UFUNCTION(BlueprintImplementableEvent, CallInEditor, DisplayName = "SetUpVeiw", meta=(CallableWithoutWorldContext))
	void K2_SetUpVeiw(FVector ViewLocation);
};
