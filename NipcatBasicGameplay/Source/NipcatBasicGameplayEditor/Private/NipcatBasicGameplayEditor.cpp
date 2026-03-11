#include "NipcatBasicGameplayEditor.h"

#include "EditorUtilitySubsystem.h"
#include "NipcatBasicGameplay/AnimMetaData/NipcatAnimMetaData.h"
#include "ToolMenus.h"
#include "PropertyEditorModule.h"
#include "EditorUtilityWidgetBlueprint.h"
#include "NipcatBasicGameplayEditor/Settings/NipcatBasicGameplayEditorDeveloperSettings.h"
#include "NipcatBasicGameplayEditor/Settings/NipcatBasicGameplayEditorPerUserSettings.h"

#define LOCTEXT_NAMESPACE "FNipcatBasicGameplayEditorModule"

void FNipcatBasicGameplayEditorModule::StartupModule()
{
	FCoreDelegates::OnPostEngineInit.AddRaw(this, &FNipcatBasicGameplayEditorModule::OnPostEngineInit);
    UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FNipcatBasicGameplayEditorModule::RegisterMenus));
}

void FNipcatBasicGameplayEditorModule::ShutdownModule()
{
	if (GEditor)
	{
		if (const auto PerUserSettings = UNipcatBasicGameplayEditorPerUserSettings::Get())
		{
			PerUserSettings->SaveConfig();
		}
		UToolMenus::UnRegisterStartupCallback(this);
		UToolMenus::UnregisterOwner(this);
		if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			AssetEditorSubsystem->OnAssetEditorOpened().Remove(OnAssetEditorOpenedHandle);
		}
	}
}

void FNipcatBasicGameplayEditorModule::RegisterMenus()
{
	FToolMenuOwnerScoped OwnerScoped(this);
	
	UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar.PlayToolBar");
        
	if (!ToolbarMenu) return;

	FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("NipcatBasicGameplay");
	
	Section.AddEntry(FToolMenuEntry::InitToolBarButton(
		"NipcatEditorDebugTool",
		FUIAction(
			FExecuteAction::CreateRaw(this, &FNipcatBasicGameplayEditorModule::OnToolButtonClicked)
		),
		FText::FromString("Nipcat Editor Debug Tool"),
		FText::FromString("Open Nipcat Editor Debug Tool"),
		FSlateIcon(FAppStyle::GetAppStyleSetName(), "EditorPreferences.TabIcon")
	));
	
}

void FNipcatBasicGameplayEditorModule::OnToolButtonClicked()
{
	const auto Settings = UNipcatBasicGameplayEditorDeveloperSettings::Get();
	if (Settings->EditorDebugWidget.IsNull())
	{
		UE_LOG(LogTemp, Warning, TEXT("No Editor Debug Utility Widget configured in Project Settings -> Nipcat Basic Gameplay Editor!"));
		return;
	}

	UEditorUtilityWidgetBlueprint* WidgetBP = Settings->EditorDebugWidget.LoadSynchronous();
        
	if (WidgetBP)
	{
		if (UEditorUtilitySubsystem* EditorUtilitySubsystem = GEditor->GetEditorSubsystem<UEditorUtilitySubsystem>())
		{
			EditorUtilitySubsystem->SpawnAndRegisterTab(WidgetBP);
		}
	}
}

void FNipcatBasicGameplayEditorModule::OnPostEngineInit()
{
	if (GEditor)
	{
		//FGlobalTabmanager::Get()->OnActiveTabChanged_Subscribe(
		//	FOnActiveTabChanged::FDelegate::CreateRaw(this, &FNipcatBasicGameplayEditorModule::OnActiveTabChanged));
		if (UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>())
		{
			OnAssetEditorOpenedHandle = AssetEditorSubsystem->OnAssetEditorOpened().AddRaw(this, &FNipcatBasicGameplayEditorModule::OnAssetEditorOpened);
		}
	}
}

void FNipcatBasicGameplayEditorModule::OnAssetEditorOpened(UObject* Asset)
{
	if (const auto AnimSequenceBase = Cast<UAnimSequenceBase>(Asset))
	{
		for (auto& MetaData : AnimSequenceBase->GetMetaData())
		{
			if (MetaData)
			{
				if (const auto& NipcatAnimMetaData = Cast<UNipcatAnimMetaData>(MetaData))
				{
					NipcatAnimMetaData->OnAssetEditorOpened(Asset);
				}
			}
		}
	}
}

void FNipcatBasicGameplayEditorModule::OnActiveTabChanged(TSharedPtr<SDockTab> InPreviouslyActive,
	TSharedPtr<SDockTab> InNewlyActivated)
{
	/*
	if (!InNewlyActivated.IsValid()) return;

	// 检查标签页标题
	FString TabLabel = InNewlyActivated->GetTabLabel().ToString();
	if (TabLabel.Contains(TEXT("Montage")) || 
		TabLabel.Contains(TEXT("AnimMontage")) ||
		TabLabel.Contains(TEXT("Animation")))
	{
		// 进一步检查内容
		TSharedPtr<SWidget> Content = InNewlyActivated->GetContent();
		if (Content.IsValid())
		{
			
		}
	}*/
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FNipcatBasicGameplayEditorModule, NipcatBasicGameplayEditor)