// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatInteractCondition_Angle.h"

#include "MathUtil.h"
#include "Kismet/GameplayStatics.h"
#include "NipcatBasicGameplay/Interaction/NipcatInteractableComponent.h"

bool UNipcatInteractCondition_Angle::CanInteract_Implementation(UNipcatInteractComponent* InteractComponent,
                                                                UNipcatInteractableComponent* Interactable, FText& OutFailedReason)
{
	if (IsValid(InteractComponent))
	{
		const FRotator BaseRotation = UseCameraRotation
			                              ? UGameplayStatics::GetPlayerCameraManager(InteractComponent, 0)->GetCameraRotation()
			                              : InteractComponent->GetOwner()->GetActorRotation();
		const auto TargetRotation = (Interactable->GetInteractPointLocation() - InteractComponent->GetOwner()->GetActorLocation()).GetSafeNormal2D().Rotation();
		auto DeltaRotation = (TargetRotation	- BaseRotation);
		DeltaRotation.Normalize();
		auto DeltaRotationInLocalSpace = (DeltaRotation - FRotator(0.f, Direction, 0.f));
		DeltaRotationInLocalSpace.Normalize();
		const auto DirectionAngle = DeltaRotationInLocalSpace.Yaw;
		if (const bool Succeed = FMathf::Abs(DirectionAngle) < HalfAngle)
		{
			return true;
		}
		OutFailedReason = GetFailedReason(Interactable);
	}
	return false;
}
