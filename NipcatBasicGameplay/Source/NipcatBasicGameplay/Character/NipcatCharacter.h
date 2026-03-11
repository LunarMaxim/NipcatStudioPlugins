// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "NipcatInput/Interface/NipcatInputComponentOwnerInterface.h"
#include "NipcatCharacter.generated.h"

UCLASS()
class NIPCATBASICGAMEPLAY_API ANipcatCharacter : public ACharacter, public IGenericTeamAgentInterface, public INipcatInputComponentOwnerInterface
{
	GENERATED_UCLASS_BODY()

public:
	ANipcatCharacter();

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Setter, Interp, Category = "Collision")
	bool DisableCollision;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Setter, Interp, Category = "Character Movement")
	bool DisableMovement;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Rotation")
	float FaceRotationInterpSpeed = 8.f;

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Nipcat Character")
	void SetDisableCollision(bool Value);

	UFUNCTION(BlueprintCallable, CallInEditor, Category = "Nipcat Character")
	void SetDisableMovement(bool Value);

	virtual UNipcatInputComponent* GetNipcatInputComponent() const override;

	virtual void FaceRotation(FRotator NewControlRotation, float DeltaTime) override;
	
	virtual bool CanJumpInternal_Implementation() const override;
	
	/** Begin IGenericTeamAgent interface */
	virtual void SetGenericTeamId(const FGenericTeamId& TeamID) override;
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;
	/** End IGenericTeamAgent interface */
};
