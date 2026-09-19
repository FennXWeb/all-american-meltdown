#include "LWTradingCards36.h"
#include "HAL/PlatformTime.h"
void ALWGameMode::BuildRecovery37Smoke(ALWCharacter& Initial){
 struct FState {int Gear=0;FName Quest;};auto S=MakeShared<FState>();
 auto Add=[this](const TCHAR* Label,double Delay,FLWV2Action Begin,FLWV2Action End=FLWV2Action()){V2->Steps.Add({Label,Delay,120,MoveTemp(Begin),MoveTemp(End),[](ALWCharacter&){return true;}});};
 Add(TEXT("Cold card requests and mission start"),3,[this,S](ALWCharacter& P){
  const double Begin=FPlatformTime::Seconds();for(int I=0;I<100;I++){
   if(FParse::Param(FCommandLine::Get(),TEXT("CardSyncBaseline37")))LoadObject<UTexture2D>(nullptr,*FString::Printf(TEXT("/Game/Art/Textures/T_TradingCard%03d36.T_TradingCard%03d36"),I+1,I+1));else LWCollect36::Artwork(I);
  }UE_LOG(LogTemp,Display,TEXT("CARD37_REQUEST_TIME_MS %.3f"),(FPlatformTime::Seconds()-Begin)*1000);
  P.NewGame();P.RPG.Story.Enabled=true;P.RPG.Story.Stage=3;P.Money=100;P.MissionRecovery37.Empty();S->Gear=P.Inventory.Num();P.CaptureMission37();Check(P.HasMissionRecovery37(),TEXT("mission start snapshot created"));
 });
 Add(TEXT("Objective checkpoint and death"),2,[this](ALWCharacter& P){
  P.RPG.Story.Stage=4;P.Money=200;P.World->KilledZombies.Add(LWStory::EnemyId(4,0));P.World->PropStates.Add(TEXT("recovery_test37"),1);P.CaptureMission37();
  P.Money=1;P.World->KilledZombies.Add(LWStory::EnemyId(4,1));P.World->PropStates[TEXT("recovery_test37")]=2;P.RPG.TradingCards36.Add(99);P.Health=0;P.HandleDeath();
  Check(P.HasMissionRecovery37()&&!P.CanAct(),TEXT("death offers recovery and blocks attacks"));P.SaveProgress();P.LoadProgress();Check(P.Health<=0&&P.HasMissionRecovery37(),TEXT("loading death save keeps choice menu"));
 },[this](ALWCharacter& P){CaptureV2(TEXT("Recovery37_DeathChoices"));});
 Add(TEXT("Checkpoint rollback"),2,[this,S](ALWCharacter& P){
  P.Respawn();Check(P.Health<=0,TEXT("enter cannot bypass mission choice"));Check(P.RecoverMission37(0),TEXT("checkpoint retry accepted"));
  Check(P.Health>0&&P.RPG.Story.Stage==4&&P.Money==200,TEXT("checkpoint restores objective and rewards"));Check(P.Inventory.Num()==S->Gear,TEXT("checkpoint restores carried inventory"));
  Check(P.World->KilledZombies.Contains(LWStory::EnemyId(4,0))&&!P.World->KilledZombies.Contains(LWStory::EnemyId(4,1)),TEXT("checkpoint rolls back later kills"));Check(P.World->PropStates.FindRef(TEXT("recovery_test37"))==1,TEXT("world interactions restored"));Check(P.RPG.TradingCards36.Contains(99),TEXT("permanent collection retained"));
  Check(!P.RecoverMission37(0),TEXT("living player cannot farm retries"));
 });
 Add(TEXT("Mission restart"),2,[this](ALWCharacter& P){P.Health=0;P.HandleDeath();Check(P.RecoverMission37(1),TEXT("mission restart accepted"));Check(P.RPG.Story.Stage==3&&P.Money==100,TEXT("mission restart differs from checkpoint"));});
 Add(TEXT("Prison cancel"),2,[this,S](ALWCharacter& P){
  P.RPG.Story.Stage=21;P.Story->Confiscate();P.CaptureMission37();P.Health=0;P.HandleDeath();Check(P.RecoverMission37(2),TEXT("prison mission can be cancelled"));Check(P.bSafehouse&&!P.RPG.Story.Enabled&&!P.RPG.Story.GearHeld&&P.Inventory.Num()==S->Gear,TEXT("cancel returns gear and survivor to bunker"));
 });
 Add(TEXT("Contract checkpoint"),2,[this,S](ALWCharacter& P){
  for(const auto& Q:ULWRPGCatalog::Get()->Quests)if(Q.Prerequisite.IsNone()&&Q.Objectives.Num()>0){S->Quest=Q.Id;break;}
  Check(P.AcceptQuest(S->Quest),TEXT("accept contract"));Check(P.HasMissionRecovery37(),TEXT("contract has start snapshot"));auto* Q=P.RPG.Quests.FindByPredicate([&](const auto& V){return V.Id==S->Quest;});if(Q)Q->Stage=1;P.CaptureMission37(S->Quest);P.Health=0;P.HandleDeath();Check(P.RecoverMission37(1),TEXT("contract restart"));Q=P.RPG.Quests.FindByPredicate([&](const auto& V){return V.Id==S->Quest;});Check(Q&&Q->Stage==0,TEXT("contract restarts first objective"));
 });
 Add(TEXT("Contract cancellation and assets"),2,[this,S](ALWCharacter& P){P.Health=0;P.HandleDeath();Check(P.RecoverMission37(2),TEXT("contract cancellation"));Check(P.bSafehouse&&!P.RPG.Quests.ContainsByPredicate([&](const auto& V){return V.Id==S->Quest;}),TEXT("cancel removes active contract"));});
 V2->Steps.Add({TEXT("Await asynchronous artwork"),0,90,[](ALWCharacter&){},[this](ALWCharacter&){for(int I=0;I<100;I++)Check(LWCollect36::Artwork(I)!=nullptr,TEXT("asynchronous card art eventually available"));},[](ALWCharacter&){for(int I=0;I<100;I++)if(!LWCollect36::Artwork(I))return false;return true;}});
 Add(TEXT("capture flush"),2,[](ALWCharacter& P){});
}
