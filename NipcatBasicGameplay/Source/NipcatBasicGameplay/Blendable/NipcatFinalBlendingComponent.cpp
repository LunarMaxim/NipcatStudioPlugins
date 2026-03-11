// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatFinalBlendingComponent.h"

#include "NipcatFinalBlendingViewExtension.h"


UNipcatFinalBlendingComponent::UNipcatFinalBlendingComponent()
{
	bAutoActivate = true;
}

void UNipcatFinalBlendingComponent::OnRegister()
{
	Super::OnRegister();
	if (bAutoActivate)
	{
		GetOwner()->GetWorld()->OnBeginPostProcessSettings.AddUObject(this, &UNipcatFinalBlendingComponent::OnBeginPostProcessSettings);
		if (!FinalBlendingViewExt.IsValid())
		{
			FinalBlendingViewExt = MakeShared<FNipcatFinalBlendingViewExtension>();
			FinalBlendingViewExt->FinalBlendingComponent = this;
		}
	}
}

void UNipcatFinalBlendingComponent::OnUnregister()
{
	GetOwner()->GetWorld()->OnBeginPostProcessSettings.RemoveAll(this);
	Super::OnUnregister();
}

void UNipcatFinalBlendingComponent::OnBeginPostProcessSettings(FVector ViewLocation, FSceneView* SceneView)
{
	FSceneViewFamily* ViewFamily = const_cast<FSceneViewFamily*>(SceneView->Family);
	if (ViewFamily && !ViewFamily->ViewExtensions.Contains(FinalBlendingViewExt))
	{
		ViewFamily->ViewExtensions.Add(FinalBlendingViewExt.ToSharedRef());
	}
	K2_OnBeginPostProcessSettings(ViewLocation);
}
