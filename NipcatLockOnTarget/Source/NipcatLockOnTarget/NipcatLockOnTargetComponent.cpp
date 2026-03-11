// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatLockOnTargetComponent.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "LockOnTargetExtensions/WidgetExtension.h"
#include "TargetHandlers/TargetHandlerBase.h"

namespace NipcatLockOnTaragetTags
{
	UE_DEFINE_GAMEPLAY_TAG(TAG_State_SoftLock, "State.SoftLock")
}

void UNipcatLockOnTargetComponent::EnableTargeting(const FFindTargetRequestParams& RequestParams)
{
	if (IsSoftLockEnabled())
	{
		if (CanCaptureTarget() && !IsInputDelayActive())
		{
			//Prevents reliable buffer overflow.
			ActivateInputDelay();
			
			RequestFindTarget(RequestParams);
		}
		return;
	}
	Super::EnableTargeting(RequestParams);
}

void UNipcatLockOnTargetComponent::RequestFindTarget(const FFindTargetRequestParams& RequestParams)
{
	if (IsSoftLockEnabled())
	{
		if (!RequestParams.PlayerInput.IsZero())
		{
			// Clear SoftLock
			ClearTargetManual();
		}
		else
		{
			// Switch to HardLock
			SetSoftLockEnabled(false);
		}
		return;
	}
	Super::RequestFindTarget(RequestParams);
}

void UNipcatLockOnTargetComponent::NotifyTargetReleased(const FTargetInfo& Target)
{
	SetSoftLockEnabled(false);
	Super::NotifyTargetReleased(Target);
}

void UNipcatLockOnTargetComponent::CheckTargetState(float DeltaTime)
{
	if (!IsTargetLocked())
	{
		return;
	}
	Super::CheckTargetState(DeltaTime);
}

void UNipcatLockOnTargetComponent::ReceiveTargetException(ETargetExceptionType Exception)
{
	TargetExceptionWhenSoftLock = IsSoftLockEnabled();
	Super::ReceiveTargetException(Exception);
	TargetExceptionWhenSoftLock = false;
}

void UNipcatLockOnTargetComponent::SetSoftLockEnabled(bool Enabled)
{
	if (const auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		if (Enabled)
		{
			ASC->AddLooseGameplayTag(NipcatLockOnTaragetTags::TAG_State_SoftLock);
		}
		else
		{
			ASC->RemoveLooseGameplayTag(NipcatLockOnTaragetTags::TAG_State_SoftLock);
		}
	}
	if (const auto Extenstion = FindExtensionByClass(UWidgetExtension::StaticClass()))
	{
		const auto WidgetExtension = Cast<UWidgetExtension>(Extenstion);
		WidgetExtension->SetWidgetVisibility(!Enabled);
	}
}

bool UNipcatLockOnTargetComponent::IsSoftLockEnabled() const
{
	if (const auto ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		return ASC->HasMatchingGameplayTag(NipcatLockOnTaragetTags::TAG_State_SoftLock);
	}
	return false;
}
