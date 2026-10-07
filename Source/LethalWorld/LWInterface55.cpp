#include "LWSettlement82.h"
#include "LWHUD.h"
#include "LWMainMenu81.h"
#include "LWCampaign76.h"
#include "LWCharacter.h"
#include "LWPlayerInput51.h"
#include "LWStory.h"
#include "LWAudioCatalog.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"
#include "Engine/Font.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "Misc/ConfigCacheIni.h"
#include "HAL/PlatformTime.h"
#include "InputCoreTypes.h"
#include "AudioDevice.h"

void ALWHUD::BeginPlay(){
 Super::BeginPlay();
 FLWResolvedAudioSlot Click;if(ULWAudioCatalog::ResolveDefaultSlot(TEXT("InventoryMove"),Click))UIClick55=Click.Sound;
 UIFont55=LoadObject<UFont>(nullptr,TEXT("/Engine/EngineFonts/RobotoDistanceField.RobotoDistanceField"));
 Backdrop55=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/World78/T_Menu78.T_Menu78"));
 Skyline81=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/Menu81/T_Skyline81.T_Skyline81"));
 Foreground81=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/Menu81/T_Foreground81.T_Foreground81"));
 Mist81=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/Menu81/T_Mist81.T_Mist81"));
 Surface55=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/UI55/T_Surface55.T_Surface55"));
 FontAtlas=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/Textures/T_Font.T_Font"));
 GConfig->GetBool(TEXT("LWInterface55"),TEXT("ReducedMotion"),ReducedMotion55,GGameUserSettingsIni);
 GConfig->GetBool(TEXT("LWInterface55"),TEXT("HighContrast"),HighContrast55,GGameUserSettingsIni);
}
void ALWHUD::SaveUI55(){
 GConfig->SetBool(TEXT("LWInterface55"),TEXT("ReducedMotion"),ReducedMotion55,GGameUserSettingsIni);
 GConfig->SetBool(TEXT("LWInterface55"),TEXT("HighContrast"),HighContrast55,GGameUserSettingsIni);
 GConfig->Flush(false,GGameUserSettingsIni);
}
FString ALWHUD::Context55(ALWCharacter* P)const{
 if(!P)return TEXT("none");
 return FString::Printf(TEXT("%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d"),P->bMenu,P->bSettings,P->bInventory,P->bMap,P->RPGPanel,P->OpeningMode,P->bWorldSetup,P->SecurityMode,P->BaseUI45,P->Workbench39!=nullptr,P->Health<=0,GraphicsPage,KeysPage51,P->bVideoConfirm)+FString::Printf(TEXT(":%d:%d"),SettingsSection78,P->Campaign76&&P->Campaign76->SceneOpen)+FString::Printf(TEXT(":%d:%d"),P->SavePanel62,!P->SaveConfirm62.IsEmpty())+FString::Printf(TEXT(":%d:%d"),News81,Release81)+FString::Printf(TEXT(":%d:%d:%d"),P->SettlementBuild82,P->Settlement82?P->Settlement82->Palette:0,P->Settlement82?P->Settlement82->Panel:0);
}
void ALWHUD::Frame55(ALWCharacter* P){
 if(P->bStarted&&Menu81){Menu81->Revision++;Menu81->StopPortrait();Menu81=nullptr;News81=false;}
 const double Now=FPlatformTime::Seconds();Delta55=FMath::Clamp(float(Now-LastTime55),0.f,.05f);LastTime55=Now;
 if(SettingsDrag55>=0&&(!P->bSettings||GraphicsPage||KeysPage51>=0||P->bVideoConfirm)){SettingsDrag55=-1;P->ApplySettings();}
 if(P->Campaign76&&P->Campaign76->SceneOpen&&!PlayerOwner->bShowMouseCursor)P->SetMenuInput(true);
 FString Next=Context55(P);if(Screen55!=Next){Screen55=Next;Reveal55=ReducedMotion55?1:.35f;Focus55=NAME_None;Controls55.Reset();Keyboard55=false;DragId.Invalidate();CreatorDrag35=-1;}
 Reveal55=ReducedMotion55?1:FMath::FInterpTo(Reveal55,1.f,Delta55,14.f);
 Controls55.Reset();Tooltip55.Empty();
 float X=0,Y=0;if(Mouse55(X,Y)){FVector2D M(X,Y);if(FVector2D::DistSquared(M,LastPointer55)>4)Keyboard55=false;LastPointer55=M;}
 if(GFrameCounter%120==0)for(auto It=Motion55.CreateIterator();It;++It)if(GFrameCounter>It.Value().Frame+180)It.RemoveCurrent();
}
float ALWHUD::Animate55(FName Id,float Target,float Rate){
 auto* Existing=Motion55.Find(Id);if(!Existing){FLWUIAnim55 A;A.Value=Target;A.Frame=GFrameCounter;Motion55.Add(Id,A);return Target;}
 if(Existing->Frame!=GFrameCounter){Existing->Value=ReducedMotion55?Target:FMath::FInterpTo(Existing->Value,Target,Delta55,Rate);Existing->Frame=GFrameCounter;}return Existing->Value;
}
bool ALWHUD::Mouse55(float& X,float& Y)const{
 if(!PlayerOwner||!PlayerOwner->GetMousePosition(X,Y))return false;X-=Origin55.X;Y-=Origin55.Y;return true;
}
void ALWHUD::AddHitBox(FVector2D Position,FVector2D Size,FName Name,bool Consume,int32 Priority){
 auto* P=PlayerOwner?Cast<ALWCharacter>(PlayerOwner->GetPawn()):nullptr;
 if(P&&P->bVideoConfirm&&Name!=TEXT("ConfirmVideo")&&Name!=TEXT("RevertVideo"))return;
 if(P&&P->SavePanel62&&!P->SaveConfirm62.IsEmpty()&&Name!=TEXT("save62_confirm")&&Name!=TEXT("save62_cancel"))return;
 Super::AddHitBox(Position+Origin55,Size,Name,Consume,Priority);
 Controls55.Add({Name,(Position+Size*.5)/Scale,Priority});
 if(Focus55.IsNone())Focus55=Name;
 if(Keyboard55&&Focus55==Name){const FLinearColor C(.95,.68,.32,.9);Line(Position.X/Scale,Position.Y/Scale,(Position.X+Size.X)/Scale,Position.Y/Scale,C,2);Line(Position.X/Scale,(Position.Y+Size.Y)/Scale,(Position.X+Size.X)/Scale,(Position.Y+Size.Y)/Scale,C,2);}
}
void ALWHUD::SurfacePanel55(float X,float Y,float W,float H,FLinearColor C){
 // Large surfaces share one material; small slots remain cheap solid primitives.
 const float A=HighContrast55?FMath::Max(C.A,.96f):C.A;
 DrawRect(FLinearColor(.009f,.017f,.023f,A),X*Scale,Y*Scale,W*Scale,H*Scale);
 if(Surface55&&!HighContrast55&&W>180&&H>90)DrawTexture(Surface55,X*Scale,Y*Scale,W*Scale,H*Scale,0,0,1,1,FLinearColor(.34f,.42f,.43f,.045f*Reveal55),BLEND_Translucent);
 if(W<1250&&H<700){
  Line(X,Y,X+W*Reveal55,Y,FLinearColor(.38,.48,.48,.28f*Reveal55));
  Line(X,Y+H,X+W,Y+H,FLinearColor(.02,.035,.04,.85));
  Line(X,Y,X,Y+FMath::Min(12.f,H),FLinearColor(.68,.51,.3,.48));
 }
}
void ALWHUD::Control55(FName Id,const FString& Label,float X,float Y,float W,float H,bool Selected,bool Register){
 float MX=0,MY=0;bool HasMouse=Mouse55(MX,MY);MX/=Scale;MY/=Scale;
 bool Hover=HasMouse&&MX>=X&&MX<X+W&&MY>=Y&&MY<Y+H;
 auto* P=Cast<ALWCharacter>(PlayerOwner->GetPawn());bool Disabled=P&&P->bVideoConfirm&&Id!=TEXT("ConfirmVideo")&&Id!=TEXT("RevertVideo");
 if(Disabled)Hover=false;
 const bool Focus=Selected||(Keyboard55&&Focus55==Id);
 const float Hot=Animate55(Id,(!Disabled&&(Hover||Focus))?1.f:0.f,18);
 const bool Pressed=Hover&&PlayerOwner->IsInputKeyDown(EKeys::LeftMouseButton);
 const FLinearColor Gold(.98,.59,.28),Paper(.91,.93,.91),Dim(.61,.69,.72);
 DrawRect(Selected?FLinearColor(.11,.23,.26,1):FLinearColor(.019+Hot*.025,.031+Hot*.045,.040+Hot*.048,HighContrast55?1:.94),X*Scale,Y*Scale,W*Scale,H*Scale);
 Line(X,Y+H,X+W,Y+H,FLinearColor(.2,.3,.32,.55));
 if(Hot>.01f){Line(X,Y,X+W*Hot,Y,FLinearColor(.95,.65,.30,Hot*.7f),1);DrawRect(FLinearColor(.95,.65,.30,Hot*.10f),(X+W-16)*Scale,Y*Scale,16*Scale,H*Scale);}
 if(Focus||Hot>.02f)Rect(X,Y+10,2,H-20,FLinearColor(.98,.59,.28,Selected?1.f:Hot));
 const float Font=FMath::Max(.72f,FMath::Min(H<34?.83f:1.02f,(W-(W<100?14:35))/FMath::Max(1.f,float(Label.Len())*9.f)));
 const int MaxChars=FMath::Max(1,FMath::FloorToInt((W-(W<100?14:35))/(Font*9)));FString Display=Label.Len()>MaxChars?Label.Left(FMath::Max(1,MaxChars-3))+TEXT("..."):Label;
 if(Hover&&Display!=Label){Tooltip55=Label;TooltipAt55=FVector2D(X,Y+H+7);}
 Text(Display,X+(W<100?7:14)+Hot*3,Y+(H-17*Font)*.5f+(Pressed?1:0),FMath::Max(.56f,Font),Disabled?Dim*.5f:FMath::Lerp(Dim,Paper,Selected?1.f:Hot));
 if(Register)AddHitBox(FVector2D(X,Y)*Scale,FVector2D(W,H)*Scale,Id,true,(Id==TEXT("ConfirmVideo")||Id==TEXT("RevertVideo"))?100:0);
}
bool ALWHUD::UIKey55(FKey Key){
 auto* P=PlayerOwner?Cast<ALWCharacter>(PlayerOwner->GetPawn()):nullptr;
 if(P&&!P->bStarted&&News81&&(Key==EKeys::Escape||Key==EKeys::Gamepad_FaceButton_Right)){News81=false;P->ConsumeUIAttack();return true;}
 if(!P||!P->IsUIOpen()||P->BaseUI45||P->BuildMode45||(P->SettlementBuild82&&P->Settlement82&&!P->Settlement82->Palette)||P->bSeedEdit||P->OpeningMode==2||P->SecurityMode||P->bConsole47||Screen55!=Context55(P))return false;
 if(auto* K=ULWPlayerInput51::Get51(this);K&&K->Capture>=0)return false;
 if(Key!=EKeys::Up&&Key!=EKeys::Down&&Key!=EKeys::Left&&Key!=EKeys::Right&&Key!=EKeys::Enter&&Key!=EKeys::Gamepad_DPad_Up&&Key!=EKeys::Gamepad_DPad_Down&&Key!=EKeys::Gamepad_DPad_Left&&Key!=EKeys::Gamepad_DPad_Right&&Key!=EKeys::Gamepad_FaceButton_Bottom)return false;
 if(Controls55.IsEmpty())return false;
 Keyboard55=true;P->ConsumeUIAttack();
 int Current=Controls55.IndexOfByPredicate([&](const auto& C){return C.Id==Focus55;});if(Current<0)Current=0;
 if(Key==EKeys::Enter||Key==EKeys::Gamepad_FaceButton_Bottom){NotifyHitBoxClick(Controls55[Current].Id);return true;}
 FVector2D D=(Key==EKeys::Left||Key==EKeys::Gamepad_DPad_Left)?FVector2D(-1,0):(Key==EKeys::Right||Key==EKeys::Gamepad_DPad_Right)?FVector2D(1,0):(Key==EKeys::Up||Key==EKeys::Gamepad_DPad_Up)?FVector2D(0,-1):FVector2D(0,1);
 int Pick=-1;double Best=DBL_MAX;
 for(int I=0;I<Controls55.Num();++I){if(I==Current)continue;FVector2D V=Controls55[I].Center-Controls55[Current].Center;double Forward=FVector2D::DotProduct(V,D),Side=FMath::Abs(V.X*D.Y-V.Y*D.X);if(Forward<1)continue;double Score=Forward+Side*3;if(Score<Best){Best=Score;Pick=I;}}
 if(Pick<0)Pick=(Current+(D.X+D.Y>0?1:Controls55.Num()-1))%Controls55.Num();Focus55=Controls55[Pick].Id;return true;
}

