// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "NipcatGeometryCollectionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class NIPCATBASICGAMEPLAY_API UNipcatGeometryCollectionComponent : public UGeometryCollectionComponent
{
	GENERATED_BODY()
	
	virtual void SetCollisionResponseToChannel(ECollisionChannel Channel, ECollisionResponse NewResponse) override;

	virtual void SetCollisionProfileName(FName InCollisionProfileName, bool bUpdateOverlaps) override;

	virtual void SetCollisionResponseToChannels(const FCollisionResponseContainer& NewReponses) override;
	
	virtual void SetCollisionResponseToAllChannels(ECollisionResponse NewResponse) override;

	virtual void SetCollisionEnabled(ECollisionEnabled::Type NewType) override;

};
