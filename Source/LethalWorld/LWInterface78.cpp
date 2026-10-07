#include "LWHUD.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWGraphics33.h"
#include "LWLighting.h"
#include "LWPlayerInput51.h"
#include "Engine/Canvas.h"
#include "Engine/World.h"

namespace {const FLinearColor Paper(.91,.93,.91),Dim(.52,.64,.67),Accent(.98,.59,.28),Panel(.009,.017,.023,.96);}

void ALWHUD::Menu78(ALWCharacter* P){
 const float W=Canvas->ClipX/Scale;
 if(P->bStarted)Rect(0,0,W,720,FLinearColor(.006,.012,.018,.58));
 // The menu is anchored to the safe left edge; artwork/world remains full-width on ultrawide.
 for(int I=0;I<24;I++)DrawRect(FLinearColor(.005,.012,.019,.93f*(1-I/24.f)),I*28*Scale,0,29*Scale,720*Scale);
 Text(P->bStarted?TEXT("PAUSED"):TEXT("ALL AMERICAN MELTDOWN"),56,38,.78f,Dim);
 Logo(42,66,490,236,!P->bStarted&&!ReducedMotion55);
 Line(56,322,442,322,FLinearColor(.52,.66,.68,.3));
 float Y=349;
 auto Entry=[&](FName Id,const TCHAR* Name,bool Selected=false){Control55(Id,Name,56,Y,390,46,Selected);Y+=52;};
 Entry(TEXT("Start"),P->bStarted?TEXT("Resume"):TEXT("Continue"));
 Entry(TEXT("New"),TEXT("New survivor"));Entry(TEXT("Load62"),TEXT("Load game"));
 if(P->bStarted)Entry(TEXT("Save62"),TEXT("Save game"));
 Entry(TEXT("Settings"),TEXT("Settings"));Entry(TEXT("Quit"),TEXT("Quit"));
 if(P->bStarted){
  const float X=W-336;Rect(X,532,280,132,Panel);
  Text(P->Identity.Name,X+22,551,1.1,Paper);
  Text(FString::Printf(TEXT("LEVEL %d"),P->RPG.Level),X+22,582,.8,Accent);
  Text(FString::Printf(TEXT("CR %lld"),P->Money),X+22,609,.85,Dim);
  if(P->World)Text(FString::Printf(TEXT("DAY %d  /  %02d:%02d"),P->World->DayNumber,FMath::FloorToInt(P->World->TimeOfDay),FMath::FloorToInt(FMath::Frac(P->World->TimeOfDay)*60)),X+22,634,.78,Dim);
 }
}

void ALWHUD::NewSurvivor78(ALWCharacter* P){
 const float W=Canvas->ClipX/Scale;Rect(0,0,W,720,FLinearColor(.006,.012,.019,.86));
 const float X=(W-660)*.5f;Rect(X,156,660,404,Panel);
 Text(TEXT("New survivor"),X+36,193,2.25,Paper);
 Line(X+36,251,X+624,251,FLinearColor(.25,.36,.39,.6));
 const TCHAR* Names[]={TEXT("Easy"),TEXT("Normal"),TEXT("Hard")};
 Control55(TEXT("difficulty"),FString(TEXT("Difficulty   /   "))+Names[FMath::Clamp(P->SetupDifficulty,0,2)],X+36,291,588,52);
 const TCHAR* Descriptions[]={TEXT("Reduced damage and survival needs."),TEXT("Standard damage and survival needs."),TEXT("More enemies, higher damage and survival needs.")};
 Text(Descriptions[FMath::Clamp(P->SetupDifficulty,0,2)],X+36,365,.87,Dim);
 Control55(TEXT("setupback"),TEXT("Back"),X+36,466,180,48);
 Control55(TEXT("createworld"),TEXT("Create character"),X+240,466,384,48,true);
}

