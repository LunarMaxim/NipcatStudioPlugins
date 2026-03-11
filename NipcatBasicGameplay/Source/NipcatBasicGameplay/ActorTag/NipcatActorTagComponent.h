// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

#include "NipcatActorTagComponent.generated.h"


UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent), Blueprintable, BlueprintType)
class NIPCATBASICGAMEPLAY_API UNipcatActorTagComponent : public UActorComponent
{
	GENERATED_BODY()
	
public:

	UNipcatActorTagComponent();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(ExposeOnSpawn, Categories="Actor"))
	FGameplayTagContainer ActorTags;

protected:
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;
};
