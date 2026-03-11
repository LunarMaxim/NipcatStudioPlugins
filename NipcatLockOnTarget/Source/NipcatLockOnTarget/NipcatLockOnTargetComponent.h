// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LockOnTargetComponent.h"
#include "NativeGameplayTags.h"

#include "NipcatLockOnTargetComponent.generated.h"

namespace NipcatLockOnTaragetTags
{
	NIPCATLOCKONTARGET_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_State_SoftLock)
}

UCLASS(ClassGroup=(Nipcat), meta=(BlueprintSpawnableComponent))
class NIPCATLOCKONTARGET_API UNipcatLockOnTargetComponent : public ULockOnTargetComponent
{
	GENERATED_BODY()

public:
	virtual void EnableTargeting(const FFindTargetRequestParams& RequestParams) override;
	virtual void RequestFindTarget(const FFindTargetRequestParams& RequestParams) override;
	virtual void NotifyTargetReleased(const FTargetInfo& Target) override;
	virtual void CheckTargetState(float DeltaTime) override;
	virtual void ReceiveTargetException(ETargetExceptionType Exception) override;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	bool TargetExceptionWhenSoftLock;
	
	UFUNCTION(BlueprintCallable)
	virtual void SetSoftLockEnabled(bool Enabled = true);
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsSoftLockEnabled() const;
};
