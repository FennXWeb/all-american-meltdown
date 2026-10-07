#include "LWCampaign76.h"
#include "LWCharacter.h"
#include "LWSaveGame.h"
#include "LWStory.h"
#include "LWChapter52.h"
#include "LWWorld.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Async/Async.h"
#include "ProfilingDebugging/CpuProfilerTrace.h"

FName ALWCharacter::ActiveMission37()const {
 if(RPG.Campaign76.Started&&!RPG.Campaign76.Paused&&RPG.Campaign76.Ending.IsNone())return TEXT("campaign76");

 for(const auto& Q:RPG.Quests)if(Q.Id==RPG.TrackedQuest&&!Q.Rewarded)return Q.Id;
 return NAME_None;
}
bool ALWCharacter::HasMissionRecovery37()const {
 for(const auto& Job:CheckpointJobs43)if(Job->Key==ActiveMission37())return true;
 const auto* R=MissionRecovery37.Find(ActiveMission37());return R&&!R->Start.IsEmpty()&&!R->Checkpoint.IsEmpty();
}
void ALWCharacter::CaptureMission37(FName Key,bool Force){
 if(!bStarted||!World||Health<=0||bLoadingSave||bRestoringMission37||OpeningMode)return;
 if(Key.IsNone())Key=ActiveMission37();if(Key.IsNone()||Key==TEXT("chapter1"))return;
 int Stage=-1,StartStage=0;FString Title;
 if(Key==TEXT("campaign76")){
  const auto* Current=LWCampaign76::Stage(RPG.Campaign76.Stage);if(!Current)return;Stage=RPG.Campaign76.Revision;Title=Current->Title;StartStage=0;for(int I=0;I<LWCampaign76::Stages().Num();I++)if(LWCampaign76::Stages()[I].Mission==Current->Mission){StartStage=I;break;}
 }else if(Key==TEXT("chapter1")){
  if(!RPG.Story.Enabled||RPG.Story.Stage>=LWStory::Complete)return;
  Stage=RPG.Story.Stage;Title=LWStory::Missions()[Stage].Title;StartStage=Stage;
  while(StartStage>0&&Title==LWStory::Missions()[StartStage-1].Title)--StartStage;
 }else{
  const auto* Q=RPG.Quests.FindByPredicate([&](const auto& V){return V.Id==Key&&!V.Rewarded;});
  const auto* Def=ULWRPGCatalog::Get()->Quests.FindByPredicate([&](const auto& V){return V.Id==Key;});
  if(!Q||!Def)return;Stage=Q->Stage;Title=Def->Title;
 }
 auto* Existing=MissionRecovery37.Find(Key);
 if(!Force)for(int32 I=CheckpointJobs43.Num()-1;I>=0;--I)if(CheckpointJobs43[I]->Key==Key){
  if(CheckpointJobs43[I]->Stage==Stage&&CheckpointJobs43[I]->StartStage==StartStage)return;break;
 }
 if(!Force&&Existing&&Existing->CheckpointStage==Stage&&Existing->StartStage==StartStage)return;
 auto* Snapshot=MakeProgressSnapshot37();Snapshot->Health=FMath::Max(1.f,Health);
 if(Key==TEXT("chapter1")){Snapshot->Health=FMath::Max(Snapshot->Health,MaxHealth()*.75f);}
 // Checkpoints never contain other checkpoints: snapshots remain bounded, not recursive.
 auto Job=MakeShared<FLWCheckpointJob43>();Job->Key=Key;Job->Title=Title;Job->Stage=Stage;Job->StartStage=StartStage;
 CheckpointSnapshots43.Add(Snapshot);CheckpointJobs43.Add(Job);
 Job->Bytes=Async(EAsyncExecution::ThreadPool,[Snapshot](){
  TRACE_CPUPROFILER_EVENT_SCOPE(LW_CheckpointSerialize43);
  TArray<uint8> Bytes;UGameplayStatics::SaveGameToMemory(Snapshot,Bytes);return Bytes;
 });
}
void ALWCharacter::FinishCheckpoints43(bool Wait){
 // Commit in capture order, even if a later worker finishes first.
 while(!CheckpointJobs43.IsEmpty()){
  auto Job=CheckpointJobs43[0];if(!Wait&&!Job->Bytes.IsReady())return;
  TArray<uint8> Bytes=Job->Bytes.Get();CheckpointJobs43.RemoveAt(0);CheckpointSnapshots43.RemoveAt(0);
  if(Bytes.IsEmpty()){Notify(TEXT("CHECKPOINT SAVE FAILED"),5);continue;}
  // Completed/abandoned missions must not be resurrected by an older worker.
  const bool Active=Job->Key==TEXT("campaign76")?RPG.Campaign76.Started&&!RPG.Campaign76.Paused&&RPG.Campaign76.Ending.IsNone():Job->Key==TEXT("chapter1")?RPG.Story.Enabled&&RPG.Story.Stage<LWStory::Complete:
   RPG.Quests.ContainsByPredicate([&](const auto& Q){return Q.Id==Job->Key&&!Q.Rewarded;});
  if(!Active)continue;
  auto& R=MissionRecovery37.FindOrAdd(Job->Key);
  if(R.Start.IsEmpty()||R.StartStage!=Job->StartStage){R.Start=Bytes;R.StartStage=Job->StartStage;}
  R.Title=Job->Title;R.CheckpointStage=Job->Stage;R.Checkpoint=MoveTemp(Bytes);
  Notify(TEXT("CHECKPOINT SAVED"),3);RequestSave40();
 }
}
bool ALWCharacter::RecoverMission37(int32 Choice){
 if(Choice<0||Choice>2||bRestoringMission37||!(Health<=0||(Story&&Story->Failed)))return false;
 FinishCheckpoints43(true);
 const FName Key=ActiveMission37();const auto* R=MissionRecovery37.Find(Key);if(!R)return false;
 auto* Snapshot=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Choice==0?R->Checkpoint:R->Start));
 if(!Snapshot||Snapshot->Version>2){Notify(TEXT("CHECKPOINT COULD NOT BE READ"),5);return false;}
 TGuardValue<bool> Restoring(bRestoringMission37,true);
 TMap<FName,FLWMissionRecovery37> Recovery;Recovery.Add(Key,*R);auto Collection=RPG.TradingCards36;
 // The snapshot restores inventory, stash, vehicles, corpse loot, killed enemies and rewards together.
 if(Choice==2){if(Key==TEXT("campaign76"))Snapshot->RPG.Campaign76.Paused=true;else if(Key==TEXT("chapter1"))Snapshot->RPG.Story.Enabled=false;
  else {Snapshot->RPG.Quests.RemoveAll([&](const auto& Q){return Q.Id==Key;});Snapshot->RPG.TrackedQuest=NAME_None;}
  Recovery.Remove(Key);
 }
 if(Choice!=0&&Recovery.Contains(Key)){Recovery[Key].Checkpoint=Recovery[Key].Start;Recovery[Key].CheckpointStage=Key==TEXT("campaign76")?Snapshot->RPG.Campaign76.Revision:Key==TEXT("chapter1")?Snapshot->RPG.Story.Stage:0;}
 Snapshot->MissionRecovery37=MoveTemp(Recovery);Snapshot->RPG.TradingCards36.Append(Collection);
 Snapshot->Health=FMath::Max(1.f,Snapshot->Health);Snapshot->SeatedVehicle=NAME_None;
 ApplyProgressSnapshot37(Snapshot);bStoryLocked=false;bDeadSaved=false;bTrigger=bAim=bSprint=false;
 GetCharacterMovement()->SetMovementMode(MOVE_Walking);SetMenuInput(false);
 if(Choice==2){Health=MaxHealth();Stamina=MaxStamina();EnterSafehouse();if(RPG.Story.GearHeld){auto* D=ALWStoryDirector::Ensure(this);D->RecoverGear();D->Destroy();Story=nullptr;}ClearWaypoint();}
 else if(RPG.Story.Enabled)ALWStoryDirector::Ensure(this)->Start(false);
 RequestSave40();return true;
}
