#include "LWHUD.h"
#include "LWMainMenu81.h"
#include "LWCharacter.h"
#include "LWSaveSlots62.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/PlayerController.h"
#include "HAL/PlatformTime.h"

namespace {
const FLinearColor Paper81(.94f,.94f,.88f),Dim81(.59f,.68f,.71f),Gold81(1,.66f,.34f);
// Match Text's proportional font measurement rather than guessing line lengths.
float Width81(ALWHUD& H,const FString& S,float Size){
 float W=0,T=0;H.GetTextSize(S,W,T,H.UIFont55,1);
 return W*FMath::Min(9*Size*S.Len()/FMath::Max(1.f,W),17*Size/FMath::Max(1.f,T));
}
FString Fit81(ALWHUD& H,FString S,float W,float Size){
 if(Width81(H,S,Size)<=W)return S;
 while(S.Len()>1&&Width81(H,S+TEXT("..."),Size)>W)S.LeftChopInline(1);
 return S+TEXT("...");
}
float Wrap81(ALWHUD& H,const FString& S,float X,float Y,float W,float Size,FLinearColor Color,int Limit=8,bool Draw=true){
 TArray<FString> Words;S.ParseIntoArrayWS(Words);FString Row;int Lines=0;
 for(int I=0;I<Words.Num();I++){
  const FString Next=Row.IsEmpty()?Words[I]:Row+TEXT(" ")+Words[I];
  if(!Row.IsEmpty()&&Width81(H,Next,Size)>W){
   if(++Lines==Limit){for(;I<Words.Num();I++)Row+=TEXT(" ")+Words[I];if(Draw)H.Text(Fit81(H,Row,W,Size),X,Y,Size,Color);return Y+24*Size;}
   if(Draw)H.Text(Row,X,Y,Size,Color);Y+=24*Size;Row=Words[I];
  }else Row=Next;
 }
 if(!Row.IsEmpty()){if(Draw)H.Text(Fit81(H,Row,W,Size),X,Y,Size,Color);Y+=24*Size;}return Y;
}
}

void ALWHUD::EndPlay(const EEndPlayReason::Type Reason){if(Menu81){Menu81->Revision++;Menu81->StopPortrait();}Super::EndPlay(Reason);}

void ALWHUD::MenuBackdrop81(){
 const float W=Canvas->ClipX/Scale,H=720;
 const double Time=ReducedMotion55?0:FPlatformTime::Seconds();
 float MX=0,MY=0;const bool Pointer=Mouse55(MX,MY);
 const float PX=ReducedMotion55||!Pointer?0:FMath::Clamp(MX/Canvas->ClipX-.5f,-.5f,.5f);
 const float PY=ReducedMotion55||!Pointer?0:FMath::Clamp(MY/(H*Scale)-.5f,-.5f,.5f);
 const float PanX=Animate55(TEXT("parallax_x81"),PX,2),PanY=Animate55(TEXT("parallax_y81"),PY,2);
 DrawRect(FLinearColor(.008,.016,.026,1),0,0,W*Scale,H*Scale);
 auto Layer=[&](UTexture2D* T,float Zoom,float X,float Y,float Alpha){
  if(!T)return;const float Factor=FMath::Max(W/T->GetSizeX(),H/T->GetSizeY())*Zoom;
  const float TW=T->GetSizeX()*Factor,TH=T->GetSizeY()*Factor;
  DrawTexture(T,((W-TW)*.5f+X)*Scale,((H-TH)*.5f+Y)*Scale,TW*Scale,TH*Scale,0,0,1,1,FLinearColor(1,1,1,Alpha),BLEND_Translucent);
 };
 Layer(Skyline81?Skyline81:Backdrop55,1.045f,float(FMath::Sin(Time*.027)*7)-PanX*6,float(FMath::Sin(Time*.021)*3)-PanY*4,1);
 Layer(Mist81,1.3f,float(FMath::Sin(Time*.065)*100)-PanX*13,24,.07f);
 Layer(Foreground81,1.065f,PanX*19+float(FMath::Sin(Time*.032)*3),PanY*9,1);
 // Soft lower and left falloff keeps the art visible without sacrificing legibility.
 for(int I=0;I<20;I++){
  DrawRect(FLinearColor(.004,.01,.016,.35f*(1-I/20.f)),I*(W/20)*Scale,0,(W/20+1)*Scale,H*Scale);
  DrawRect(FLinearColor(.003,.008,.015,.42f*I/20),0,(440+I*14)*Scale,W*Scale,15*Scale);
 }
 if(!ReducedMotion55)for(int I=0;I<14;I++){
  const float X=FMath::Fmod(I*157.3f+float(Time)*(.9f+(I%3)*.4f),W);
  const float Y=470-FMath::Fmod(float(Time)*(1.1f+(I%4)*.23f)+I*41.f,400.f);
  const float A=.09f+.11f*FMath::Square(FMath::Sin(float(Time)*.4f+I));
  DrawRect(FLinearColor(1,.62f,.26f,A),X*Scale,Y*Scale,1.4f*Scale,1.4f*Scale);
 }
}

