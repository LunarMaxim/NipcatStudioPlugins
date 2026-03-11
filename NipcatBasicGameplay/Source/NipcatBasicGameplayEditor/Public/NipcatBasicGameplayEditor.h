#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FNipcatBasicGameplayEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
	
    void RegisterMenus();
	void OnToolButtonClicked();
	
    void OnPostEngineInit();

    FDelegateHandle OnAssetEditorOpenedHandle;
    void OnAssetEditorOpened(UObject* Asset);
    
    void OnActiveTabChanged(TSharedPtr<SDockTab> InPreviouslyActive, TSharedPtr<SDockTab> InNewlyActivated);
};
