#include "LWGeneration.h"
#include "LWLootTable.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWUnderground29Test,"LethalWorld.World29.UndergroundConnectivity",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWUnderground29Test::RunTest(const FString&){for(int T=53;T<=55;T++){const auto& P=LWDungeons::Profile(T);TestTrue(TEXT("multiple subterranean decks"),P.Floors>=2);for(int Seed:{1,198706,73642})for(int F=0;F<P.Floors;F++){const auto A=LWDungeons::Layout(T,Seed,F),B=LWDungeons::Layout(T,Seed,F);TestTrue(TEXT("seeded layout stable"),A.Links==B.Links);TestFalse(TEXT("every chamber connected to entrance"),A.Distance.Contains(-1));TestTrue(TEXT("finale separated from entry"),A.End>0&&A.Distance[A.End]>=3);for(int Relay=0;Relay<3;Relay++)TestTrue(TEXT("reachable control"),LWDungeons::RelayRoom(T,Seed,Relay)<P.Rows*P.Columns);}}return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWWorld29Placement,"LethalWorld.World29.NewSitePlacement",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWWorld29Placement::RunTest(const FString&){TSet<int> Seen;for(int Y=-6;Y<=6;Y++)for(int X=-6;X<=6;X++){TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Region({X,Y},198706,Roads,Sites);for(const auto& S:Sites)if(S.Type>=53){Seen.Add(S.Type);TestFalse(TEXT("new site does not intersect roads"),Roads.ContainsByPredicate([&](const auto& R){return LWGen::RoadOverlaps(S,R);}));if(LWPlaces::IsTower(S.Type))TestTrue(TEXT("tower variant in city center"),S.District==4);}}
 for(int T=53;T<=57;T++)TestTrue(*FString::Printf(TEXT("new POI type %d actually survives generation"),T),Seen.Contains(T));return true;}
#endif