void ALWHUD::MenuTile81(FName Id,const FString& Label,float X,float Y,float W,float H,int Icon,bool Enabled){
 float MX=0,MY=0;bool Hover=Mouse55(MX,MY)&&MX>=X*Scale&&MX<(X+W)*Scale&&MY>=Y*Scale&&MY<(Y+H)*Scale;
 const float Hot=Animate55(Id,Enabled&&(Hover||(Keyboard55&&Focus55==Id))?1.f:0.f,12);
 const FLinearColor Edge=FMath::Lerp(FLinearColor(.20,.32,.36,.65),Gold81,Hot);
 DrawRect(FLinearColor(.008,.02,.029,HighContrast55?1:.92f),X*Scale,Y*Scale,W*Scale,H*Scale);
 DrawRect(FLinearColor(.19,.32,.35,.12f+Hot*.09f),X*Scale,Y*Scale,W*Scale,H*Scale);
 Line(X,Y,X+W,Y,Edge,1+Hot);Line(X,Y+H,X+W,Y+H,Edge,1+Hot);
 Line(X,Y,X,Y+H,Edge);Line(X+W,Y,X+W,Y+H,Edge);
 if(Hot>.01)DrawRect(FLinearColor(1,.66,.34,.055f*Hot),X*Scale,Y*Scale,W*Scale,H*Scale);
 if(Icon>=0){
  float CX=X+28,CY=Y+35-Hot*3;const auto C=FMath::Lerp(Dim81,Gold81,Hot);
  if(Icon==0){Line(CX,CY+12,CX+26,CY+12,C,2);Line(CX+13,CY-1,CX+13,CY+25,C,2);}
  if(Icon==1){Line(CX,CY+4,CX+11,CY+4,C,2);Line(CX+11,CY+4,CX+16,CY+10,C,2);Line(CX+16,CY+10,CX+31,CY+10,C,2);Line(CX+31,CY+10,CX+31,CY+28,C,2);Line(CX+31,CY+28,CX,CY+28,C,2);Line(CX,CY+28,CX,CY+4,C,2);}
  if(Icon==2)for(int I=0;I<3;I++){float YY=CY+I*11;Line(CX,YY,CX+30,YY,C,1.5);DrawRect(C,(CX+5+(I%2)*16)*Scale,(YY-3)*Scale,4*Scale,6*Scale);}
  if(Icon==3){Line(CX,CY,CX+28,CY,C,2);Line(CX,CY+12,CX+28,CY+12,C,2);Line(CX,CY+24,CX+17,CY+24,C,2);DrawRect(Gold81,(X+W-30)*Scale,(Y+25)*Scale,5*Scale,5*Scale);}
  Text(Label,X+25,Y+H-45,1.30f,Paper81);
  Line(X+W-37,Y+H-36,X+W-23,Y+H-36,C);Line(X+W-28,Y+H-41,X+W-23,Y+H-36,C);Line(X+W-28,Y+H-31,X+W-23,Y+H-36,C);
 }
 if(Enabled)AddHitBox(FVector2D(X,Y)*Scale,FVector2D(W,H)*Scale,Id,true,1);
}

