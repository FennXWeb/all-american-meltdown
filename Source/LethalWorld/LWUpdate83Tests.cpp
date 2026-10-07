#include "LWRoads33.h"
#include "LWSyracuse73.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWStops83Test,"LethalWorld.Update83.StopPriority",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWStops83Test::RunTest(const FString&){
 TArray<LWGen::FRoad> Roads{{{0,0},{0,4000},780,false},{{-4000,0},{4000,0},780,false}};
 auto J=LWRoads33::Junctions(Roads,1);if(!TestEqual(TEXT("T junction"),J.Num(),1))return false;
 TestTrue(TEXT("terminating approach stops"),LWRoads33::NeedsStop(J[0],{0,1}));TestFalse(TEXT("through road keeps priority"),LWRoads33::NeedsStop(J[0],{1,0}));TestFalse(TEXT("opposing through road keeps priority"),LWRoads33::NeedsStop(J[0],{-1,0}));
 Roads[0].A={0,-4000};J=LWRoads33::Junctions(Roads,1);int Stops=0;for(auto& A:J[0].Arms)Stops+=LWRoads33::NeedsStop(J[0],A.Out);TestEqual(TEXT("local crossroad has two stop signs rather than four"),Stops,2);
 Roads[1].Width=1680;Roads[1].Highway=true;J=LWRoads33::Junctions(Roads,1);for(auto& A:J[0].Arms)TestFalse(TEXT("signals never also spawn stop signs"),LWRoads33::NeedsStop(J[0],A.Out));
 int Before=0,After=0;TSet<uint32> Seen;
 for(FVector2D Offset:{FVector2D(0,0),FVector2D(50000,0),FVector2D(-50000,0),FVector2D(0,50000),FVector2D(0,-50000)}){
  Roads.Reset();TArray<LWGen::FSite> Sites;LWGen::Gather(LWSyracuse73::Start()+Offset,198706,Roads,Sites);
  for(const auto& X:LWRoads33::Junctions(Roads,198706))if(!X.Signals&&!Seen.Contains(X.Id)){Seen.Add(X.Id);for(const auto& A:X.Arms){Before++;After+=LWRoads33::NeedsStop(X,A.Out);}}
 }
 AddInfo(FString::Printf(TEXT("Syracuse sampled signs: before=%d after=%d reduction=%.1f%%"),Before,After,Before?100.*(Before-After)/Before:0));
 TestTrue(TEXT("populated sample reduces stop signs by at least half"),Before>20&&After<=Before/2);
 return true;
}
#endif
