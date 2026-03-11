// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "NipcatAbilitySystem/NipcatAbilitySystemComponent.h"
#include "NipcatBasicGameplay/Character/NipcatCharacter.h"
#include "NipcatAbilitySystemCharacter.generated.h"

UCLASS()
class NIPCATABILITYSYSTEM_API ANipcatAbilitySystemCharacter : public ANipcatCharacter, public IAbilitySystemInterface
{
	GENERATED_UCLASS_BODY()

public:
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
	// Sets default values for this character's properties
	ANipcatAbilitySystemCharacter();

	UFUNCTION(BlueprintCallable, Category = "NipcatAbilitySystem")
	virtual UNipcatAbilitySystemComponent* GetAbilitySystemComponent() const override;

private:
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UNipcatAbilitySystemComponent> AbilitySystemComponent;
};
