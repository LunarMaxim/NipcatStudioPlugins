// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatIndicatorComponent.h"

#include "NipcatBasicGameplay/Settings/NipcatBasicGameplayDeveloperSettings.h"
#include "NativeGameplayTags.h"

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Indicator_Interactable, "Indicator.Interactable")

UNipcatIndicatorComponent::UNipcatIndicatorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	IndicatorType = TAG_Indicator_Interactable;
}

void UNipcatIndicatorComponent::ApplyIndicatorConfig(UIndicatorDescriptor* Indicator)
{
	Indicator->SetSceneComponent(this);
	const FNipcatIndicatorConfig* IndicatorConfig = UseCustomIndicatorConfig ? &CustomIndicatorConfig : UNipcatBasicGameplayDeveloperSettings::Get()->IndicatorConfig.Find(IndicatorType);
	if (!IndicatorConfig)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, FString::Printf(TEXT("Actor %s Missing Indicator Config!"), *GetOwner()->GetName()));
		return;
	}
	Indicator->SetIndicatorClass(IndicatorConfig->IndicatorWidgetClass);
	Indicator->SetAutoRemoveWhenIndicatorComponentIsNull(IndicatorConfig->bAutoRemoveWhenIndicatorComponentIsNull);
	Indicator->SetProjectionMode(IndicatorConfig->ProjectionMode);
	Indicator->SetHAlign(IndicatorConfig->HAlignment);
	Indicator->SetVAlign(IndicatorConfig->VAlignment);
	Indicator->SetClampToScreen(IndicatorConfig->bClampToScreen);
	Indicator->SetShowClampToScreenArrow(IndicatorConfig->bShowClampToScreenArrow);
	Indicator->SetWorldPositionOffset(IndicatorConfig->WorldPositionOffset);
	Indicator->SetScreenSpaceOffset(IndicatorConfig->ScreenSpaceOffset);
	Indicator->SetBoundingBoxAnchor(IndicatorConfig->BoundingBoxAnchor);
	Indicator->SetPriority(IndicatorConfig->Priority);
}