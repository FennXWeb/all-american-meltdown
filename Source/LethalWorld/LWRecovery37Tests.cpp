#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWRecovery37Test,"LethalWorld.Missions.RecoverySnapshots",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWRecovery37Test::RunTest(const FString&){
 auto* State=NewObject<ULWSaveGame>();State->Version=2;State->RPG.Story.Enabled=true;State->RPG.Story.Stage=4;State->Money=120;
 auto Gear=LWItems::Make(TEXT("water"),2);State->Inventory.Add(Gear);State->Killed.Add(123);
 TArray<uint8> Start;TestTrue(TEXT("encode mission start"),UGameplayStatics::SaveGameToMemory(State,Start));
 State->Money=155;State->RPG.Story.Stage=5;State->PropStates.Add(TEXT("door"),1);
 TArray<uint8> Checkpoint;UGameplayStatics::SaveGameToMemory(State,Checkpoint);
 auto* Disk=NewObject<ULWSaveGame>();auto& R=Disk->MissionRecovery37.Add(TEXT("chapter1"));R.Start=Start;R.Checkpoint=Checkpoint;R.StartStage=3;R.CheckpointStage=5;
 TArray<uint8> Bytes;UGameplayStatics::SaveGameToMemory(Disk,Bytes);auto* Read=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));TestNotNull(TEXT("recovery survives save reload"),Read);if(!Read)return false;
 auto* A=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Read->MissionRecovery37[TEXT("chapter1")].Start));auto* B=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Read->MissionRecovery37[TEXT("chapter1")].Checkpoint));
 TestNotNull(TEXT("start readable"),A);TestNotNull(TEXT("checkpoint readable"),B);if(A&&B){TestEqual(TEXT("start keeps original credits"),A->Money,int64(120));TestEqual(TEXT("checkpoint keeps later credits"),B->Money,int64(155));TestEqual(TEXT("checkpoint objective"),B->RPG.Story.Stage,5);TestTrue(TEXT("inventory identity restored"),B->Inventory[0].Id==Gear.Id);TestTrue(TEXT("world state restored"),B->Killed.Contains(123)&&B->PropStates.Contains(TEXT("door")));TestTrue(TEXT("payloads are not recursive"),A->MissionRecovery37.IsEmpty()&&B->MissionRecovery37.IsEmpty());}
 return true;
}
#endif
