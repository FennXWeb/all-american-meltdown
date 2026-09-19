#include "LWRoads33.h"
#include "LWSurface26.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWRoad33Test,"LethalWorld.Roads33.JunctionsAndSignals",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWRoad33Test::RunTest(const FString&){
 TArray<LWGen::FRoad> Roads{{{-5000,0},{5000,0},1680,true},{{0,-4000},{0,4000},780,false}};
 auto J=LWRoads33::Junctions(Roads,12);TestEqual(TEXT("Crossing has one junction"),J.Num(),1);if(J.Num()!=1)return false;TestEqual(TEXT("Four approaches"),J[0].Arms.Num(),4);TestTrue(TEXT("Arterial has signals"),J[0].Signals);
 bool Green=false,Red=false,Amber=false,Clearance=false;for(int T=0;T<680;T++){int A=LWRoads33::Phase(J[0],{1,0},T*.1),B=LWRoads33::Phase(J[0],{0,1},T*.1);TestFalse(TEXT("Perpendicular greens never conflict"),A==2&&B==2);TestEqual(TEXT("Opposing main arms agree"),A,LWRoads33::Phase(J[0],{-1,0},T*.1));Green|=A==2;Red|=A==0;Amber|=A==1;Clearance|=A==0&&B==0;}TestTrue(TEXT("Complete phased cycle with all-red clearance"),Green&&Red&&Amber&&Clearance);
 Roads={{{-1000,0},{1000,0},780,false},{{0,0},{0,1000},580,false}};J=LWRoads33::Junctions(Roads,1);TestEqual(TEXT("T junction recognized"),J.Num(),1);if(J.Num())TestFalse(TEXT("Local junction uses stop signs"),J[0].Signals);
 Roads={{{-1000,0},{0,0},780,false},{{0,0},{1000,150},780,false}};TestTrue(TEXT("Curve seam is not a traffic junction"),LWRoads33::Junctions(Roads,1).IsEmpty());
 LWGen::FSite Site;Site.Position=FVector2D::ZeroVector;LWGen::FRoad Drive{{0,-3000},LWGen::Entrance(Site),500,false};TestFalse(TEXT("Driveway cap and shoulder clear the entrance footprint"),LWGen::RoadOverlaps(Site,Drive));
 TestEqual(TEXT("Four lane highway"),LWRoads33::Lanes({{0,0},{1,0},1680,true}),4);TestEqual(TEXT("Six lane highway"),LWRoads33::Lanes({{0,0},{1,0},2440,true}),6);
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWRoad33ParcelTest,"LethalWorld.Roads33.FinalRoadClearance",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWRoad33ParcelTest::RunTest(const FString&){int SitesFound=0;TSet<int> Lanes;
 for(int Seed:{198706,731904,-97})for(FIntPoint Cell:{FIntPoint(0,0),FIntPoint(2,-3),FIntPoint(-5,4)}){TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(FVector2D(Cell)*LWGen::RegionSize,Seed,Roads,Sites);SitesFound+=Sites.Num();for(const auto& R:Roads)Lanes.Add(LWRoads33::Lanes(R));for(const auto& S:Sites)TestFalse(TEXT("Final gathered POI clears every road including adjacent region accesses"),Roads.ContainsByPredicate([&](const auto& R){return LWGen::RoadOverlaps(S,R);}));}
 TestTrue(TEXT("Clearance preserves populated world"),SitesFound>20);TestTrue(TEXT("Mixed arterial lane counts"),Lanes.Contains(4)&&Lanes.Contains(6));return true;
}
#endif
