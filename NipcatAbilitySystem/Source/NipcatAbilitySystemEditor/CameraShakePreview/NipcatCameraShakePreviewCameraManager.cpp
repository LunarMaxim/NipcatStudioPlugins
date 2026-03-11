// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatCameraShakePreviewCameraManager.h"

#include "Camera/CameraModifier_CameraShake.h"

#if WITH_EDITOR
namespace UE
{
namespace MovieScene
{
constexpr auto AnimationViewportMapName = "Transient";
	
UWorld* FindCameraShakePreviewerWorld()
{
	for (const FWorldContext& Context : GEngine->GetWorldContexts())
	{
		if (Context.WorldType == EWorldType::Editor)
		{
			return Context.World();
		}
	}
	return nullptr;
}

}
}

/**
 * Data struct for each entry in the panel's main list.
 */
struct FViewportCameraShakeData
	: public TSharedFromThis<FViewportCameraShakeData>
	, public FGCObject
{
	TSubclassOf<UCameraShakeBase> ShakeClass;
	TObjectPtr<UCameraShakeBase> ShakeInstance = nullptr;
	bool bIsPlaying = false;
	bool bIsHidden = false;

	TWeakObjectPtr<UCameraShakeSourceComponent> SourceComponent;

	virtual void AddReferencedObjects(FReferenceCollector& Collector) override
	{
		Collector.AddReferencedObject(ShakeInstance);
	}
	virtual FString GetReferencerName() const override
	{
		return TEXT("FViewportCameraShakeData");
	}
};

FNipcatCameraShakePreviewer::FNipcatCameraShakePreviewer()
	: PreviewCamera(NewObject<ANipcatCameraShakePreviewCameraManager>())
	, LastLocationModifier(FVector::ZeroVector)
	, LastRotationModifier(FRotator::ZeroRotator)
	, LastFOVModifier(0.f)
{
	PreviewCameraShake = CastChecked<UCameraModifier_CameraShake>(
			PreviewCamera->AddNewCameraModifier(UCameraModifier_CameraShake::StaticClass()));
}

void FNipcatCameraShakePreviewer::Tick(float DeltaTime)
{
	// Accumulate the deltas in case we get ticked several times before we are 
	// asked to modify a viewport.
	LastDeltaTime = LastDeltaTime.Get(0.f) + DeltaTime; 
}

void FNipcatCameraShakePreviewer::ModifyCamera(FEditorViewportViewModifierParams& Params)
{
	const float DeltaTime = LastDeltaTime.Get(-1.f);
	if (DeltaTime > 0.f)
	{
		LastPostProcessSettings.Reset();
		LastPostProcessBlendWeights.Reset();
		PreviewCamera->ResetPostProcessSettings();

		const FMinimalViewInfo OriginalPOV(Params.ViewInfo);

		PreviewCameraShake->ModifyCamera(DeltaTime, Params.ViewInfo);

		PreviewCamera->MergePostProcessSettings(LastPostProcessSettings, LastPostProcessBlendWeights);

		LastLocationModifier = Params.ViewInfo.Location - OriginalPOV.Location;
		LastRotationModifier = Params.ViewInfo.Rotation - OriginalPOV.Rotation;
		LastFOVModifier = Params.ViewInfo.FOV - OriginalPOV.FOV;

		LastDeltaTime.Reset();
	}
	else
	{                                      
		Params.ViewInfo.Location += LastLocationModifier;
		Params.ViewInfo.Rotation += LastRotationModifier;
		Params.ViewInfo.FOV += LastFOVModifier;
	}

	for (int32 PPIndex = 0; PPIndex < LastPostProcessSettings.Num(); ++PPIndex)
	{
		Params.AddPostProcessBlend(LastPostProcessSettings[PPIndex], LastPostProcessBlendWeights[PPIndex]);
	}

}

void FNipcatCameraShakePreviewer::AddReferencedObjects(FReferenceCollector& Collector)
{
	Collector.AddReferencedObject(PreviewCamera);
}

UCameraShakeBase* FNipcatCameraShakePreviewer::AddCameraShake(const TSubclassOf<UCameraShakeBase> ShakeClass, const FAddCameraShakeParams& Params) const
{
	const FAddCameraShakeParams ActualParams(Params);

	UCameraShakeBase* ShakeInstance = PreviewCameraShake->AddCameraShake(ShakeClass, ActualParams);
	UE_LOG(LogTemp, Warning, TEXT("The Shake Instance's name is %s"), *ShakeInstance->GetName());
	return ShakeInstance;
}

void FNipcatCameraShakePreviewer::RemoveAllCameraShake(const TSubclassOf<UCameraShakeBase> OptionalShakeClass,
	const bool bImmediately) const
{
	if (OptionalShakeClass)
	{
		PreviewCameraShake->RemoveAllCameraShakesOfClass(OptionalShakeClass, bImmediately);
	}
	else
	{
		PreviewCameraShake->RemoveAllCameraShakes(bImmediately);
	}
}

#endif
