// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "NipcatCustomMovementMode.generated.h"

class UNipcatCharacterMovementComponent;
/**
 * 
 */
UCLASS(EditInlineNew, Abstract, Blueprintable, BlueprintType, CollapseCategories)
class NIPCATBASICGAMEPLAY_API UNipcatCustomMovementMode : public UObject
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool IgnoreMoveInput = true;
	
	UFUNCTION()
	virtual void AddCustomMovementInput(UNipcatCharacterMovementComponent* CMC, FVector InputVector, float DeltaTime);
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName=AddCustomMovementInput)
	void BP_AddCustomMovementInput(UNipcatCharacterMovementComponent* CMC, FVector InputVector, float DeltaTime);

	UFUNCTION()
	virtual void PreEnter(UNipcatCharacterMovementComponent* CMC);

	UFUNCTION(BlueprintImplementableEvent, DisplayName=PreEnter)
	void BP_PreEnter(UNipcatCharacterMovementComponent* CMC);

	UFUNCTION()
	virtual void PostEnter(UNipcatCharacterMovementComponent* CMC);

	UFUNCTION(BlueprintImplementableEvent, DisplayName=PostEnter)
	void BP_PostEnter(UNipcatCharacterMovementComponent* CMC);

	UFUNCTION()
	virtual void OnUpdate(UNipcatCharacterMovementComponent* CMC, float DeltaTime);

	UFUNCTION(BlueprintImplementableEvent, DisplayName=OnUpdate)
	void BP_OnUpdate(UNipcatCharacterMovementComponent* CMC, float DeltaTime);

	UFUNCTION()
	virtual void PreExit(UNipcatCharacterMovementComponent* CMC);

	UFUNCTION(BlueprintImplementableEvent, DisplayName=PreExit)
	void BP_PreExit(UNipcatCharacterMovementComponent* CMC);

	UFUNCTION()
	virtual void PostExit(UNipcatCharacterMovementComponent* CMC);

	UFUNCTION(BlueprintImplementableEvent, DisplayName=PostExit)
	void BP_PostExit(UNipcatCharacterMovementComponent* CMC);

	virtual UWorld* GetWorld() const override;
	
};
