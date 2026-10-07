#include "LWSyracuse73.h"
#include "LWNewYork69.h"
#include "LWGeneration.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWSyracuse73,"LethalWorld.Update73.Geography",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWSyracuse73::RunTest(const FString&){
 for(auto P:{FVector2D(0),FVector2D(200000,2200000),FVector2D(700000,2600000),FVector2D(-200000,1500000)})TestTrue(TEXT("Projection round trip"),(LWSyracuse73::Unwarp(LWSyracuse73::Warp(P))-P).Size()<.01);
 TestTrue(TEXT("Buffalo remains at regional origin"),LWNY69::Project(42.8864,-78.8784).IsNearlyZero());
 TestTrue(TEXT("Syracuse landmark scale close to metres"),(LWNY69::Project(43.0482,-76.1474)-LWNY69::Project(43.0481,-76.1474)).Size()>1000);
 TestTrue(TEXT("Mapped lake has water"),LWSyracuse73::LakeDepth(LWNY69::Project(43.09,-76.205))>0);
 TestEqual(TEXT("Start is dry"),LWNY69::WaterDepth(LWSyracuse73::Start()),0.f);
 const auto& Sites=LWNY69::Sites();int Blocks=0,Houses=0;
 for(const auto& S:Sites){Blocks+=S.Type==79;Houses+=S.Id>=0xC7300000u&&S.Id<0xC7400000u;}
 TestTrue(TEXT("Mapped city blocks present"),Blocks>1500);TestTrue(TEXT("Playable street-front parcels present"),Houses>500);
 for(int T=70;T<=78;T++){
  const auto* S=Sites.FindByPredicate([&](const auto& V){return V.Type==T;});TestNotNull(FString::Printf(TEXT("Landmark %d exists"),T),S);if(!S)continue;
  TArray<LWGen::FRoad> R;TArray<LWGen::FSite> Local;LWGen::Gather(S->Position,198706,R,Local);
  TestTrue(FString::Printf(TEXT("Landmark %d survives road filtering"),T),Local.ContainsByPredicate([&](const auto& V){return V.Id==S->Id;}));
  TestEqual(FString::Printf(TEXT("Landmark %d emitted once"),T),Local.FilterByPredicate([&](const auto& V){return V.Id==S->Id;}).Num(),1);
  if(T!=70&&T!=78)TestFalse(FString::Printf(TEXT("Landmark %d road clearance"),T),R.ContainsByPredicate([&](const auto& Road){return LWGen::RoadOverlaps(*S,Road);}));
 }
 TestTrue(TEXT("Mapped fairground buildings"),LWSyracuse73::FairBuildings().Num()>50);
 return true;
}
#endif
