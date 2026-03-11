// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FlowComponent.h"
#include "NipcatFlowOwnerInterface.h"
#include "NipcatFlowTypes.h"

#include "NipcatFlowComponent.generated.h"


UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent))
class NIPCATFLOW_API UNipcatFlowComponent : public UFlowComponent, public INipcatFlowOwnerInterface
{
	GENERATED_BODY()

public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, SaveGame, NonPIEDuplicateTransient, TextExportTransient, NonTransactional)
	FGuid Guid;
	
	UPROPERTY(VisibleAnywhere, Transient)
	FNipcatFlowIdentityInfo CurrentInfo;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void ApplyFlowIdentityInfo(FNipcatFlowIdentityInfo& NewInfo);
	
	UFUNCTION(BlueprintCallable)
	void RefreshFlowIdentityInfo();
	
	virtual void PostLoad() override;
	
};
