#include "LWAircraft84.h"
#include "LWGeography84.h"
#include "LWNewYork69.h"
#include "LWGeneration.h"
#include "LWCampaign76Corridor.h"
#include "LWCampaignProduction77.h"
#include "LWSaveGame.h"
#include "Misc/AutomationTest.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWGeography84Test,"LethalWorld.Update84.Geography",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWGeography84Test::RunTest(const FString&){
 TestTrue(TEXT("Toronto is in Canada"),LWGeography84::Canada(LWNY69::Project(43.6532,-79.3832)));for(auto P:{FVector2D(42.8864,-78.8784),FVector2D(43.1566,-77.6088),FVector2D(43.0481,-76.1474),FVector2D(43.9748,-75.9108)})TestFalse(TEXT("New York cities remain American"),LWGeography84::Canada(LWNY69::Project(P.X,P.Y)));
 TestTrue(TEXT("Lake Ontario contains its geographic center"),LWNY69::WaterDepth(LWNY69::Project(43.70,-77.7))>0);TestTrue(TEXT("Onondaga Lake retains detailed shoreline"),LWNY69::WaterDepth(LWNY69::Project(43.094,-76.22))>0);TestEqual(TEXT("three real border crossing areas"),LWGeography84::Checkpoints().Num(),3);
 TestTrue(TEXT("regional town coverage"),LWNY69::Towns().Num()>=35);TestTrue(TEXT("map-derived road network"),LWNY69::Roads().Num()>10000);TestTrue(TEXT("town and rural parcels populate map"),LWNY69::Sites().Num()>300);
 FString CSV=TEXT("id,type,x,y,width,height,name\n");int Communities=0;TSet<FIntPoint> TownRegions;for(const auto& S:LWNY69::Sites()){CSV+=FString::Printf(TEXT("%u,%d,%.2f,%.2f,%.2f,%.2f,\"%s\"\n"),S.Id,S.Type,S.Position.X,S.Position.Y,S.Size.X,S.Size.Y,*S.PlaceName69);if(S.SettlementBuilding){Communities++;TownRegions.Add({LWGen::FloorDiv(S.Position.X+25600,51200),LWGen::FloorDiv(S.Position.Y+25600,51200)});}}
 FFileHelper::SaveStringToFile(CSV,*(FPaths::ProjectSavedDir()/TEXT("Geography84_Parcels.csv")));TestTrue(TEXT("multiple populated mapped communities"),Communities>=40);AddInfo(FString::Printf(TEXT("roads=%d parcels=%d community plots=%d"),LWNY69::Roads().Num(),LWNY69::Sites().Num(),Communities));
 const auto& Stops=LWCampaign76::Corridor();TestTrue(TEXT("Rome and Watertown story stops keep geographical order"),Stops[1].Position.X<Stops[7].Position.X);TestTrue(TEXT("Adirondack stops lie east of Tug Hill"),Stops[10].Position.Y>Stops[4].Position.Y);
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWAircraft84Test,"LethalWorld.Update84.AircraftContracts",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWAircraft84Test::RunTest(const FString&){
 const auto* R=LWAviation84::Runway(TEXT("SYR10"));if(!TestNotNull(TEXT("Hancock east-west runway"),R))return false;TestTrue(TEXT("10-28 near published 9003 feet"),FMath::Abs(R->Length()/100-2744)<15);R=LWAviation84::Runway(TEXT("SYR15"));TestTrue(TEXT("15-33 near published 7500 feet"),R&&FMath::Abs(R->Length()/100-2286)<15);TestTrue(TEXT("regional destination airfields"),LWAviation84::Runways().Num()>=7);
 for(FName Model:{FName(TEXT("private_jet")),FName(TEXT("airbus")),FName(TEXT("luxury_airbus"))}){auto S=LWAviation84::Spec(Model);TestTrue(TEXT("manual fuel endurance is one real hour"),FMath::IsNearlyEqual(LWAviation84::Burn(S.Capacity,false,3600),S.Capacity,.01f));TestTrue(TEXT("autopilot fuel endurance is five real hours"),FMath::IsNearlyEqual(LWAviation84::Burn(S.Capacity,true,18000),S.Capacity,.01f));TArray<FVector> Seats;for(int I=-1;I<S.Seats-1;I++){auto V=LWAviation84::Seat(Model,I);TestTrue(TEXT("seat stays inside cabin width"),FMath::Abs(V.Y)<S.Width);TestFalse(TEXT("no duplicated seat positions"),Seats.Contains(V));Seats.Add(V);}if(Model==TEXT("airbus"))TestEqual(TEXT("airbus has exactly 100 seating positions"),Seats.Num(),100);}
 TestEqual(TEXT("waypoint resolves nearest airport"),LWAviation84::Closest(LWNY69::Project(43.0481,-76.1474))->Id,FName(TEXT("SYR10")));return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWMigration84Test,"LethalWorld.Update84.SaveMigration",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWMigration84Test::RunTest(const FString&){auto* S=NewObject<ULWSaveGame>();S->Position=FVector(360050,2820100,110);S->Money=12345;FLWVehicleRecord R;R.Position=S->Position+FVector(50,0,0);R.Owned45=true;S->Vehicles.Add(TEXT("owned"),R);LWGeography84::Migrate(S);TestTrue(TEXT("story player relocates with local offset"),S->Position.Equals(FVector(LWCampaign76::Corridor()[1].Position+FVector2D(50,100),110),1));TestTrue(TEXT("owned vehicles follow relocated story parcel"),S->Vehicles[TEXT("owned")].Position.Equals(S->Position+FVector(50,0,0),1));TestEqual(TEXT("money preserved"),S->Money,int64(12345));auto P=S->Position;LWGeography84::Migrate(S);TestEqual(TEXT("migration idempotent"),S->Position,P);return true;}
#endif