void ALWHUD::Finish55(){
 ControllerOverlay58();
 if(Tooltip55.IsEmpty())return;
 const float W=FMath::Min(900.f,float(Tooltip55.Len())*7.2f+24),X=FMath::Clamp(float(TooltipAt55.X),12.f,Canvas->ClipX/Scale-W-12),Y=FMath::Clamp(float(TooltipAt55.Y),12.f,674.f);
 Rect(X,Y,W,34,FLinearColor(.012,.02,.024,.99));Text(Tooltip55,X+12,Y+9,.8f,FLinearColor(.92,.93,.89));
}

void ALWHUD::SettingsSlider55(ALWCharacter* P,int Kind,float X,float Y,float W){
 float MX=0,MY=0;const bool Mouse=Mouse55(MX,MY);MX/=Scale;MY/=Scale;
 const bool Down=PlayerOwner->IsInputKeyDown(EKeys::LeftMouseButton);
 if(!P->bVideoConfirm&&Mouse&&PlayerOwner->WasInputKeyJustPressed(EKeys::LeftMouseButton)&&MX>=X-5&&MX<=X+W+5&&MY>=Y-6&&MY<=Y+6){SettingsDrag55=Kind;P->ConsumeUIAttack();}
 if(SettingsDrag55==Kind){
  if(Down&&Mouse){const float T=FMath::Clamp((MX-X)/W,0.f,1.f);if(Kind==0)P->Sensitivity=.025f+T*T*4.975f;else {P->MasterVolume=T;if(FAudioDeviceHandle Device=GetWorld()->GetAudioDevice())Device->SetTransientPrimaryVolume(T);}P->ConsumeUIAttack();}
  else {SettingsDrag55=-1;P->ApplySettings();}
 }
 float T=Kind==0?FMath::Sqrt(FMath::Clamp((P->Sensitivity-.025f)/4.975f,0.f,1.f)):P->MasterVolume;
 Rect(X,Y-2,W,4,FLinearColor(.12,.2,.22));Rect(X,Y-2,W*T,4,FLinearColor(.85,.58,.29));Rect(X+W*T-3,Y-6,6,12,FLinearColor(.9,.93,.87));
}