void ALWHUD::MainMenu81(ALWCharacter* P){
 if(!Menu81){Menu81=NewObject<ULWMainMenu81>(this);Menu81->Refresh();}
 const float W=Canvas->ClipX/Scale,X=(W-1170)*.5f;
 if(News81){
  DrawRect(FLinearColor(.004,.012,.020,.83),0,0,W*Scale,720*Scale);
  Text(TEXT("What's new"),X,65,2.65f,Paper81);Control55(TEXT("news81_back"),TEXT("Back"),X+1010,64,160,45);
  const auto& Releases=LWMenu81::Releases();Release81=FMath::Clamp(Release81,0,Releases.Num()-1);
  for(int I=0;I<Releases.Num();I++)Control55(FName(*FString::Printf(TEXT("release81_%d"),I)),Releases[I].Version,X,169+I*68,226,55,I==Release81);
  const auto& R=Releases[Release81];const float RX=X+260;
  DrawRect(FLinearColor(.012,.027,.037,.97),(RX)*Scale,154*Scale,910*Scale,481*Scale);
  Text(R.Date,RX+32,181,.84f,Gold81);Text(R.Title,RX+32,219,2,Paper81);
  Line(RX+32,271,RX+878,271,FLinearColor(.27,.39,.41,.55));float Y=300;
  float NoteSize=1.03f;
  auto NotesHeight=[&](float Size){float Height=0;for(const auto& N:R.Notes)Height=Wrap81(*this,N,0,Height,812,Size,Paper81,8,false)+12*Size;return Height;};
  while(NoteSize>.78f&&NotesHeight(NoteSize)>315)NoteSize-=.02f;
  for(const auto& N:R.Notes){DrawRect(Gold81,(RX+34)*Scale,(Y+6)*Scale,4*Scale,4*Scale);Y=Wrap81(*this,N,RX+54,Y,812,NoteSize,Paper81)+12*NoteSize;}
  return;
 }
 Logo(X-8,18,424,214,!ReducedMotion55);
 const float Y=252,CW=674,CH=356;
 MenuTile81(TEXT("Start"),TEXT("Continue"),X,Y,CW,CH,-1,Menu81->Ready);
 if(Menu81->Ready){
  if(!Menu81->Portrait&&!Keyboard55)Focus55=TEXT("Start");
  Menu81->EnsurePortrait(GetWorld());
  if(Menu81->Portrait&&Menu81->Portrait->Target){
   Menu81->Portrait->Render(FPlatformTime::Seconds(),ReducedMotion55);
   DrawTexture(Menu81->Portrait->Target,(X+390)*Scale,(Y+1)*Scale,283*Scale,(CH-2)*Scale,0,0,1,1,FLinearColor::White,BLEND_Opaque);
   for(int I=0;I<64;I++)DrawRect(FLinearColor(.015,.032,.041,.96f*(1-I/64.f)),(X+390+I*1.125f)*Scale,(Y+1)*Scale,1.2f*Scale,(CH-2)*Scale);
   Line(X+CW-1,Y+1,X+CW-1,Y+CH-1,FLinearColor(.2,.32,.36,.65));
  }
  Text(TEXT("Continue"),X+30,Y+27,2.45f,Paper81);
  Text(Fit81(*this,Menu81->Identity.Name,342,1.17f),X+32,Y+86,1.17f,Dim81);
  Text(FString::Printf(TEXT("LEVEL %d"),Menu81->Level),X+32,Y+130,.88f,Gold81);
  Text(FString::Printf(TEXT("CR %s"),*FText::AsNumber(Menu81->Credits).ToString()),X+166,Y+130,.99f,Paper81);
  if(Menu81->CanadianDollars>0)Text(FString::Printf(TEXT("CA$ %s"),*FText::AsNumber(Menu81->CanadianDollars).ToString()),X+166,Y+157,.88f,Dim81);
  Line(X+32,Y+192,X+362,Y+192,FLinearColor(.29,.41,.44,.6));
  Text(Fit81(*this,Menu81->Place,332,1.17f),X+32,Y+211,1.17f,Paper81);
  Text(TEXT("ACTIVE MISSION"),X+32,Y+254,.74f,Gold81);
  Wrap81(*this,Menu81->Mission,X+32,Y+279,332,1.05f,Paper81,2);
 }else{
  Text(TEXT("Continue"),X+32,Y+30,2.45f,Dim81);
  Text(Menu81->Loading?TEXT("Reading save..."):Menu81->Error.IsEmpty()?TEXT("No saved survivor"):TEXT("Save unavailable"),X+32,Y+126,1.5f,Paper81);
  if(!Menu81->Loading)Wrap81(*this,Menu81->Error.IsEmpty()?TEXT("Start a new survivor to begin."):Menu81->Error,X+32,Y+177,540,1.1f,Dim81);
 }
 const float RX=X+692,TW=230,TH=170;
 MenuTile81(TEXT("New"),TEXT("New survivor"),RX,Y,TW,TH,0);
 MenuTile81(TEXT("Load62"),TEXT("Load game"),RX+248,Y,TW,TH,1);
 MenuTile81(TEXT("Settings"),TEXT("Settings"),RX,Y+186,TW,TH,2);
 MenuTile81(TEXT("news81"),TEXT("What's new"),RX+248,Y+186,TW,TH,3);
 Control55(TEXT("Quit"),TEXT("Quit game"),X+1010,640,160,41);
 Text(TEXT("ALL AMERICAN MELTDOWN"),X,651,.73f,Dim81);
}

bool ALWHUD::MenuClick81(ALWCharacter* P,FName Id){
 if(P->bStarted||P->bSettings||P->bWorldSetup||P->OpeningMode||P->SavePanel62)return false;
 if(News81){
  if(Id==TEXT("news81_back"))News81=false;
  else if(Id.ToString().StartsWith(TEXT("release81_")))Release81=FMath::Clamp(FCString::Atoi(*Id.ToString().Mid(10)),0,LWMenu81::Releases().Num()-1);
  return true;
 }
 if(Id==TEXT("news81")){News81=true;Release81=0;return true;}
 if(Id==TEXT("Start")){
  if(Menu81&&Menu81->Ready){
   // Load exactly the record displayed, even if timestamps change while the menu is open.
   const FString Slot=Menu81->Slot;Menu81->StopPortrait();
   if(!P->LoadSlot62(Slot)){Menu81->Ready=false;Menu81->Error=TEXT("Save unavailable. Choose Load game.");}
  }
  return true;
 }
 return false;
}
