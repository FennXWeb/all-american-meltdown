#include "Modules/ModuleManager.h"

#include "AssetToolsModule.h"
#include "Editor.h"
#include "Factories/DataAssetFactory.h"
#include "Framework/Commands/UIAction.h"
#include "IAssetTools.h"
#include "LWAudioCatalog.h"
#include "Misc/MessageDialog.h"
#include "ScopedTransaction.h"
#include "Textures/SlateIcon.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "ToolMenus.h"
#include "UObject/Package.h"

#define LOCTEXT_NAMESPACE "LethalWorldAudioEditor"

class FLethalWorldEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override
    {
        if (!IsRunningCommandlet())
            UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FLethalWorldEditorModule::RegisterMenus));
    }

    virtual void ShutdownModule() override
    {
        UToolMenus::UnRegisterStartupCallback(this);
        UToolMenus::UnregisterOwner(this);
    }

private:
    void RegisterMenus()
    {
        FToolMenuOwnerScoped Owner(this);
        UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Tools");
        FToolMenuSection& Section = Menu->FindOrAddSection("LethalWorld");
        Section.AddMenuEntry("LethalWorldAudioManager", LOCTEXT("AudioManager", "Lethal World Audio Manager"),
            LOCTEXT("AudioManagerTip", "Open the audio catalog. Expand any slot and assign Source or multiple audio assets in Tracks; adjust volume, pitch, looping and attenuation, then Save."),
            FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &FLethalWorldEditorModule::OpenAudioManager)));
        Section.AddMenuEntry("LethalWorldValidateAudio", LOCTEXT("ValidateAudio", "Validate Lethal World Audio"),
            LOCTEXT("ValidateAudioTip", "Check catalog keys, source assets, ranges and loop configuration."),
            FSlateIcon(), FUIAction(FExecuteAction::CreateRaw(this, &FLethalWorldEditorModule::ValidateAudio)));
    }

    void OpenAudioManager()
    {
        if (!GEditor) return;
        ULWAudioCatalog* Catalog = ULWAudioCatalog::GetDefaultCatalog();
        if (!Catalog)
        {
            // AssetTools refuses a name collision; never overwrite another asset at this path.
            UDataAssetFactory* Factory = NewObject<UDataAssetFactory>();
            Factory->DataAssetClass = ULWAudioCatalog::StaticClass();
            FAssetToolsModule& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
            Catalog = Cast<ULWAudioCatalog>(AssetTools.Get().CreateAsset(TEXT("DA_AudioCatalog"), TEXT("/Game/Audio"), ULWAudioCatalog::StaticClass(), Factory));
        }
        if (!Catalog)
        {
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("CreateFailed", "Could not open or create /Game/Audio/DA_AudioCatalog. Check the Output Log for a name collision or asset creation error."));
            return;
        }
        {
            const FScopedTransaction Transaction(LOCTEXT("SeedAudio", "Add missing audio catalog slots"));
            if (Catalog->AddMissingDefaultSlots() > 0) Catalog->MarkPackageDirty();
        }
        GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->OpenEditorForAsset(Catalog);
    }

    void ValidateAudio()
    {
        ULWAudioCatalog* Catalog = ULWAudioCatalog::GetDefaultCatalog();
        if (!Catalog)
        {
            FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("MissingCatalog", "Open Lethal World Audio Manager to create the catalog, or run Tools/build_audio_v2.py after importing the sounds."));
            return;
        }
        TArray<FString> Errors, Warnings;
        Catalog->ValidateCatalog(Errors, Warnings);
        for (const FString& Error : Errors) UE_LOG(LogTemp, Error, TEXT("LW_AUDIO: %s"), *Error);
        for (const FString& Warning : Warnings) UE_LOG(LogTemp, Warning, TEXT("LW_AUDIO: %s"), *Warning);
        FString Message = FString::Printf(TEXT("Audio catalog: %d errors, %d warnings.\n"), Errors.Num(), Warnings.Num());
        int32 Shown = 0;
        for (const FString& Error : Errors) { if (Shown++ < 8) Message += TEXT("\nERROR: ") + Error; }
        for (const FString& Warning : Warnings) { if (Shown++ < 8) Message += TEXT("\nWARNING: ") + Warning; }
        if (Shown > 8) Message += TEXT("\n\nSee the Output Log for the complete list.");
        FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(Message));
    }
};

IMPLEMENT_MODULE(FLethalWorldEditorModule, LethalWorldEditor)
#undef LOCTEXT_NAMESPACE
