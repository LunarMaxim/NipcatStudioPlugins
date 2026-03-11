// Copyright Nipcat Studio All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimMetaData.h"
#include "NipcatAnimMetaData.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class NIPCATBASICGAMEPLAY_API UNipcatAnimMetaData : public UAnimMetaData
{
	GENERATED_BODY()

public:
	
#if WITH_EDITOR
	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintPostEditChangeProperty();
	
	UFUNCTION()
	virtual void OnAssetEditorOpened(UObject* Asset);

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintOnAssetEditorOpened(UObject* Asset);

	void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
};
