#include "LWWeaponMods.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWDentSave,"LethalWorld.Vehicles.DentSaveRoundTrip",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWDentSave::RunTest(const FString&){auto* S=NewObject<ULWSaveGame>();FLWVehicleRecord R;R.VIN=FGuid::NewGuid();FLWVehicleDent D;D.Point=FVector(100,80,50);D.Direction=FVector(0,-1,0);D.Radius=90;D.Depth=15;R.Dents.Add(D);S->Vehicles.Add(TEXT("car"),R);TArray<uint8> Bytes;TestTrue(TEXT("dented vehicle serializes"),UGameplayStatics::SaveGameToMemory(S,Bytes));auto* Loaded=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));if(!TestNotNull(TEXT("save restores"),Loaded))return false;const auto& Back=Loaded->Vehicles.FindChecked(TEXT("car"));TestEqual(TEXT("dent count persists"),Back.Dents.Num(),1);if(Back.Dents.Num()){TestTrue(TEXT("local impact point persists"),Back.Dents[0].Point.Equals(D.Point));TestEqual(TEXT("dent depth persists"),Back.Dents[0].Depth,15.f);TestTrue(TEXT("impact direction persists"),Back.Dents[0].Direction.Equals(D.Direction));}TestTrue(TEXT("vehicle identity persists"),Back.VIN==R.VIN);return true;}
#endif
