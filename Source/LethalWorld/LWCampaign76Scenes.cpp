#include "LWCampaign76.h"
#include "LWCampaignProduction77.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWDialogue59.h"
#include "LWNPCLife.h"
#include "LWHUD.h"
#include "Components/AudioComponent.h"
#include "Engine/Canvas.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
namespace {FString Wrap76(const FString& S,int Width){TArray<FString> Words;S.ParseIntoArrayWS(Words);FString Out;int Col=0;for(const FString& W:Words){if(Col&&Col+W.Len()+1>Width){Out+=TEXT("\n");Col=0;}if(Col){Out+=TEXT(" ");Col++;}Out+=W;Col+=W.Len();}return Out;}}
void ALWCampaign76::Show(FName Id,FName Afterwards){
 const auto* S=LWCampaign76::Scene(Id);if(!S)return;
 const bool Restore=State().Scene==Id;Pause();Player->ClosePanels();Player->CancelReload();Player->StopAttack();
 if(!Restore)State().Beat=0;State().Scene=Id;State().AfterScene=Afterwards;SceneOpen=true;Page=0;
 Player->bStoryLocked=true;Player->SetMenuInput(true);Player->GetCharacterMovement()->StopMovementImmediately();ShowBeat();Player->RequestSave40();
}
void ALWCampaign76::ShowBeat(){
 const auto* S=LWCampaign76::Scene(State().Scene);if(!S){FinishScene();return;}
 ShownChoices.Empty();Sync();
 while(S->Beats.IsValidIndex(State().Beat)){
  const auto& B=S->Beats[State().Beat];if(!LWCampaign76::Meets(State(),B.Needs)||(!B.Who.IsNone()&&(!CastAvailable(B.Who)||!People.Contains(B.Who)))){State().Beat++;continue;}
  Speech=B.Text;const auto* Def=LWCampaign76::Person(B.Who);SpeakerName=Def?Def->Name:FString();BeatWait=B.Pause;SceneClock=0;
  if(auto* N=People.FindRef(B.Who).Get();IsValid(N)&&!N->bDead){
   N->Subtitle=B.Text;N->SubtitleTime=FMath::Max(5.f,B.Text.Len()*.055f);
   // Missing performances remain silent, fully subtitled, and facially staged. No gibberish or paid generation.
   auto* Voice=ULWDialogue59::Channel(N);if(ULWDialogue59::Available())Voice->Say(N->ResidentId,N->Appearance35.Body==1,B.Text,true);
   if(N->LifeAnimation){N->LifeAnimation->Speak(nullptr,B.Text);N->LifeAnimation->Gesture=B.Gesture==TEXT("stop")?2:B.Gesture==TEXT("point")?1:0;}
   if(!B.Move.IsNearlyZero()){N->Mark=At(B.Move+FVector(0,0,92));N->MoveTime=5;}
  }
  if(SceneSound){SceneSound->Stop();SceneSound=nullptr;}if(!B.Cue.IsNone())SceneSound=World->Sound(B.Cue,At(FVector(0,0,100)),.25f);
  if(Production)Production->Beat(S->Id,State().Beat);
  return;
 }
 for(int I=0;I<S->Choices.Num();I++)if(LWCampaign76::Meets(State(),S->Choices[I].Needs))ShownChoices.Add(I);
 BeatWait=0;SceneClock=0;
 if(ShownChoices.IsEmpty())FinishScene();
}
void ALWCampaign76::AdvanceBeat(){if(!SceneOpen||BeatWait>0||!ShownChoices.IsEmpty())return;State().Beat++;ShowBeat();Player->RequestSave40();}
void ALWCampaign76::FinishScene(){
 const auto* S=LWCampaign76::Scene(State().Scene);const FName Next=!State().AfterScene.IsNone()?State().AfterScene:(S?S->Next:NAME_None);
 Pause();State().Scene=State().AfterScene=NAME_None;State().Beat=0;State().Revision++;
 if(!Next.IsNone())SetStage(Next);else {Refresh();Route();Player->CaptureMission37(TEXT("campaign76"));}
 Player->RequestSave40();
}
bool ALWCampaign76::Choose(int I){
 if(!SceneOpen||!ShownChoices.IsValidIndex(I))return false;const auto* S=LWCampaign76::Scene(State().Scene);if(!S)return false;
 Sync();const auto C=S->Choices[ShownChoices[I]];if(!LWCampaign76::Meets(State(),C.Needs)){ShowBeat();return true;}
 for(FString E:C.Effects)if(E.RemoveFromStart(TEXT("spend:"))&&Player->Money<FCString::Atoi(*E)){ShowBeat();return true;}
 const FName Decision(*(TEXT("choice_")+S->Id.ToString()+TEXT("_")+FString::FromInt(ShownChoices[I])));
 if(!State().Rewards.Contains(Decision)&&!CanApply(C.Effects))return true;
 Effects(C.Effects,Decision);State().Decisions.Add(Decision);
 if(!C.Next.IsNone()){const FName Afterwards=C.Stage.IsNone()?State().AfterScene:C.Stage;State().Scene=NAME_None;Show(C.Next,Afterwards);}
 else if(!C.Stage.IsNone()){Pause();State().Scene=State().AfterScene=NAME_None;SetStage(C.Stage);}
 else FinishScene();return true;
}
void ALWCampaign76::Pause(){
 if(!SceneOpen)return;SceneOpen=false;if(Player){Player->bStoryLocked=false;Player->bTrigger=false;Player->SetMenuInput(Player->bMenu);}
 if(SceneSound){SceneSound->Stop();SceneSound=nullptr;}for(auto& P:People)if(IsValid(P.Value))ULWDialogue59::Channel(P.Value)->Stop();
}
void ALWCampaign76::Resume(){if(!State().Started){Begin(true);return;}State().Paused=false;SetActorTickEnabled(true);Spawn();if(!State().Scene.IsNone()&&SpawnedStage==State().Stage&&FVector::Dist(Player->GetActorLocation(),At(FVector(0,0,92)))<750)Show(State().Scene,State().AfterScene);else Route();}
bool ALWCampaign76::Click(FName Id){
 const FString Name=Id.ToString();if(!Name.StartsWith(TEXT("c76_")))return false;
 if(Id==TEXT("c76_begin")){Player->ClosePanels();Begin(true);}
 else if(Id==TEXT("c76_next"))AdvanceBeat();else if(Id==TEXT("c76_leave")){Pause();State().Paused=true;Player->RequestSave40();}
 else if(Id==TEXT("c76_resume")){Player->ClosePanels();Resume();}else if(Id==TEXT("c76_route"))Route();
 else if(Name.StartsWith(TEXT("c76_choice_")))Choose(FCString::Atoi(*Name.Mid(11)));return true;
}
bool ALWCampaign76::Screen(ALWHUD& H){
 if(!Player||Player->bMenu||Player->OpeningMode)return false;
 const auto* Stage=LWCampaign76::Stage(State().Stage);auto* Canvas=H.GetCanvas45();if(!Canvas)return false;
 const float W=Canvas->ClipX/H.Scale;
 if(!SceneOpen){
  if(State().Started&&!State().Paused&&!Player->IsUIOpen()&&Stage){
   FVector2D Mark;if(H.PlayerOwner->ProjectWorldLocationToScreen(Goal(),Mark)){Mark=(Mark-H.Origin55)/H.Scale;if(Mark.X>25&&Mark.X<W-200&&Mark.Y>180&&Mark.Y<480)H.Text(FString::Printf(TEXT("+ %dm"),FMath::RoundToInt(FVector::Dist(Player->GetActorLocation(),Goal())/100)),Mark.X,Mark.Y,.85f,FLinearColor(1,.72,.35));}
   if(!WorkAction.IsNone())H.Text(TEXT("Working... ")+FString::Printf(TEXT("%.1fs"),WorkSeconds),W*.5f-90,430,.85f);
  }return false;
 }
 // Retains the player's camera and visible blocking; only the lower dialogue area is covered.
 H.Rect(42,430,W-84,270,FLinearColor(.009,.015,.018,.94));H.Text(SpeakerName,66,446,1.2,FLinearColor(1,.72,.4));
 H.Text(Wrap76(Speech,FMath::Max(56,int((W-132)/9))),66,477,.93f);
 if(ShownChoices.IsEmpty())H.Button(TEXT("c76_next"),BeatWait>0?TEXT("..."):TEXT("CONTINUE"),W-310,644,240);
 else for(int I=0;I<ShownChoices.Num();I++){
  const auto* S=LWCampaign76::Scene(State().Scene);const float Y=202+I*44;
  H.Control55(FName(*FString::Printf(TEXT("c76_choice_%d"),I)),S->Choices[ShownChoices[I]].Text,66,Y,W-132,40);
 }
 H.Button(TEXT("c76_leave"),TEXT("PAUSE CONVERSATION"),66,644,290);return true;
}
void ALWCampaign76::Journal(ALWHUD& H){
 const float W=H.GetCanvas45()->ClipX/H.Scale;H.Text(TEXT("THE COUNTRY WE LEFT BEHIND"),45,188,1.35f,FLinearColor(1,.72,.4));
 if(!State().Started){H.Text(TEXT("Meet Mara Finch at Bellwether, outside Syracuse."),45,245);H.Text(TEXT("Starting the campaign takes you to Bellwether. Your survivor and equipment are retained."),45,280,.85f);H.Button(TEXT("c76_begin"),TEXT("START AT BELLWETHER"),45,510,420);return;}
 if(const auto* S=LWCampaign76::Stage(State().Stage)){H.Text(S->Title,45,234,1.1);H.Text(Wrap76(S->Goal,100),45,270,.95f);}
 int Row=0;for(int I=FMath::Max(0,State().Journal.Num()-4);I<State().Journal.Num();I++)H.Text(Wrap76(State().Journal[I],102),45,330+(Row++)*49,.8f);
 H.Button(TEXT("c76_route"),TEXT("TRACK OBJECTIVE"),45,572,300);H.Button(TEXT("c76_resume"),State().Paused?TEXT("RESUME CAMPAIGN"):TEXT("RESUME CONVERSATION"),365,572,350);
 if(!State().Ending.IsNone())H.Text(State().Ending.ToString(),45,636,1.1f);
}