void ALWHUD::Settings78(ALWCharacter* P){
 const float W=Canvas->ClipX/Scale;Rect(0,0,W,720,FLinearColor(.006,.012,.019,.98));
 Text(TEXT("Settings"),48,37,2.6,Paper);Control55(TEXT("Settings"),TEXT("Back"),1072,35,160,43);
 if(KeysPage51>=0){KeysScreen51();return;}
 const TCHAR* Names[]={TEXT("Display"),TEXT("Audio / input"),TEXT("Quality"),TEXT("Effects"),TEXT("Performance"),TEXT("Controller"),TEXT("Interface")};
 for(int I=0;I<7;I++)Control55(FName(*FString::Printf(TEXT("settings78_%d"),I)),Names[I],48+I*170,105,160,38,I==SettingsSection78);
 const int Section=FMath::Clamp(SettingsSection78,0,6);
 Rect(48,174,1184,462,Panel);
 if(Section==0){
  Text(TEXT("Video output"),72,193,1.25,Paper);
  Button(TEXT("Resolution"),FString::Printf(TEXT("Resolution    %d x %d"),P->PendingResolution.X,P->PendingResolution.Y),72,245,532);
  Button(TEXT("WindowMode"),TEXT("Window mode    ")+P->WindowModeName(),72,302,532);
  Button(TEXT("ApplyVideo"),TEXT("Apply display changes"),72,379,532);
  Text(TEXT("Borderless uses desktop resolution."),72,449,.85,Dim);
  Text(TEXT("Lighting"),664,193,1.25,Paper);
  Button(TEXT("LightingQuality"),LWLighting::Label(),664,245,540);
  Text(LWLighting::HardwareAvailable()?TEXT("Hardware ray tracing available"):TEXT("Hardware ray tracing unavailable"),664,305,.82,Dim);
  Button(TEXT("Crust"),P->bCrust?TEXT("Retro camera filter    On"):TEXT("Retro camera filter    Off"),664,379,540);
 }else if(Section==1){
  Text(TEXT("Mouse"),72,193,1.25,Paper);Text(FString::Printf(TEXT("Sensitivity    %.3f"),P->Sensitivity),72,245,1,Accent);
  SettingsSlider55(P,0,72,306,532);Button(TEXT("SensitivityDown"),TEXT("Decrease"),72,344,254);Button(TEXT("Sensitivity"),TEXT("Increase"),350,344,254);
  Button(TEXT("keys51"),TEXT("Customize key bindings"),72,432,532);
  Text(TEXT("Audio"),664,193,1.25,Paper);Button(TEXT("Volume"),FString::Printf(TEXT("Master volume    %d%%"),FMath::RoundToInt(P->MasterVolume*100)),664,245,540);SettingsSlider55(P,1,664,306,540);
 }else if(Section>=2&&Section<=5){
  const int Quality[]={10,0,1,2,3,4,5,6,7,8,9,22},Display[]={11,12,13,14,15,16,17,18,19,20,21};
  const int Count=Section==2?12:Section==3||Section==4?11:7;
  for(int Row=0;Row<Count;Row++){
   float X=72+(Row/6)*592,Y=202+(Row%6)*57;
   if(Section==5&&Row<4){if(auto* K=ULWPlayerInput51::Get51(this))Button(FName(*FString::Printf(TEXT("pad58_%d"),Row)),K->Label58(Row),X,Y,540);continue;}
   const int I=Section==2?Quality[Row]:Section==3?Display[Row]:Section==4?23+Row:34+Row-4;
   Button(FName(*FString::Printf(TEXT("gfx_%d"),I)),LWGraphics33::Label(I),X,Y,540);
  }
  if(Section>=4)Text(LWGraphics33::Status58(),72,576,.78,Dim);
 }else{
  Text(TEXT("Accessibility"),72,193,1.25,Paper);
  Button(TEXT("ui_motion55"),ReducedMotion55?TEXT("Menu animation    Reduced"):TEXT("Menu animation    Full"),72,254,540);
  Button(TEXT("ui_contrast55"),HighContrast55?TEXT("Contrast    High"):TEXT("Contrast    Standard"),664,254,540);
 }
 if(P->bVideoConfirm){
  Rect(0,0,W,720,FLinearColor(0,0,0,.75));Rect(350,208,580,316,Panel);
  Text(TEXT("Keep this display mode?"),382,246,1.55,Paper);
  Text(FString::Printf(TEXT("Reverting in %d seconds"),FMath::Max(0,FMath::CeilToInt(P->VideoConfirmTimer-GetWorld()->GetRealTimeSeconds()))),382,297,.95,Accent);
  Button(TEXT("ConfirmVideo"),TEXT("Keep changes"),382,357,516);Button(TEXT("RevertVideo"),TEXT("Revert"),382,422,516);
 }
}
