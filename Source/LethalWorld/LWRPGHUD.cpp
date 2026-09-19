#include "LWHUD.h"
#include "LWStory.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWEncounter.h"
#include "LWWorld.h"
#include "Engine/Canvas.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Camera/CameraComponent.h"
namespace {FString Wrap(FString S,int W){TArray<FString> Words;S.ParseIntoArrayWS(Words);FString Out;int N=0;for(const auto& Word:Words){if(N+Word.Len()+1>W){Out+=TEXT("\n");N=0;}else if(N){Out+=TEXT(" ");N++;}Out+=Word;N+=Word.Len();}return Out;}}
void ALWHUD::RPGScreen(ALWCharacter* P){
 const float W=Canvas->SizeX/Scale;if(P->RPGPanel==4&&(P->Speaker||P->EncounterSpeaker||P->RestBed)){
  if(DialogueSeen55!=P->DialogueText){DialogueSeen55=P->DialogueText;DialoguePage55=0;}
  DialoguePage55=FMath::Clamp(DialoguePage55,0,FMath::Max(0,(P->DialogueChoices.Num()-1)/5));
  Rect(0,0,W,720,FLinearColor(.005,.009,.012,.28));
  Rect(72,284,W-144,410,FLinearColor(.012,.02,.024,.96));
  const FString Name=P->EncounterSpeaker&&P->EncounterSpeaker->Definition()?P->EncounterSpeaker->Definition()->Title:P->Speaker?P->Speaker->DisplayName:FString(TEXT("REST"));
  Text(Name,98,306,1.55f,FLinearColor(.96,.68,.34));Button(TEXT("rpg_close"),TEXT("LEAVE"),W-252,301,150);
  Text(Wrap(P->DialogueText,112),98,355,.9f,FLinearColor(.89,.92,.87));
  for(int I=DialoguePage55*5;I<FMath::Min(P->DialogueChoices.Num(),DialoguePage55*5+5);++I)Control55(FName(*FString::Printf(TEXT("dialogue_%d"),I)),P->DialogueChoices[I],98,435+(I%5)*44,W-196,39);
  if(P->DialogueChoices.Num()>5){Button(TEXT("dialoguepage55"),TEXT("MORE RESPONSES"),98,658,250);}
  return;
 }
 Rect(20,65,W-40,620,FLinearColor(.02f,.035f,.025f,.98f));
 Text(FString::Printf(TEXT("LEVEL %02d   XP %lld / %lld   POINTS %d"),P->RPG.Level,P->RPG.XP,LWRPG::Threshold(P->RPG.Level),P->RPG.Points),45,85,1.15f);
 if(P->RPGPanel==4)Button(TEXT("rpg_close"),TEXT("CLOSE"),W-200,78,150);
 if(P->RPGPanel==1){
 for(int C=0;C<7;C++)Button(FName(*FString::Printf(TEXT("cat_%d"),C)),FString::Printf(TEXT("%s %d"),LWRPG::Category(C),P->RPG.Attributes[C]),40,145+C*62,270);
 Button(TEXT("attribute"),TEXT("ATTRIBUTE +1"),40,594,270);
 int I=0;for(const auto& Skill:ULWRPGCatalog::Get()->Perks)if(Skill.Category==P->SkillCategory){int Row=I++;if(Row/7!=P->SkillPage)continue;float Y=143+(Row%7)*72;int Rank=P->RPG.Perks.FindRef(Skill.Id);Text(Skill.Name,335,Y,1.05f);Text(Skill.Description,335,Y+23,.8f);Text(FString::Printf(TEXT("REQ ATTRIBUTE %d / LEVEL %d"),Skill.AttributeRequired,Skill.LevelRequired+Rank*3),335,Y+42,.8f,FLinearColor(.55f,.62f,.48f));Button(FName(*(TEXT("buy_")+Skill.Id.ToString())),FString::Printf(TEXT("%d/%d +"),Rank,Skill.MaxRank),W-180,Y+4,130);}
 if(I>7)Button(TEXT("skill_next"),FString::Printf(TEXT("PAGE %d / %d"),P->SkillPage+1,(I+6)/7),335,650,240);
 }else if(P->RPGPanel==2){
 Button(TEXT("encounter_journal"),P->EncounterJournal?TEXT("SHOW CONTRACTS"):TEXT("ROAD JOURNAL"),W-340,130,290);
 if(P->EncounterJournal){
 Text(FString::Printf(TEXT("ROAD JOURNAL / REPUTATION %d"),P->World->Encounters.Reputation),45,139,.9f);
 TArray<FLWEncounterRecord> Rows;for(const auto& E:P->World->Encounters.Records)if(E.Value.Seen)Rows.Add(E.Value);Rows.Sort([](const auto& A,const auto& B){return A.Created>B.Created;});
 int Pages=FMath::Max(1,(Rows.Num()+4)/5);P->JournalPage=FMath::Clamp(P->JournalPage,0,Pages-1);
 for(int I=P->JournalPage*5;I<FMath::Min(Rows.Num(),P->JournalPage*5+5);I++){const auto& R=Rows[I];const auto* D=ULWEncounterCatalog::Get()->Find(R.Type);if(!D)continue;float Y=188+(I%5)*82;Text(D->Title,45,Y,1);FString Status=R.Stage==0?TEXT("UNRESOLVED"):R.Stage==1?TEXT("IN PROGRESS"):R.Stage==2?TEXT("COMPLETED"):TEXT("LEFT BEHIND");Text(Status,45,Y+23,.75f);Text(Wrap(R.Stage>=2?R.Result:D->Intro,70).Left(85),45,Y+43,.7f);Button(FName(*(TEXT("eventtrack_")+R.Id.ToString())),TEXT("ROUTE"),W-210,Y,160);}
 if(Rows.IsEmpty())Text(TEXT("ENCOUNTERS YOU DISCOVER WILL APPEAR HERE."),45,230,.9f);
 Button(TEXT("encounter_next"),FString::Printf(TEXT("NEXT / %d OF %d"),P->JournalPage+1,Pages),45,615,280);return;
 }
 if(!P->RPG.Story.Enabled)Button(TEXT("story_start"),P->RPG.Story.Stage>0?TEXT("RESUME CHAPTER 1"):TEXT("BEGIN CHAPTER 1"),45,130,290);
 if(P->RPG.Story.Enabled){Button(TEXT("story_journal_toggle"),P->ChapterJournal?TEXT("SIDE CONTRACTS"):TEXT("MAIN STORY"),45,130,290);if(P->ChapterJournal){StoryJournal(P);return;}}
 Text(TEXT("CONTRACTS"),45,137,.85f);
 int Start=P->JournalPage*5;for(int I=Start;I<FMath::Min(Start+5,P->RPG.Quests.Num());I++){const auto& S=P->RPG.Quests[I];const auto* Q=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& X){return X.Id==S.Id;});if(!Q)continue;float Y=184+(I-Start)*83;
 Text(Q->Title+(S.Rewarded?TEXT(" // PAID"):S.Stage>=Q->Objectives.Num()?TEXT(" // RETURN TO WARDEN"):TEXT(" // ACTIVE")),45,Y,1);
 if(Q->Objectives.IsValidIndex(S.Stage)){const auto& O=Q->Objectives[S.Stage];int Progress=O.Event==TEXT("deliver")?P->CountSupply(O.Target):S.Progress;Text(O.Text+FString::Printf(TEXT(" [%d/%d]"),Progress,O.Count),45,Y+28,.85f);}
 Button(FName(*(TEXT("track_")+S.Id.ToString())),P->RPG.TrackedQuest==S.Id?TEXT("TRACKED"):TEXT("TRACK"),W-220,Y,170);}
 if(P->RPG.Quests.IsEmpty())Text(TEXT("SPEAK TO A SETTLEMENT WARDEN TO ACCEPT CONTRACTS."),45,220,1);
 Button(TEXT("journal_next"),TEXT("NEXT PAGE"),45,615,250);
 }else if(P->RPGPanel==3){
 int Active=0;for(const auto& C:P->RPG.Crew)Active+=C.Following;Text(FString::Printf(TEXT("SQUAD %d/%d // RESIDENTS %d"),Active,P->CompanionLimit(),P->RPG.Crew.Num()),45,139,1);
 CrewPage55=FMath::Clamp(CrewPage55,0,FMath::Max(0,(P->RPG.Crew.Num()-1)/9));for(int I=CrewPage55*9;I<FMath::Min(P->RPG.Crew.Num(),CrewPage55*9+9);I++){const auto& C=P->RPG.Crew[I];float Y=185+(I%9)*45;Text(FString::Printf(TEXT("%02d %s // %s"),C.Bedroom+1,*C.Name,C.Following?TEXT("FOLLOWING"):!C.Station.IsNone()?TEXT("STATIONED"):TEXT("AT BUNKER")),45,Y+8,.9f);Button(FName(*(TEXT("crew_")+C.Id.ToString())),C.Following?TEXT("SEND HOME"):TEXT("TAKE ALONG"),W-260,Y,210);}
 Button(TEXT("crewpage55_prev"),TEXT("PREVIOUS"),45,626,200);Button(TEXT("crewpage55_next"),FString::Printf(TEXT("PAGE %d / %d  NEXT"),CrewPage55+1,FMath::Max(1,(P->RPG.Crew.Num()+8)/9)),270,626,285);
 if(P->RPG.Crew.IsEmpty())Text(TEXT("HIRE SURVIVORS AT A FRIENDLY WAYSTATION."),45,220,1);
 }else if(P->RPGPanel==4&&(P->Speaker||P->EncounterSpeaker||P->RestBed)){
 Text(P->EncounterSpeaker&&P->EncounterSpeaker->Definition()?P->EncounterSpeaker->Definition()->Title:P->Speaker?P->Speaker->DisplayName:FString(TEXT("BED")),45,139,1.6f,FLinearColor(.9f,.65f,.3f));Text(Wrap(P->DialogueText,FMath::Max(40,int((W-120)/9))),45,187,1);
 for(int I=0;I<P->DialogueChoices.Num();I++)Button(FName(*FString::Printf(TEXT("dialogue_%d"),I)),P->DialogueChoices[I],45,325+I*45,W-90);
 }
}
bool ALWHUD::RPGClick(ALWCharacter* P,FName N){if(!P->RPGPanel)return false;FString S=N.ToString();
 if(N==TEXT("encounter_journal")){P->EncounterJournal=!P->EncounterJournal;P->JournalPage=0;}
 else if(N==TEXT("encounter_next")){int NRows=0;for(const auto& E:P->World->Encounters.Records)NRows+=E.Value.Seen;P->JournalPage=(P->JournalPage+1)/1%FMath::Max(1,(NRows+4)/5);}
 else if(S.StartsWith(TEXT("eventtrack_"))){if(auto* R=P->World->Encounters.Records.Find(FName(*S.Mid(11))))P->SetWaypoint(FVector2D(R->Stage==1&&R->EscortPosition!=FVector::ZeroVector?R->EscortPosition:R->Position));}
 else if(N==TEXT("rpg_close"))P->ClosePanels();
 else if(S.StartsWith(TEXT("cat_"))){P->SkillCategory=FMath::Clamp(FCString::Atoi(*S.Mid(4)),0,6);P->SkillPage=0;}
 else if(N==TEXT("skill_next")){int Count=0;for(const auto& V:ULWRPGCatalog::Get()->Perks)Count+=V.Category==P->SkillCategory;P->SkillPage=(P->SkillPage+1)%FMath::Max(1,(Count+6)/7);}
 else if(S.StartsWith(TEXT("buy_")))P->BuyPerk(FName(*S.Mid(4)));
 else if(N==TEXT("attribute"))P->RaiseAttribute(P->SkillCategory);
 else if(N==TEXT("journal_next"))P->JournalPage=(P->JournalPage+1)%FMath::Max(1,(P->RPG.Quests.Num()+4)/5);
 else if(S.StartsWith(TEXT("track_")))P->TrackQuest(FName(*S.Mid(6)));
 else if(N==TEXT("crewpage55_prev"))CrewPage55=FMath::Max(0,CrewPage55-1);
 else if(N==TEXT("crewpage55_next"))CrewPage55=(CrewPage55+1)%FMath::Max(1,(P->RPG.Crew.Num()+8)/9);
 else if(S.StartsWith(TEXT("crew_"))){FName Id(*S.Mid(5));const auto* C=P->RPG.Crew.FindByPredicate([&](const auto& X){return X.Id==Id;});if(C)P->SetCompanion(Id,!C->Following);}
 else if(N==TEXT("dialoguepage55"))DialoguePage55=(DialoguePage55+1)%FMath::Max(1,(P->DialogueChoices.Num()+4)/5);
 else if(S.StartsWith(TEXT("dialogue_")))P->ChooseDialogue(FCString::Atoi(*S.Mid(9)));
 return true;
}
void ALWHUD::RPGOverlay(ALWCharacter* P){
 for(const auto& E:P->World->LiveEncounters)if(IsValid(E.Value)){const auto* R=E.Value->Record();if(R&&R->Stage==1&&FVector::Dist2D(R->Position,P->GetActorLocation())<3500){Text(Wrap(E.Value->Objective(),58),24,193,.75f,FLinearColor(.9,.72,.4));break;}}

 Text(FString::Printf(TEXT("LV %02d / %d SP"),P->RPG.Level,P->RPG.Points),24,49,.8f);
 const int64 Base=P->RPG.Level>1?LWRPG::Threshold(P->RPG.Level-1):0,Next=LWRPG::Threshold(P->RPG.Level);
 const float Progress=P->RPG.Level>=100?1.f:FMath::Clamp(float(P->RPG.XP-Base)/FMath::Max<int64>(1,Next-Base),0.f,1.f);
 Rect(24,68,180,3,FLinearColor(.12,.15,.12,.85));Rect(24,68,180*Animate55(TEXT("xp55"),Progress,10),3,FLinearColor(.78,.61,.29));Text(TEXT("XP"),211,60,.6f,FLinearColor(.58,.62,.5));
 if(P->RPG.Story.Enabled&&P->RPG.Story.Stage<29){const auto& M=LWStory::Missions()[P->RPG.Story.Stage];Text(FString(M.Title)+TEXT("\n")+Wrap(M.Objective,45),24,80,.75f);if(P->Story&&M.Enemies)Text(FString::Printf(TEXT("Hostiles remaining: %d"),P->Story->Remaining()),24,140,.7f);}
 else if(!P->RPG.TrackedQuest.IsNone())for(const auto& S:P->RPG.Quests)if(S.Id==P->RPG.TrackedQuest&&!S.Rewarded){const auto* Q=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& X){return X.Id==S.Id;});if(Q){FString Line=Q->Title;if(Q->Objectives.IsValidIndex(S.Stage)){const auto& O=Q->Objectives[S.Stage];int Count=FMath::Clamp(O.Event==TEXT("deliver")?P->CountSupply(O.Target):S.Progress,0,O.Count);Line+=TEXT("\n")+Wrap(O.Text,42)+FString::Printf(TEXT("  %d/%d"),Count,O.Count);Rect(24,164,250,4,FLinearColor(.12f,.16f,.12f,.8f));Rect(24,164,250.f*Count/FMath::Max(1,O.Count),4,FLinearColor(.8f,.65f,.3f,1));}else Line+=TEXT("\nRETURN TO WARDEN");Text(Line,24,78,.8f);}}
 // Screen-space speech remains legible when the speaker is beside or behind you.
 ALWResident* Speaker56=nullptr;float Score56=-1;
 for(TActorIterator<ALWResident> I(GetWorld());I;++I){
  const float Distance=FVector::Dist(I->GetActorLocation(),P->GetActorLocation());
  if(I->SubtitleTime<=0||I->Subtitle.IsEmpty()||Distance>1100)continue;
  FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,P);
  bool Occluded=GetWorld()->LineTraceSingleByChannel(Hit,P->Camera->GetComponentLocation(),I->GetActorLocation()+FVector(0,0,50),ECC_Visibility,Q)&&Hit.GetActor()!=*I;
  if(Occluded&&Distance>450)continue;
  const float Score=I->SubtitleTime-Distance/2200.f;
  if(Score>Score56){Speaker56=*I;Score56=Score;}
 }
 if(Speaker56){
  const FString Line=Wrap(Speaker56->Subtitle,76);int Lines=1;for(TCHAR C:Line)if(C=='\n')++Lines;
  const float W=700,X=(Canvas->ClipX/Scale-W)*.5f,Y=540-Lines*18;
  const float Fade=FMath::Clamp(Speaker56->SubtitleTime*3.f,0.f,1.f);
  Rect(X-16,Y-10,W+32,42+Lines*18,FLinearColor(0,0,0,.78f*Fade));
  Text(Speaker56->DisplayName,X,Y,.85f,FLinearColor(1,.76,.42,Fade));
  Text(Line,X,Y+23,.95f,FLinearColor(.96,.96,.92,Fade));
 }
}
