#include "LWPlayerInput51.h"
#include "LWCharacter.h"
#include "LWHUD.h"
#include "InputKeyEventArgs.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Misc/ConfigCacheIni.h"
namespace { FKey Default56(FKey K){if(K==EKeys::LeftControl)return EKeys::C;if(K==EKeys::C)return EKeys::E;if(K==EKeys::E)return EKeys::F;if(K==EKeys::F)return EKeys::H;if(K==EKeys::H)return EKeys::LeftControl;if(K==EKeys::Tab)return EKeys::M;if(K==EKeys::M)return EKeys::Tab;return K;} }
ULWPlayerInput51* ULWPlayerInput51::Get51(const UObject* C){auto* PC=UGameplayStatics::GetPlayerController(C,0);return PC?Cast<ULWPlayerInput51>(PC->PlayerInput):nullptr;}
void ULWPlayerInput51::Load51(){
 if(Loaded)return;Loaded=true;
 auto Add=[&](FKey K,const TCHAR* S){Bindings.Add({K,K,S});};
 Add(EKeys::W,TEXT("FORWARD / ACCELERATE"));Add(EKeys::S,TEXT("BACK / BRAKE"));Add(EKeys::A,TEXT("LEFT / STEER LEFT"));Add(EKeys::D,TEXT("RIGHT / STEER RIGHT"));
 Add(EKeys::SpaceBar,TEXT("JUMP / VAULT / CLIMB / HANDBRAKE"));Add(EKeys::LeftShift,TEXT("SPRINT / MODIFIER"));Add(EKeys::LeftControl,TEXT("CROUCH / SPRINT SLIDE"));
 Add(EKeys::LeftMouseButton,TEXT("FIRE"));Add(EKeys::RightMouseButton,TEXT("AIM"));Add(EKeys::MiddleMouseButton,TEXT("GUN BASH"));
 Add(EKeys::R,TEXT("RELOAD"));Add(EKeys::E,TEXT("INTERACT"));Add(EKeys::F,TEXT("FLASHLIGHT / HEADLIGHTS"));Add(EKeys::Q,TEXT("LEAN LEFT"));Add(EKeys::C,TEXT("LEAN RIGHT"));Add(EKeys::H,TEXT("HEAL"));Add(EKeys::V,TEXT("FIRE MODE / WIPERS"));
 Add(EKeys::One,TEXT("PRIMARY WEAPON"));Add(EKeys::Two,TEXT("SECONDARY WEAPON"));Add(EKeys::Three,TEXT("SIDEARM"));Add(EKeys::Four,TEXT("MELEE"));Add(EKeys::Five,TEXT("CHEST RIG WEAPON"));
 Add(EKeys::MouseScrollUp,TEXT("NEXT WEAPON"));Add(EKeys::MouseScrollDown,TEXT("PREVIOUS WEAPON"));Add(EKeys::I,TEXT("INVENTORY"));Add(EKeys::Tab,TEXT("MAP"));Add(EKeys::K,TEXT("SKILLS"));Add(EKeys::J,TEXT("CONTRACTS"));Add(EKeys::O,TEXT("CREW"));Add(EKeys::B,TEXT("BUNKER WAYPOINT"));Add(EKeys::N,TEXT("NIGHT VISION"));Add(EKeys::G,TEXT("VEHICLE CARGO"));Add(EKeys::T,TEXT("VEHICLE RADIO"));Add(EKeys::Z,TEXT("LEFT SIGNAL"));Add(EKeys::X,TEXT("RIGHT SIGNAL"));Add(EKeys::LeftAlt,TEXT("SEAT ADJUST MODIFIER"));
 Add(EKeys::P,TEXT("PRONE / STAND"));Add(EKeys::Y,TEXT("QUICK LOOT: TAKE ALL"));Add(EKeys::L,TEXT("QUICK LOOT: TRANSFER"));
 // Apply saved pairs as swaps so even stale or hand-edited files cannot create duplicate bindings.
 for(int I=0;I<Bindings.Num();++I){FString S;if(GConfig->GetString(TEXT("LWKeybinds51"),*Bindings[I].Logical.ToString(),S,GGameUserSettingsIni)){FKey K(*S);if(K.IsValid()&&!K.IsAnalog()&&K!=EKeys::Escape&&K!=EKeys::Tilde&&K!=EKeys::Enter){int Other=Bindings.IndexOfByPredicate([&](const auto& B){return B.Physical==K;});if(Other!=INDEX_NONE)Swap(Bindings[I].Physical,Bindings[Other].Physical);else Bindings[I].Physical=K;}}}
 int Version=0;GConfig->GetInt(TEXT("LWKeybinds51"),TEXT("DefaultsVersion"),Version,GGameUserSettingsIni);
 if(Version<56){for(auto& B:Bindings)B.Physical=Default56(B.Physical);Save51();}
 for(auto& B:Bindings)if(B.Logical==EKeys::LeftControl)B.Label=TEXT("CROUCH / SPRINT SLIDE / HOLD PRONE");
}
void ULWPlayerInput51::Save51(){GConfig->SetInt(TEXT("LWKeybinds51"),TEXT("DefaultsVersion"),56,GGameUserSettingsIni);for(const auto& B:Bindings)GConfig->SetString(TEXT("LWKeybinds51"),*B.Logical.ToString(),*B.Physical.ToString(),GGameUserSettingsIni);GConfig->Flush(false,GGameUserSettingsIni);}
void ULWPlayerInput51::Reset51(){Load51();for(auto& B:Bindings)B.Physical=Default56(B.Logical);Capture=-1;Status=TEXT("DEFAULTS RESTORED");Save51();}
bool ULWPlayerInput51::Assign51(int I,FKey K){Load51();if(!Bindings.IsValidIndex(I)||!K.IsValid()||K.IsAnalog()||K==EKeys::Escape||K==EKeys::Tilde||K==EKeys::Enter||K==EKeys::F5||K==EKeys::F9||K==EKeys::F11||K==EKeys::Up||K==EKeys::Down||K==EKeys::Left||K==EKeys::Right){Status=TEXT("THAT KEY IS RESERVED");return false;}int Other=Bindings.IndexOfByPredicate([&](const auto& B){return B.Physical==K;});if(Other!=INDEX_NONE&&Other!=I){Swap(Bindings[I].Physical,Bindings[Other].Physical);Status=TEXT("BINDINGS SWAPPED");}else{Bindings[I].Physical=K;Status=TEXT("BINDING SAVED");}return true;}
FKey ULWPlayerInput51::Translate51(FKey K)const{for(const auto& B:Bindings)if(B.Physical==K)return B.Logical;for(const auto& B:Bindings)if(B.Logical==K)return EKeys::Invalid;return K;}
bool ULWPlayerInput51::InputKey(const FInputKeyEventArgs& P){
 Load51();auto* PC=Cast<APlayerController>(GetOuter());auto* C=PC?Cast<ALWCharacter>(PC->GetPawn()):nullptr;
 if(P.Event==IE_Released){if(Suppressed51.Remove(P.Key))return true;if(FKey* K=Held51.Find(P.Key)){auto R=P;R.Key=*K;Held51.Remove(P.Key);return Super::InputKey(R);}}
 if(Capture>=0&&C&&C->bSettings){if(P.Event==IE_Pressed&&GFrameCounter>CaptureFrame){Suppressed51.Add(P.Key);C->ConsumeUIAttack();if(P.Key==EKeys::Escape){Capture=-1;Status=TEXT("CANCELLED");}else if(Assign51(Capture,P.Key)){Capture=-1;Save51();}return true;}return true;}
 if(C&&!C->bMenu&&!C->bSettings&&(C->bMap||C->bInventory||(C->RPGPanel>=1&&C->RPGPanel<=3)||C->RPGPanel==6)&&Translate51(P.Key)==EKeys::Tab&&P.Event==IE_Pressed){C->ToggleMap();Suppressed51.Add(P.Key);return true;}
 if(C&&C->IsUIOpen()&&P.Event==IE_Repeat&&(P.Key==EKeys::Enter||P.Key==EKeys::Gamepad_FaceButton_Bottom))return true;
 if(C&&C->IsUIOpen()&&(P.Event==IE_Pressed||P.Event==IE_Repeat))if(auto* H=Cast<ALWHUD>(PC->GetHUD());H&&H->UIKey55(P.Key)){Suppressed51.Add(P.Key);return true;}
 if(!C||C->IsUIOpen()){Capture=-1;if(C&&P.Event==IE_Pressed&&(P.Key==EKeys::LeftMouseButton||Translate51(P.Key)==EKeys::LeftMouseButton))C->ConsumeUIAttack();return Super::InputKey(P);}
 auto R=P;R.Key=Translate51(P.Key);if(!R.Key.IsValid())return true;
 if(P.Event==IE_Pressed)Held51.Add(P.Key,R.Key);
 return Super::InputKey(R);
}
void ALWHUD::KeysScreen51(){auto* K=ULWPlayerInput51::Get51(this);if(!K)return;K->Load51();Text(TEXT("KEY BINDINGS"),60,153,1.5f);constexpr int PageSize=14;int Pages=FMath::DivideAndRoundUp(K->Bindings.Num(),PageSize);KeysPage51=FMath::Clamp(KeysPage51,0,Pages-1);
 for(int J=0;J<PageSize;++J){int I=KeysPage51*PageSize+J;if(!K->Bindings.IsValidIndex(I))break;const auto& B=K->Bindings[I];Button(FName(*FString::Printf(TEXT("key51_%d"),I)),B.Label+TEXT("  :  ")+(K->Capture==I?TEXT("PRESS A KEY"):B.Physical.GetDisplayName().ToString().ToUpper()),60+(J/7)*600,198+(J%7)*49,565);}
 Text(K->Capture>=0?TEXT("PRESS A KEY OR MOUSE BUTTON. ESC CANCELS."):K->Status,60,551,.9f);Text(TEXT("DUPLICATE KEYS SWAP. MENU NAVIGATION AND CONSOLE KEEP THEIR DEFAULT KEYS."),60,580,.8f);
 Button(TEXT("keys51_back"),TEXT("BACK"),60,625,230);Button(TEXT("keys51_reset"),TEXT("RESET ALL"),320,625,250);Button(TEXT("keys51_next"),FString::Printf(TEXT("PAGE %d / %d >"),KeysPage51+1,Pages),800,625,370);
}
bool ALWHUD::KeysClick51(FName N){auto* K=ULWPlayerInput51::Get51(this);if(!K)return false;if(N==TEXT("keys51")){KeysPage51=0;GraphicsPage=0;K->Status.Empty();return true;}if(KeysPage51<0)return false;if(K->Capture>=0)return true;
 if(N==TEXT("keys51_back")){KeysPage51=-1;K->Capture=-1;}else if(N==TEXT("keys51_reset"))K->Reset51();else if(N==TEXT("keys51_next")){K->Load51();KeysPage51=(KeysPage51+1)%FMath::DivideAndRoundUp(K->Bindings.Num(),14);K->Capture=-1;}else if(N.ToString().StartsWith(TEXT("key51_"))){K->Capture=FCString::Atoi(*N.ToString().Mid(6));K->CaptureFrame=GFrameCounter;}return true;}
