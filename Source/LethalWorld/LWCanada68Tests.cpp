#include "LWStreaming68.h"
#include "LWCanada68.h"
#include "LWGeography84.h"
#include "Async/Async.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCanada68Generation,"LethalWorld.Update68.CountryAndStreaming",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCanada68Generation::RunTest(const FString&){
 for(const auto& C:LWGeography84::Checkpoints()){
  TestFalse(TEXT("Standing beside guards is neutral"),LWGeography84::Restricted(C.Position+FVector2D(-1100,0).GetRotated(C.Yaw)));
  TestFalse(TEXT("Approaching the red stripe is neutral"),LWGeography84::Restricted(C.Position+FVector2D(200,0).GetRotated(C.Yaw)));
  TestTrue(TEXT("Passing the red stripe is restricted"),LWGeography84::Restricted(C.Position+FVector2D(700,0).GetRotated(C.Yaw)));
 }
 for(int Seed:{7,198706,-917}){
  TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(LWGen::CanadaCity68(),Seed,Roads,Sites);
  TestTrue(TEXT("Major Canadian city has many destinations"),Sites.Num()>35);
  for(const auto& S:Sites){TestTrue(TEXT("Every Canadian destination is friendly"),S.Canadian&&S.Friendly);TestFalse(TEXT("No city parcel intersects its roads"),Roads.ContainsByPredicate([&](const auto& R){return LWGen::RoadOverlaps(S,R);}));}
  const auto C=LWGen::ChunkAt(LWGen::CanadaCity68());auto A=LWStreaming68::Plan(C,Seed,43,1,1);auto B=LWStreaming68::Plan(C+FIntPoint(1,0),Seed,43,1,1);
  for(int Y=0;Y<=24;Y++)TestEqual(TEXT("Terrain seam positions agree"),A->Vertices[Y*25+24].Z,B->Vertices[Y*25].Z);
  auto Job=Async(EAsyncExecution::ThreadPool,[C,Seed](){return LWStreaming68::Plan(C,Seed,43,1,1);});auto Worker=Job.Get();
  TestTrue(TEXT("Worker planning matches game-thread terrain"),Worker->Vertices==A->Vertices);TestEqual(TEXT("Worker planning matches sites"),Worker->Sites.Num(),A->Sites.Num());
  TArray<LWGen::FRoad> Route;TArray<LWGen::FSite> Stops;LWGen::Gather(LWNY69::Project(43.39,-79.77),Seed,Route,Stops);
  TestTrue(TEXT("Wilderness contains continuous highway"),Route.ContainsByPredicate([](const auto& R){return R.Highway&&LWGeography84::Canada(R.A);}));
 }
 return true;
}
#endif
