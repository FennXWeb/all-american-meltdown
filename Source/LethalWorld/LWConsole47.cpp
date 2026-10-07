#include "LWConsole47.h"
#include "LWDebugMenu67.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWVehicle.h"
#include "Engine/World.h"
#include "Engine/GameViewportClient.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/OutputDeviceRedirector.h"

bool LWCheats47::PositiveAmount(const FString& Text,int64 Maximum,int64& Out){
 if(Text.IsEmpty()||Text.Len()>10)return false;
 for(TCHAR C:Text)if(C<'0'||C>'9')return false;
 int64 N=0;if(!LexTryParseString(N,*Text)||N<1||N>Maximum)return false;Out=N;return true;
}
ALWCharacter* ULWConsole47::Player47()const{return GetWorld()?Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(GetWorld(),0)):nullptr;}
void ULWConsole47::Install(ALWCharacter* P){
 if(!P||!P->GetWorld()||!P->GetWorld()->GetGameViewport())return;
 auto* V=P->GetWorld()->GetGameViewport();if(!Cast<ULWConsole47>(V->ViewportConsole)){if(GLog&&V->ViewportConsole)GLog->RemoveOutputDevice(V->ViewportConsole);V->ViewportConsole=NewObject<ULWConsole47>(V);V->ViewportConsole->OutputText(TEXT("ALL AMERICAN MELTDOWN // CONSOLE\nType help for cheats. Tilde or Escape closes the console."));}
}
bool ULWConsole47::InputKey(FInputDeviceId D,FKey K,EInputEvent E,float A,bool G){
 if(auto* P=Player47();P&&P->bDebug67){if(K==EKeys::Tilde&&E==IE_Pressed&&P->DebugMenu67)P->DebugMenu67->Close();return true;}
 if(K==EKeys::Tilde){if(E==IE_Pressed){FakeGotoState(ConsoleActive()?NAME_None:FName(TEXT("Open")));bCaptureKeyInput=true;}return true;}
 return Super::InputKey(D,K,E,A,G);
}
void ULWConsole47::FakeGotoState(FName Next){
 const bool Opening=!ConsoleActive()&&!Next.IsNone(),Closing=ConsoleActive()&&Next.IsNone();
 if(auto* P=Player47())if(Opening){WasPaused47=UGameplayStatics::IsGamePaused(P);P->bConsole47=true;P->ConsumeUIAttack();P->StopAttack();P->bAim=P->bSprint=false;P->LeanLeft40=P->LeanRight40=false;P->GetCharacterMovement()->StopMovementImmediately();if(P->Vehicle){P->Vehicle->Throttle=P->Vehicle->Steer=0;P->Vehicle->CabinMove=FVector2D::ZeroVector;}UGameplayStatics::SetGamePaused(P,true);}
 Super::FakeGotoState(Next);
 if(Closing)if(auto* P=Player47()){P->bConsole47=false;P->ConsumeUIAttack();FlushPlayerInput();UGameplayStatics::SetGamePaused(P,WasPaused47);}
}
void ULWConsole47::AugmentRuntimeAutoCompleteList(TArray<FAutoCompleteCommand>& List){
 Super::AugmentRuntimeAutoCompleteList(List);
 auto Add=[&](FString C,FString D){FAutoCompleteCommand E;E.Command=C;E.Desc=D;List.Add(E);};
 Add(TEXT("debugmenu"),TEXT("Open searchable cheat, item, NPC, vehicle and POI menu"));Add(TEXT("debug"),TEXT("Alias for debugmenu"));
 Add(TEXT("help"),TEXT("List game cheat commands"));Add(TEXT("money"),TEXT("money <amount>: add credits"));Add(TEXT("give"),TEXT("give <item_id> [count]: add items"));Add(TEXT("items"),TEXT("items [search]: find item IDs"));Add(TEXT("heal"),TEXT("Restore health, stamina, food and water"));Add(TEXT("xp"),TEXT("xp <amount>: gain experience"));Add(TEXT("skillpoints"),TEXT("skillpoints <amount>: add perk points"));Add(TEXT("god"),TEXT("god [on|off]: prevent damage"));Add(TEXT("time"),TEXT("time <0-23>: set world hour"));Add(TEXT("clear"),TEXT("Clear console output"));
 Add(TEXT("spawnnpc"),TEXT("spawnnpc <type> [1-20]"));Add(TEXT("spawnvehicle"),TEXT("spawnvehicle <type>"));Add(TEXT("locate"),TEXT("locate <POI_type_or_id> [radius_km]"));Add(TEXT("npctypes"),TEXT("List NPC types"));Add(TEXT("vehicles"),TEXT("List vehicle IDs"));Add(TEXT("pois"),TEXT("List POI types"));
 for(const auto& D:LWItems::All47())Add(TEXT("give ")+D.Id.ToString(),D.DisplayName.ToString());
}
FString ULWConsole47::Execute47(const FString& Command){
 TArray<FString> A;Command.TrimStartAndEnd().ParseIntoArrayWS(A);if(A.IsEmpty())return FString();const FString C=A[0].ToLower();
 if(C==TEXT("debugmenu")||C==TEXT("debug")){if(A.Num()!=1)return TEXT("Usage: debugmenu");return ULWDebugMenu67::OpenFor(Player47());}
 if(C==TEXT("help"))return TEXT("CHEAT COMMANDS\ndebugmenu           Open searchable debug menu (alias: debug)\nspawnnpc <type> [count]  Spawn NPCs (npctypes lists IDs)\nspawnvehicle <type>  Spawn vehicle (vehicles lists IDs)\nlocate <type_or_id> [km] Find nearest POI within radius; set waypoint\npois                List POI types and IDs\nmoney <amount>       Add credits (up to 1,000,000,000)\ngive <item_id> [count] Add items (1-1000; default 1)\nitems [search]       Find IDs by name, ID, or category\nheal                 Restore health, stamina, food, water\nxp <amount>          Gain up to 100,000 experience\nskillpoints <amount> Add up to 1,000 perk points\ngod [on|off]         Toggle damage immunity (session only)\ntime <0-23>          Set world hour\nclear                Clear output\nExamples: money 5000 | give medkit 5 | give ammo_357 60\nUp/Down: history. Tab: autocomplete. Tilde/Escape: close.\nInventory, credits and progression changes use normal saves.");
 if(C==TEXT("items")){FString Query=Command.TrimStartAndEnd().Mid(5).TrimStartAndEnd();FString Out;int N=0;for(const auto& D:LWItems::All47())if(Query.IsEmpty()||(D.Id.ToString()+TEXT(" ")+D.DisplayName.ToString()+TEXT(" ")+D.Category.ToString()).Contains(Query)){Out+=D.Id.ToString()+TEXT(" - ")+D.DisplayName.ToString()+TEXT("\n");++N;}return Out+FString::Printf(TEXT("%d matching items. Use give <item_id> [count]."),N);}
 if(C==TEXT("spawnnpc")||C==TEXT("spawnvehicle")||C==TEXT("locate")||C==TEXT("npctypes")||C==TEXT("vehicles")||C==TEXT("pois"))return WorldCommand60(A);
 auto* P=Player47();if(!P||!P->bStarted||!P->World||P->OpeningMode||P->bWorldSetup)return TEXT("Start or load a game before using cheats.");
 if(P->Health<=0)return TEXT("Respawn before using cheats.");
 int64 N=0;
 if(C==TEXT("money")||C==TEXT("addmoney")){
  if(A.Num()!=2||!LWCheats47::PositiveAmount(A[1],1000000000,N))return TEXT("Usage: money <1-1000000000>");
  if(P->Money>MAX_int64-N)return TEXT("Credit limit reached; no credits added.");P->Money+=N;P->RequestSave40();return FString::Printf(TEXT("Added %lld credits. Balance: %lld."),N,P->Money);
 }
 if(C==TEXT("give")||C==TEXT("additem")){
  N=1;if(A.Num()<2||A.Num()>3||(A.Num()==3&&!LWCheats47::PositiveAmount(A[2],1000,N)))return TEXT("Usage: give <item_id> [1-1000]");
  FName Id(*A[1]);const auto& D=LWItems::Def(Id);if(D.Id.IsNone())return TEXT("Unknown item. Use items <search> to find its ID.");
  if(!P->GiveItem(Id,int32(N)))return TEXT("Not enough inventory space. No items added.");P->SyncAmmoHUD();P->RequestSave40();return FString::Printf(TEXT("Added %lld x %s."),N,*D.DisplayName.ToString());
 }
 if(C==TEXT("heal")){if(A.Num()!=1)return TEXT("Usage: heal");P->Health=P->MaxHealth();P->Stamina=P->MaxStamina();P->Hunger=P->Thirst=100;P->RequestSave40();return TEXT("Health, stamina, food and water restored.");}
 if(C==TEXT("xp")||C==TEXT("skillpoints")){int64 Max=C==TEXT("xp")?100000:1000;if(A.Num()!=2||!LWCheats47::PositiveAmount(A[1],Max,N))return FString::Printf(TEXT("Usage: %s <1-%lld>"),*C,Max);if(C==TEXT("xp"))P->GainXP(int32(N));else{if(P->RPG.Points>MAX_int32-N)return TEXT("Skill point limit reached.");P->RPG.Points+=int32(N);}P->RequestSave40();return FString::Printf(TEXT("Added %lld %s."),N,*C);}
 if(C==TEXT("god")){if(A.Num()>2||(A.Num()==2&&!A[1].Equals(TEXT("on"),ESearchCase::IgnoreCase)&&!A[1].Equals(TEXT("off"),ESearchCase::IgnoreCase)))return TEXT("Usage: god [on|off]");P->bGod47=A.Num()==1?!P->bGod47:A[1].Equals(TEXT("on"),ESearchCase::IgnoreCase);return P->bGod47?TEXT("God mode ON (session only)."):TEXT("God mode OFF.");}
 if(C==TEXT("time")){if(A.Num()!=2||!(A[1]==TEXT("0")||LWCheats47::PositiveAmount(A[1],23,N)))return TEXT("Usage: time <0-23>");P->World->TimeOfDay=float(N);P->RequestSave40();return FString::Printf(TEXT("World time set to %02lld:00."),N);}
 return TEXT("Unknown cheat. Type help for commands.");
}
void ULWConsole47::ConsoleCommand(const FString& Command){
 FString Trim=Command.TrimStartAndEnd();if(Trim.Equals(TEXT("clear"),ESearchCase::IgnoreCase)){ClearOutput();return;}
 TArray<FString> Words;Trim.ParseIntoArrayWS(Words);if(Words.IsEmpty())return;const TSet<FString> Commands={TEXT("debugmenu"),TEXT("debug"),TEXT("spawnnpc"),TEXT("spawnvehicle"),TEXT("locate"),TEXT("npctypes"),TEXT("vehicles"),TEXT("pois"),TEXT("help"),TEXT("money"),TEXT("addmoney"),TEXT("give"),TEXT("additem"),TEXT("items"),TEXT("heal"),TEXT("xp"),TEXT("skillpoints"),TEXT("god"),TEXT("time")};
 if(Commands.Contains(Words[0].ToLower())){OutputText(TEXT("> ")+Trim);OutputText(Execute47(Trim));}else Super::ConsoleCommand(Trim);
}

