#include "LWVehicleState.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWSecurityArchiveTest,"LethalWorld.Security.KeysAndVehiclesArchive",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWSecurityArchiveTest::RunTest(const FString&){
 auto* Save=NewObject<ULWSaveGame>();FLWVehicleRecord V;V.VIN=FGuid::NewDeterministicGuid(TEXT("road_18_car"),73);V.Position=FVector(-33000000,8200000,75);V.LockTier=4;V.Unlocked=true;V.Hotwired=true;V.Health=37;V.Attempts=3;Save->Vehicles.Add(TEXT("road_18_car"),V);Save->SeatedVehicle=TEXT("road_18_car");
 FLWItemInstance Key;Key.Id=FGuid::NewGuid();Key.Definition=TEXT("car_key");Key.VehicleId=V.VIN;Save->Inventory.Add(Key);FLWContainerRecord Box;Box.Id=TEXT("secure_box");Box.LockTier=3;Box.Unlocked=true;Box.LockAttempts=2;Box.Items.Add(Key);Save->WorldContainers.Add(Box.Id,Box);
 TArray<uint8> Bytes;TestTrue(TEXT("security save encodes"),UGameplayStatics::SaveGameToMemory(Save,Bytes));auto* Loaded=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));if(!TestNotNull(TEXT("security archive decodes"),Loaded))return false;
 TestEqual(TEXT("key retains exact VIN"),Loaded->Inventory[0].VehicleId,V.VIN);TestEqual(TEXT("nested key retains VIN"),Loaded->WorldContainers.FindChecked(Box.Id).Items[0].VehicleId,V.VIN);const auto& R=Loaded->Vehicles.FindChecked(TEXT("road_18_car"));TestEqual(TEXT("far world coordinates preserved"),R.Position,V.Position);TestTrue(TEXT("security and damage persisted"),R.Hotwired&&R.Unlocked&&R.LockTier==4&&R.Attempts==3&&R.Health==37);TestEqual(TEXT("driver seat identity persists"),Loaded->SeatedVehicle,FName(TEXT("road_18_car")));return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLockToleranceTest,"LethalWorld.Security.LockTolerance",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWLockToleranceTest::RunTest(const FString&){float Last=100;for(int Tier=1;Tier<=4;Tier++){const float Window=LWSecurity::Window(Tier);TestTrue(TEXT("higher tiers narrow tolerance"),Window<Last);Last=Window;for(float Secret:{-78.f,0.f,78.f}){TestEqual(TEXT("correct angle allows full turn"),LWSecurity::TurnLimit(Secret,Secret,Tier),90.f);TestTrue(TEXT("wrong angle binds"),LWSecurity::TurnLimit(Secret-Window-1,Secret,Tier)<89);TestEqual(TEXT("binding symmetric"),LWSecurity::TurnLimit(Secret-Window-5,Secret,Tier),LWSecurity::TurnLimit(Secret+Window+5,Secret,Tier));}}return true;}
#endif
