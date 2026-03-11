// Copyright Nipcat Studio All Rights Reserved.


#include "NipcatAnimMetaData.h"

#if WITH_EDITOR
void UNipcatAnimMetaData::OnAssetEditorOpened(UObject* Asset)
{
	BlueprintOnAssetEditorOpened(Asset);
}

void UNipcatAnimMetaData::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	BlueprintPostEditChangeProperty();
}
#endif
