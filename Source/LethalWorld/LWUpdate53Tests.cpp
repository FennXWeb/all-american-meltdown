#include "LWGeneration.h"
#include "LWBoss48.h"
#include "LWPOISurvivors.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWMajor53Test,"LethalWorld.Update53.MajorDestinationPlacement",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWMajor53Test::RunTest(const FString&){TMap<int,int> Counts;int Checked=0;for(int Seed:{198706,42})for(int Y=-6;Y<=6;Y++)for(int X=-6;X<=6;X++){
 const FIntPoint R(X,Y);LWGen::FSite Expected;if(!LWGen::SpecialSite(R,Seed,Expected)||!(Expected.Type==21||LWPlaces::Expanded(Expected.Type)||LWDungeons::IsDungeon(Expected.Type)))continue;
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Region(R,Seed,Roads,Sites);const auto* Found=Sites.FindByPredicate([&](const auto& S){return S.Id==Expected.Id;});
 TestTrue(*FString::Printf(TEXT("reserved major POI survives road generation seed=%d region=%d,%d type=%d"),Seed,X,Y,Expected.Type),Found!=nullptr);if(!Found)continue;Counts.FindOrAdd(Found->Type)++;Checked++;
 TestFalse(TEXT("no road intersects major footprint"),Roads.ContainsByPredicate([&](const auto& Road){return LWGen::RoadOverlaps(*Found,Road);}));TestTrue(TEXT("entrance has attached access road"),Roads.ContainsByPredicate([&](const auto& Road){return Road.A.Equals(LWGen::Entrance(*Found),5)||Road.B.Equals(LWGen::Entrance(*Found),5);}));
 UE_LOG(LogTemp,Display,TEXT("WORLD53_MAJOR seed=%d type=%d id=%u xy=%.0f,%.0f"),Seed,Found->Type,Found->Id,Found->Position.X,Found->Position.Y);
 }
 TestTrue(TEXT("malls are present"),Counts.FindRef(58)>=4);TestTrue(TEXT("prisons are present"),Counts.FindRef(59)>=4);TestTrue(TEXT("casinos remain rare but present"),Counts.FindRef(21)>=1);TestTrue(TEXT("sample covers many reserved destinations"),Checked>=20);return true;}
#endif
