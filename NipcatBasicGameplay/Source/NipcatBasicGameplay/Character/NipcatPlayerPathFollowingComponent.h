// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Navigation/PathFollowingComponent.h"
#include "NipcatPlayerPathFollowingComponent.generated.h"


UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent))
class NIPCATBASICGAMEPLAY_API UNipcatPlayerPathFollowingComponent : public UPathFollowingComponent
{
	GENERATED_BODY()

public:
	virtual void UpdateDecelerationData() override;
	virtual void FollowPathSegment(float DeltaTime) override;
	
	virtual float PredictCurrentBrakingDistance();
	
	// If true, Pawn will stop on the edge of Acceptance Radius exactly. Not recommended for moving Target.
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool StartDecelerationBeforeReachingAcceptanceRadius;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (UIMin = -1, UIMax = 180, ClampMin = -1, ClampMax = 180))
	float MovementInputLimitAngle = -1.f;
};
