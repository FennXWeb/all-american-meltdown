#include "LWHUD.h"
#include "LWCharacter.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
namespace {const FLinearColor OpeningBone57(.83,.81,.7),OpeningMuted57(.46,.48,.4),OpeningAmber57(.85,.48,.2),OpeningInk57(.018,.026,.02,.95);}
void ALWHUD::Logo(float X,float Y,float W,float H,bool Animate){if(!TitleLogo)TitleLogo=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/Textures/T_AllAmericanMeltdownAlpha.T_AllAmericanMeltdownAlpha"));if(TitleLogo){const float Ratio=float(TitleLogo->GetSizeX())/TitleLogo->GetSizeY();float RW=FMath::Min(W,H*Ratio),RH=RW/Ratio; const float T=GetWorld()->GetRealTimeSeconds(); if(Animate){Y+=FMath::Sin(T*.8f)*2; for(int I=0;I<24;I++){float Life=FMath::Fmod(T*.19f+I*.618f,1.f),PX=X+W*(.12f+FMath::Fmod(I*.377f,.76f))+FMath::Sin(T+I)*8,PY=Y+H*(.85f-Life*.8f);Rect(PX,PY,1.5f,3+Life*3,FLinearColor(1,.24f,.025f,FMath::Sin(Life*PI)*.55f));}}for(int I=0;I<(Animate?64:1);I++){float N=Animate?64.f:1.f,V=I/N,Offset=Animate?FMath::Sin(T*2.4f+V*22)*.7f*V:0;DrawTexture(TitleLogo,(X+(W-RW)*.5+Offset)*Scale,(Y+(H-RH)*.5+RH*V)*Scale,RW*Scale,RH/N*Scale,0,V,1,1/N,FLinearColor::White,BLEND_Translucent);}}else {Text(TEXT("ALL AMERICAN"),X,Y,2,OpeningBone57);Text(TEXT("MELTDOWN"),X,Y+38,3,OpeningAmber57);}}
void ALWHUD::OpeningScreen(ALWCharacter* P){
 P->UpdateOpening();const float W=Canvas->ClipX/Scale;
 if(P->OpeningMode==1){float Age=P->OpeningPaused?P->OpeningSince:GetWorld()->GetRealTimeSeconds()-P->OpeningSince;const auto& B=LWOpening::Beats()[P->OpeningShot];Rect(0,0,W,88,FLinearColor(0,0,0,1));Rect(0,548,W,172,FLinearColor(0,0,0,1));Text(B.Title,48,32,1.2,OpeningAmber57);Text(B.Lines[FMath::Clamp(int(Age/6),0,2)],W*.5-410,574,1.05,OpeningBone57);
 float Fade=FMath::Max(1-FMath::Clamp(Age/1.2f,0.f,1.f),FMath::Clamp((Age-16.8f)/1.2f,0.f,1.f));if(Fade>0)Rect(0,88,W,460,FLinearColor(0,0,0,Fade));if(P->OpeningShot==4&&Age>1.5&&Age<2.1)Rect(0,88,W,460,FLinearColor(1,.9,.7,(2.1-Age)*1.5));
 Text(FString::Printf(TEXT("%02d / %02d"),P->OpeningShot+1,LWOpening::Beats().Num()),48,680,.8,OpeningMuted57);Button(TEXT("intro_pause"),P->OpeningPaused?TEXT("RESUME"):TEXT("PAUSE"),W-455,664,170);Button(TEXT("intro_skip"),TEXT("SKIP INTRO"),W-270,664,220);return;}
 if(P->OpeningMode!=2)return;
 CreatorScreen35(P);
}
