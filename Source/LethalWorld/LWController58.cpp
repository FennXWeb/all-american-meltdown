#include "LWSettlement82.h"
#include "LWPlayerInput51.h"
#include "LWCharacter.h"
#include "LWHUD.h"
#include "LWVehicle.h"
#include "LWWorkbench39.h"
#include "InputKeyEventArgs.h"
#include "GameFramework/PlayerController.h"
#include "Misc/ConfigCacheIni.h"
#include "Engine/Canvas.h"
#include "GenericPlatform/GenericPlatformInputDeviceMapper.h"

void ULWPlayerInput51::Load58(){const TCHAR* S=TEXT("LWController58");GConfig->GetFloat(S,TEXT("LookSpeed"),LookSpeed58,GGameUserSettingsIni);GConfig->GetFloat(S,TEXT("DeadZone"),DeadZone58,GGameUserSettingsIni);GConfig->GetFloat(S,TEXT("CursorSpeed"),CursorSpeed58,GGameUserSettingsIni);GConfig->GetBool(S,TEXT("InvertY"),Invert58,GGameUserSettingsIni);LookSpeed58=FMath::Clamp(LookSpeed58,60.f,360.f);DeadZone58=FMath::Clamp(DeadZone58,.05f,.35f);CursorSpeed58=FMath::Clamp(CursorSpeed58,400.f,1600.f);IPlatformInputDeviceMapper::Get().GetOnInputDeviceConnectionChange().AddWeakLambda(this,[this](EInputDeviceConnectionState State,FPlatformUserId,FInputDeviceId){if(State==EInputDeviceConnectionState::Disconnected){FlushPressedKeys();if(auto* PC=Cast<APlayerController>(GetOuter()))if(auto* P=Cast<ALWCharacter>(PC->GetPawn())){P->Forward(0);P->Right(0);P->ReleaseAttackInput();P->StopAim();P->StopSprint();P->Notify(TEXT("CONTROLLER DISCONNECTED"));}}});}
void ULWPlayerInput51::Save58(){const TCHAR* S=TEXT("LWController58");GConfig->SetFloat(S,TEXT("LookSpeed"),LookSpeed58,GGameUserSettingsIni);GConfig->SetFloat(S,TEXT("DeadZone"),DeadZone58,GGameUserSettingsIni);GConfig->SetFloat(S,TEXT("CursorSpeed"),CursorSpeed58,GGameUserSettingsIni);GConfig->SetBool(S,TEXT("InvertY"),Invert58,GGameUserSettingsIni);GConfig->Flush(false,GGameUserSettingsIni);}
void ULWPlayerInput51::Change58(int I){if(I==0)LookSpeed58=LookSpeed58>=360?60:LookSpeed58+30;if(I==1)DeadZone58=DeadZone58>=.34f?.05f:FMath::Min(.35f,DeadZone58+.05f);if(I==2)Invert58=!Invert58;if(I==3)CursorSpeed58=CursorSpeed58>=1600?400:CursorSpeed58+200;Save58();}
FString ULWPlayerInput51::Label58(int I)const{if(I==0)return FString::Printf(TEXT("CONTROLLER LOOK: %.0f"),LookSpeed58);if(I==1)return FString::Printf(TEXT("STICK DEAD ZONE: %.0f%%"),DeadZone58*100);if(I==2)return Invert58?TEXT("INVERT CONTROLLER Y: ON"):TEXT("INVERT CONTROLLER Y: OFF");return FString::Printf(TEXT("MENU CURSOR SPEED: %.0f"),CursorSpeed58);}
void ULWPlayerInput51::Emit58(FKey Key,EInputEvent Event,bool MouseUI){FInputKeyEventArgs E;E.Key=Key;E.Event=Event;E.AmountDepressed=Event==IE_Released?0:1;Super::InputKey(E);if(MouseUI&&Key==EKeys::LeftMouseButton)if(auto* PC=Cast<APlayerController>(GetOuter()))if(auto* H=PC->GetHUD()){float X,Y;if(PC->GetMousePosition(X,Y))H->UpdateAndDispatchHitBoxClickEvents(FVector2D(X,Y),Event);}}
void ULWPlayerInput51::Release58(){for(const auto& H:Held58)Emit58(H.Value,IE_Released);Held58.Empty();Modifier58=false;}
void ULWPlayerInput51::FlushPressedKeys(){Release58();Axes58.Empty();Super::FlushPressedKeys();}
bool ULWPlayerInput51::PadInput58(const FInputKeyEventArgs& E){
 auto* PC=Cast<APlayerController>(GetOuter());auto* P=PC?Cast<ALWCharacter>(PC->GetPawn()):nullptr;if(!P)return true;
 if(E.Key.IsAnalog()){Axes58.Add(E.Key,E.AmountDepressed);if(FMath::Abs(E.AmountDepressed)>DeadZone58){Controller58=true;LastPad58=FPlatformTime::Seconds();}return true;}
 if(E.Event==IE_Released){if(E.Key==EKeys::Gamepad_LeftShoulder)Modifier58=false;if(auto* K=Held58.Find(E.Key)){FKey Release=*K;Held58.Remove(E.Key);Emit58(Release,IE_Released,P->IsUIOpen());}return true;}
 if(E.Event!=IE_Pressed&&!(E.Event==IE_Repeat&&P->IsUIOpen()&&(E.Key==EKeys::Gamepad_DPad_Up||E.Key==EKeys::Gamepad_DPad_Down||E.Key==EKeys::Gamepad_DPad_Left||E.Key==EKeys::Gamepad_DPad_Right)))return true;Controller58=true;LastPad58=FPlatformTime::Seconds();
 if(E.Key==EKeys::Gamepad_LeftShoulder){Modifier58=true;if(P->IsUIOpen()&&!(P->BuildMode45||(P->SettlementBuild82&&P->Settlement82&&!P->Settlement82->Palette))){Held58.Add(E.Key,EKeys::LeftShift);Emit58(EKeys::LeftShift,IE_Pressed);}return true;}
 const bool Text=P->NameEditing||P->bSeedEdit||(P->Workbench39&&P->Workbench39->EditingName);
 if(Text){const FString Letters=TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -");if(E.Key==EKeys::Gamepad_DPad_Left)TextCell58=(TextCell58+Letters.Len()-1)%Letters.Len();if(E.Key==EKeys::Gamepad_DPad_Right)TextCell58=(TextCell58+1)%Letters.Len();if(E.Key==EKeys::Gamepad_DPad_Up)TextCell58=(TextCell58+Letters.Len()-10)%Letters.Len();if(E.Key==EKeys::Gamepad_DPad_Down)TextCell58=(TextCell58+10)%Letters.Len();FString* Value=P->NameEditing?&P->DraftIdentity.Name:P->bSeedEdit?&P->SeedText:nullptr;
  if(E.Key==EKeys::Gamepad_FaceButton_Bottom){FString Ch=Letters.Mid(TextCell58,1);if(Value&&Value->Len()<(P->bSeedEdit?9:18)&&(!P->bSeedEdit||Ch.IsNumeric()))*Value+=Ch;else if(P->Workbench39)P->Workbench39->SetBuildName39(P->Workbench39->Draft.Gun.CustomName39+Ch);}
  if(E.Key==EKeys::Gamepad_FaceButton_Left){if(Value)Value->LeftChopInline(1);else if(P->Workbench39)P->Workbench39->SetBuildName39(P->Workbench39->Draft.Gun.CustomName39.LeftChop(1));}
  if(E.Key==EKeys::Gamepad_FaceButton_Right||E.Key==EKeys::Gamepad_Special_Right){P->NameEditing=P->bSeedEdit=false;if(P->Workbench39)P->Workbench39->EditingName=false;}return true;}
 const bool UI=P->IsUIOpen()&&!(P->BuildMode45||(P->SettlementBuild82&&P->Settlement82&&!P->Settlement82->Palette));
 if(E.Key==EKeys::Gamepad_Special_Right){Release58();P->ConsumeUIAttack();P->ToggleMenu();return true;}
 if(E.Key==EKeys::Gamepad_Special_Left){Release58();P->ConsumeUIAttack();if(UI)P->ClosePanels();else P->ToggleInventory();return true;}
 FKey K=EKeys::Invalid;
 if(UI){P->ConsumeUIAttack();auto* H=Cast<ALWHUD>(PC->GetHUD());
  if(E.Key==EKeys::Gamepad_FaceButton_Right){Release58();if(P->bSettings){P->bSettings=false;if(H){H->GraphicsPage=0;H->KeysPage51=-1;}}else P->ToggleMenu();return true;}
  if(P->SecurityMode){if(E.Key==EKeys::Gamepad_FaceButton_Bottom)K=EKeys::SpaceBar;}
  else if(E.Key==EKeys::Gamepad_FaceButton_Bottom){if(H&&H->Keyboard55&&H->UIKey55(EKeys::Enter))return true;K=EKeys::LeftMouseButton;}
  else if(E.Key==EKeys::Gamepad_RightShoulder)K=Modifier58?EKeys::MiddleMouseButton:EKeys::RightMouseButton;
  else if(E.Key==EKeys::Gamepad_FaceButton_Left)K=EKeys::R;
  else if(E.Key==EKeys::Gamepad_FaceButton_Top)K=EKeys::Y;
  else if(E.Key==EKeys::Gamepad_LeftTrigger)K=EKeys::MouseScrollDown;
  else if(E.Key==EKeys::Gamepad_RightTrigger)K=EKeys::MouseScrollUp;
  else {FKey Nav=E.Key==EKeys::Gamepad_DPad_Up?EKeys::Up:E.Key==EKeys::Gamepad_DPad_Down?EKeys::Down:E.Key==EKeys::Gamepad_DPad_Left?EKeys::Left:E.Key==EKeys::Gamepad_DPad_Right?EKeys::Right:EKeys::Invalid;if(Nav.IsValid()){if(H&&!P->BaseUI45&&H->UIKey55(Nav))return true;K=Nav;}}
 }else{
  if(P->Vehicle&&P->Vehicle->PlayerSeat==-1&&!P->Vehicle->IsHelicopter57()&&(E.Key==EKeys::Gamepad_RightTrigger||E.Key==EKeys::Gamepad_LeftTrigger))return true;
  if(E.Key==EKeys::Gamepad_RightTrigger)K=EKeys::LeftMouseButton;else if(E.Key==EKeys::Gamepad_LeftTrigger)K=EKeys::RightMouseButton;
  else if(E.Key==EKeys::Gamepad_FaceButton_Bottom)K=Modifier58?EKeys::Y:EKeys::SpaceBar;
  else if(E.Key==EKeys::Gamepad_FaceButton_Right)K=Modifier58?((P->BuildMode45||(P->SettlementBuild82&&P->Settlement82&&!P->Settlement82->Palette))?EKeys::Delete:EKeys::L):EKeys::LeftControl;
  else if(E.Key==EKeys::Gamepad_FaceButton_Left)K=Modifier58?EKeys::V:EKeys::R;
  else if(E.Key==EKeys::Gamepad_FaceButton_Top)K=Modifier58?EKeys::N:EKeys::E;
  else if(E.Key==EKeys::Gamepad_LeftThumbstick)K=Modifier58?EKeys::Q:EKeys::LeftShift;
  else if(E.Key==EKeys::Gamepad_RightThumbstick)K=Modifier58?EKeys::C:EKeys::MiddleMouseButton;
  else if(E.Key==EKeys::Gamepad_RightShoulder)K=(P->BuildMode45||(P->SettlementBuild82&&P->Settlement82&&!P->Settlement82->Palette))?EKeys::E:P->Vehicle&&Modifier58?EKeys::LeftAlt:EKeys::MouseScrollUp;
  else if(E.Key==EKeys::Gamepad_DPad_Up)K=Modifier58?EKeys::Tab:EKeys::H;
  else if(E.Key==EKeys::Gamepad_DPad_Down)K=Modifier58?EKeys::I:EKeys::F;
  else if(E.Key==EKeys::Gamepad_DPad_Left)K=Modifier58?EKeys::G:EKeys::MouseScrollDown;
  else if(E.Key==EKeys::Gamepad_DPad_Right)K=Modifier58?EKeys::T:EKeys::MouseScrollUp;
 }
 if(!UI&&P->Vehicle&&Modifier58&&Held58.Contains(EKeys::Gamepad_RightShoulder)){if(E.Key==EKeys::Gamepad_DPad_Up)K=EKeys::Up;if(E.Key==EKeys::Gamepad_DPad_Down)K=EKeys::Down;if(E.Key==EKeys::Gamepad_DPad_Left)K=EKeys::Left;if(E.Key==EKeys::Gamepad_DPad_Right)K=EKeys::Right;}
 if(K.IsValid()){Held58.Add(E.Key,K);Emit58(K,IE_Pressed,UI);}return true;
}
void ULWPlayerInput51::ProcessInputStack(const TArray<UInputComponent*>& Stack,float Dt,bool Paused){
 auto* PC=Cast<APlayerController>(GetOuter());auto* P=PC?Cast<ALWCharacter>(PC->GetPawn()):nullptr;auto* H=PC?Cast<ALWHUD>(PC->GetHUD()):nullptr;
 if(P&&P->bDebug67)return;
 if(P&&H){FString C=H->Context55(P);if(C!=Context58){Release58();P->ConsumeUIAttack();Context58=C;}}
 const double Now=FPlatformTime::Seconds();const float RealDt=LastFrame58>0?FMath::Clamp(float(Now-LastFrame58),.001f,.05f):.016f;LastFrame58=Now;Super::ProcessInputStack(Stack,Dt,Paused);if(!P||!Controller58)return;Dt=RealDt;
 auto Stick=[&](FKey X,FKey Y){FVector2D V(Axes58.FindRef(X),Axes58.FindRef(Y));float L=V.Size();return L<=DeadZone58?FVector2D::ZeroVector:V/L*FMath::Clamp((L-DeadZone58)/(1-DeadZone58),0.f,1.f);};
 FVector2D Move=Stick(EKeys::Gamepad_LeftX,EKeys::Gamepad_LeftY),Look=Stick(EKeys::Gamepad_RightX,EKeys::Gamepad_RightY);
 if(P->NameEditing||P->bSeedEdit||(P->Workbench39&&P->Workbench39->EditingName))return;
 if(P->SecurityMode){P->PickAngle=FMath::Clamp(P->PickAngle+Move.X*Dt*75,-90.f,90.f);return;}
 if(P->IsUIOpen()&&!(P->BuildMode45||(P->SettlementBuild82&&P->Settlement82&&!P->Settlement82->Palette))){FVector2D V=Move+Look*.5f;if(!V.IsNearlyZero()){float X=0,Y=0;int W=0,T=0;PC->GetViewportSize(W,T);if(!PC->GetMousePosition(X,Y)){X=W*.5f;Y=T*.5f;}PC->SetMouseLocation(FMath::Clamp(int(X+V.X*CursorSpeed58*Dt),0,FMath::Max(0,W-1)),FMath::Clamp(int(Y-V.Y*CursorSpeed58*Dt),0,FMath::Max(0,T-1)));if(H)H->Keyboard55=false;}return;}
 if((P->BuildMode45||(P->SettlementBuild82&&P->Settlement82&&!P->Settlement82->Palette))){for(auto Pair:{TPair<FKey,bool>(EKeys::W,Move.Y>.2f),{EKeys::S,Move.Y<-.2f},{EKeys::D,Move.X>.2f},{EKeys::A,Move.X<-.2f}}){if(IsPressed(Pair.Key)!=Pair.Value){if(Pair.Value)Held58.Add(Pair.Key,Pair.Key);else Held58.Remove(Pair.Key);Emit58(Pair.Key,Pair.Value?IE_Pressed:IE_Released);}}for(auto Pair:{TPair<FKey,float>(EKeys::MouseX,Look.X),{EKeys::MouseY,Look.Y}}){FInputKeyEventArgs E;E.Key=Pair.Key;E.Event=IE_Axis;E.AmountDepressed=Pair.Value*Dt*90;E.DeltaTime=Dt;E.NumSamples=1;Super::InputKey(E);}return;}
 if(Paused)return;if(P->Vehicle&&P->Vehicle->PlayerSeat==-1&&!P->Vehicle->IsHelicopter57()&&!P->Vehicle->IsAircraft84()){float Trigger=Axes58.FindRef(EKeys::Gamepad_RightTriggerAxis)-Axes58.FindRef(EKeys::Gamepad_LeftTriggerAxis);if(FMath::Abs(Trigger)>.05f)Move.Y=Trigger;}P->Forward(Move.Y);P->Right(Move.X);const float Gain=LookSpeed58*Dt*(P->bAim?.45f:1.f);if(P->Vehicle){P->SeatYaw=P->Vehicle->PlayerSeat==-1?FMath::Clamp(P->SeatYaw+float(Look.X)*Gain,-115.f,115.f):FMath::UnwindDegrees(P->SeatYaw+float(Look.X)*Gain);P->SeatPitch=FMath::Clamp(P->SeatPitch+float(Look.Y)*Gain*(Invert58?-1:1),-65.f,55.f);}else if(P->CanAct()){P->AddControllerYawInput(Look.X*Gain);P->AddControllerPitchInput(Look.Y*Gain*(Invert58?1:-1));}
}
FString ULWPlayerInput51::Prompt58(FKey K)const{if(K==EKeys::E)return TEXT("Y");if(K==EKeys::LeftMouseButton)return TEXT("RT");if(K==EKeys::RightMouseButton)return TEXT("LT");if(K==EKeys::R)return TEXT("X");if(K==EKeys::SpaceBar)return TEXT("A");if(K==EKeys::LeftControl)return TEXT("B");if(K==EKeys::H)return TEXT("DPAD UP");if(K==EKeys::F)return TEXT("DPAD DOWN");if(K==EKeys::I)return TEXT("VIEW");if(K==EKeys::Tab)return TEXT("LB + UP");if(K==EKeys::Escape)return TEXT("MENU");if(K==EKeys::Q)return TEXT("LB + LS");if(K==EKeys::C)return TEXT("LB + RS");if(K==EKeys::Y)return TEXT("LB + A");if(K==EKeys::L)return TEXT("LB + B");if(K==EKeys::G)return TEXT("LB + LEFT");if(K==EKeys::T)return TEXT("LB + RIGHT");if(K==EKeys::V)return TEXT("LB + X");if(K==EKeys::N)return TEXT("LB + Y");return FString();}
void ALWHUD::ControllerOverlay58(){auto* K=ULWPlayerInput51::Get51(this);auto* P=Cast<ALWCharacter>(PlayerOwner->GetPawn());if(!K||!K->Controller58||!P)return;bool TextEntry=P->NameEditing||P->bSeedEdit||(P->Workbench39&&P->Workbench39->EditingName);if(TextEntry){Rect(240,420,800,250,FLinearColor(.015,.025,.03,.98));const FString Keys=TEXT("ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 -");for(int I=0;I<Keys.Len();I++){float X=260+(I%10)*75,Y=444+(I/10)*43;if(I==K->TextCell58)Rect(X-5,Y-5,50,36,FLinearColor(.4,.25,.08,1));Text(Keys.Mid(I,1),X,Y,1.2);}Text(TEXT("DPAD SELECT   A TYPE   X DELETE   B DONE"),260,630,.85f);return;}
 if(P->IsUIOpen()&&!(P->BuildMode45||(P->SettlementBuild82&&P->Settlement82&&!P->Settlement82->Palette)))Text(TEXT("LS CURSOR   DPAD NAVIGATE   A SELECT / HOLD DRAG   B BACK   LT/RT SCROLL   RB ROTATE"),32,697,.65f);
 else if(K->Modifier58){Rect(220,490,840,165,FLinearColor(.015,.025,.03,.95));Text(TEXT("LB: UP MAP / DOWN INVENTORY / LEFT CARGO / RIGHT RADIO"),240,512,.85f);Text(TEXT("A TAKE ALL / B TRANSFER / X FIRE MODE / Y NIGHT VISION"),240,548,.85f);Text(TEXT("LS LEAN LEFT / RS LEAN RIGHT"),240,584,.85f);}
}
