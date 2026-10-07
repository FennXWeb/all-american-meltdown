#include "LWCampaign76.h"
#include "LWHUD.h"
#include "LWStory.h"
#include "LWCharacter.h"
#include "Engine/Canvas.h"
#include "GameFramework/PlayerController.h"
namespace {FString StoryWrap(const FString& S,int Width){FString Out;int Col=0;TArray<FString> Words;S.ParseIntoArray(Words,TEXT(" "),true);for(const auto& W:Words){if(Col+W.Len()+1>Width){Out+=TEXT("\n");Col=0;}if(Col){Out+=TEXT(" ");Col++;}Out+=W;Col+=W.Len();}return Out;}}
bool ALWHUD::StoryScreen(ALWCharacter* P){if(P->Campaign76&&P->Campaign76->Screen(*this))return true;auto* D=P->Story.Get();if(!D||P->bMenu||P->OpeningMode)return false;if(!D->Locked()){if(P->RPG.Story.Enabled&&P->RPG.Story.Stage<29&&!P->IsUIOpen()){FVector2D Mark;FVector Goal=D->Target();if(PlayerOwner->ProjectWorldLocationToScreen(Goal+FVector(0,0,100),Mark)){Mark=(Mark-Origin55)/Scale;if(Mark.X>30&&Mark.X<Canvas->ClipX/Scale-180&&Mark.Y>180&&Mark.Y<500){Text(TEXT("+")+FString::Printf(TEXT(" %dm"),FMath::RoundToInt(FVector::Dist(P->GetActorLocation(),Goal)/100)),Mark.X,Mark.Y,.85f,FLinearColor(.95,.65,.25));}}}return false;}float W=Canvas->ClipX/Scale;
 Rect(0,0,W,75,FLinearColor(0,0,0,.95));Rect(0,490,W,230,FLinearColor(0,0,0,.92));
 Text(D->VoiceName,60,510,1,FLinearColor(.93,.66,.32));Text(StoryWrap(D->Speech,FMath::Max(50,int((W-120)/9))),60,544,1);
 if(D->InScene)Button(TEXT("story_skip"),TEXT("SKIP SCENE"),W-270,20,230);
 if(D->Failed){Rect(0,0,W,490,FLinearColor(0,0,0,.8));Text(TEXT("A PROMISE IS NOT A PARDON"),60,260,2);Button(TEXT("story_choice_0"),TEXT("RETRY AT THE ULTIMATUM"),60,390,600);}
 else if(D->Choosing){Rect(40,200,W-80,250,FLinearColor(0,0,0,.84));for(int I=0;I<D->Choices.Num();I++)Button(FName(*FString::Printf(TEXT("story_choice_%d"),I)),D->Choices[I],60,220+I*58,W-120);}
 return true;
}
void ALWHUD::StoryJournal(ALWCharacter* P){float W=Canvas->ClipX/Scale;const auto& S=P->RPG.Story;const auto& M=LWStory::Missions()[FMath::Clamp(S.Stage,0,29)];
 Text(TEXT("CHAPTER 1 / THE GATES STAY OPEN"),45,185,1.4f,FLinearColor(.92,.66,.32));Text(M.Title,45,230,1.2f);Text(StoryWrap(M.Objective,80),45,268,1);
 Text(FString::Printf(TEXT("OBJECTIVES COMPLETED: %d / 29"),S.Paid.Num()),45,322,.9f);Rect(45,352,W-100,6,FLinearColor(.12,.16,.12));Rect(45,352,(W-100)*S.Stage/29.f,6,FLinearColor(.8,.6,.3));
 Text(S.Stage<9?TEXT("Mara Vale offers a way back into the world. Help her protect Mercy Crossing."):S.Stage<20?TEXT("The Cinder Choir took Mara. Follow the transfers. Bring her home."):S.Stage<27?TEXT("Liberty's Heroes call captivity citizenship. Get your people out."):TEXT("The fortress belongs to the people who survived it. Decide what comes next."),45,389,.85f);
 if(S.GearHeld)Text(TEXT("CONFISCATED EQUIPMENT: SECURED IN EVIDENCE STORAGE"),45,440,.85f);
 Button(TEXT("story_route"),TEXT("TRACK CURRENT OBJECTIVE"),45,510,470);
 Text(TEXT("Progress, evidence and rescued allies are saved automatically."),45,580,.85f);
}
