// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "NipcatCameraShakePreviewCameraManager.generated.h"

/**
 * 
 */
#if WITH_EDITOR
UCLASS()
class NIPCATABILITYSYSTEMEDITOR_API ANipcatCameraShakePreviewCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	void ResetPostProcessSettings()
	{
		ClearCachedPPBlends();
	}

	void MergePostProcessSettings(TArray<FPostProcessSettings>& InSettings, TArray<float>& InBlendWeights)
	{
		InSettings.Append(PostProcessBlendCache);
		InBlendWeights.Append(PostProcessBlendCacheWeights);
	}
};

class NIPCATABILITYSYSTEMEDITOR_API FNipcatCameraShakePreviewer : public FTickableEditorObject, public FGCObject
{
public:
	FNipcatCameraShakePreviewer();

	// FTickableObject Interface
	virtual ETickableTickType GetTickableTickType() const { return ETickableTickType::Always; }
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(FViewportCameraShakePreviewer, STATGROUP_Tickables); }
	virtual void Tick(float DeltaTime) override;

	// FGCObject interface
	virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual FString GetReferencerName() const override { return TEXT("SCameraShakePreviewer"); }

	UCameraShakeBase* AddCameraShake(const TSubclassOf<UCameraShakeBase> ShakeClass, const FAddCameraShakeParams& Params) const;
	void RemoveAllCameraShake(const TSubclassOf<UCameraShakeBase> OptionalShakeClass, const bool bImmediately = true) const;
	void ModifyCamera(FEditorViewportViewModifierParams& Params);

private:

	TObjectPtr<ANipcatCameraShakePreviewCameraManager> PreviewCamera;
	UCameraModifier_CameraShake* PreviewCameraShake;

	/** Hidden camera actor and active camera anims for Matinee shakes specifically */
	TWeakObjectPtr<ACameraActor> TempCameraActor;

	TOptional<float> LastDeltaTime;

	FVector LastLocationModifier;
	FRotator LastRotationModifier;
	float LastFOVModifier;

	TArray<FPostProcessSettings> LastPostProcessSettings;
	TArray<float> LastPostProcessBlendWeights;
};
#endif