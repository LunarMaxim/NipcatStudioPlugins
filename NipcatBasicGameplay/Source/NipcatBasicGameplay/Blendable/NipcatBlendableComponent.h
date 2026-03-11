// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "NipcatBlendableComponent.generated.h"


UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent), Blueprintable, Abstract)
class NIPCATBASICGAMEPLAY_API UNipcatBlendableComponent : public UActorComponent, public IBlendableInterface
{
	GENERATED_BODY()

public:
	virtual void OverrideBlendableSettings(FSceneView& View, float Weight) const override;

	UFUNCTION(BlueprintImplementableEvent, DisplayName = "OverrideBlendableSettings")
	void K2_OverrideBlendableSettings(FVector SceneViewLocation, float Weight) const;
};
