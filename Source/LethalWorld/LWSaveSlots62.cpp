#include "LWSaveSlots62.h"
#include "LWCharacter.h"
#include "LWSaveGame.h"
#include "LWWorld.h"
#include "LWLoading45.h"
#include "Misc/Paths.h"
#include "Misc/FileHelper.h"
#include "HAL/FileManager.h"
#include "Misc/CommandLine.h"
#include "Misc/Parse.h"
#include "Kismet/GameplayStatics.h"
namespace LWSaves62 {
FString Prefix(){return (FParse::Param(FCommandLine::Get(),TEXT("LWV17Smoke"))||FParse::Param(FCommandLine::Get(),TEXT("LWArsenal62Tests")))?TEXT("AAM_Test62_"):TEXT("AllAmericanMeltdown_");}
FString Auto(){return Prefix()+TEXT("Autosave");}
FString Manual(int I){return Prefix()+FString::Printf(TEXT("Manual_%02d"),FMath::Clamp(I,1,20));}
bool Managed(const FString& S){if(S==Auto())return true;for(int I=1;I<=20;I++)if(S==Manual(I))return true;return false;}
FString Path(const FString& S,const TCHAR* Ext=TEXT(".sav")){return FPaths::ProjectSavedDir()/TEXT("SaveGames")/(S+Ext);}
TArray<FLWSlot62> List(){TArray<FLWSlot62> Out;for(int I=0;I<=20;I++){FLWSlot62 E;E.Slot=I?Manual(I):Auto();E.Title=I?FString::Printf(TEXT("MANUAL SAVE %02d"),I):TEXT("AUTOSAVE");E.Exists=UGameplayStatics::DoesSaveGameExist(E.Slot,0);if(E.Exists){FFileHelper::LoadFileToString(E.Detail,*Path(E.Slot,TEXT(".meta")));if(E.Detail.IsEmpty())E.Detail=IFileManager::Get().GetTimeStamp(*Path(E.Slot)).ToString();}else E.Detail=TEXT("Empty slot");Out.Add(E);}return Out;}
FString Latest(){FString Best;FDateTime Time=FDateTime::MinValue();for(auto& E:List())if(E.Exists){auto T=IFileManager::Get().GetTimeStamp(*Path(E.Slot));if(T>Time){Time=T;Best=E.Slot;}}return Best;}
bool Write(const TArray<uint8>& Bytes,const FString& Slot,const FString& Metadata){
 if(!Managed(Slot))return UGameplayStatics::SaveDataToSlot(Bytes,Slot,0); // legacy automation isolation
 const FString Temp=Slot+TEXT("_pending");if(!UGameplayStatics::SaveDataToSlot(Bytes,Temp,0))return false;
 const FString Dest=Path(Slot),Back=Path(Slot,TEXT(".bak"));
 if(IFileManager::Get().FileExists(*Dest)&&IFileManager::Get().Copy(*Back,*Dest,true,true)!=COPY_OK)return false;
 if(!IFileManager::Get().Move(*Dest,*Path(Temp),true,true,false,true))return false;
 FFileHelper::SaveStringToFile(Metadata,*Path(Slot,TEXT(".meta")));return true;
}
}
void ALWCharacter::OpenSaves62(bool Saving){
 SavePanel62=Saving?1:2;SavePage62=0;SaveConfirm62.Empty();SaveMessage62.Empty();SaveEntries62=LWSaves62::List();SetMenuInput(true);
 if(Saving){if(!bStarted||Health<=0)SaveMessage62=TEXT("Saving is unavailable right now.");else if(bStoryLocked||OpeningMode)SaveMessage62=TEXT("Finish the current scene before saving.");}
}
bool ALWCharacter::ManualSave62(const FString& Slot){
 if(!LWSaves62::Managed(Slot)||Slot==LWSaves62::Auto())return false;
 if(!bStarted||Health<=0||!World||bLoadingSave||bWorldSetup){SaveMessage62=TEXT("Saving is unavailable right now.");return false;}
 if(bStoryLocked||OpeningMode){SaveMessage62=TEXT("Finish the current scene before saving.");return false;}
 if(!PendingSave62.IsEmpty()){SaveMessage62=TEXT("A manual save is already queued.");return false;}
 // Queue behind an autosave/checkpoint writer instead of making the user click again.
 PendingSave62=Slot;SaveProgress();StartSave43();SaveMessage62=PendingSave62.IsEmpty()?TEXT("Saving..."):TEXT("Save queued...");return true;
}
bool ALWCharacter::LoadSlot62(const FString& Slot){if(!LWSaves62::Managed(Slot)&&Slot!=TEXT("LethalWorld_Survivor")&&Slot!=SaveSlot40())return false;DrainSave40();FLWLoadingScope45 Loading(TEXT("Loading survivor"));auto* S=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromSlot(Slot,0));if(!S||S->Version>2){SaveMessage62=TEXT("Could not read this save. Current progress is unchanged.");return false;}SavePanel62=0;ApplyProgressSnapshot37(S);return true;}
