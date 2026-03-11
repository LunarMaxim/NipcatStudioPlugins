// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "NipcatInput/Interface/NipcatInputComponentOwnerInterface.h"

#include "NipcatPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class NIPCATBASICGAMEPLAY_API ANipcatPlayerController : public APlayerController, public IGenericTeamAgentInterface, public INipcatInputComponentOwnerInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	TObjectPtr<class UInputComponent>  BlueprintVisibleInputComponent;

	virtual void SetupInputComponent() override;
	
	virtual FGenericTeamId GetGenericTeamId() const override;
	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Team")
	FGenericTeamId MyTeamID = FGenericTeamId(0);
	
	//友军
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Team")
	TArray<FGenericTeamId> FriendlyTeamID;
	
	//敌人
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category="Team")
	TArray<FGenericTeamId> HostileTeamID;
	
	virtual bool ShouldFlushKeysWhenViewportFocusChanges() const override;

	virtual UNipcatInputComponent* GetNipcatInputComponent() const override;

	UFUNCTION(BlueprintNativeEvent)
	void OnSetPawn(APawn* PrevPawn, APawn* NewPawn);

	virtual void SetPawn(APawn* InPawn) override;
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSetPawn, APawn*, OldPawn, APawn*, NewPawn);
	UPROPERTY(BlueprintAssignable)
	FOnSetPawn OnSetPawnDelegate;
};
