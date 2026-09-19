#include "LWGeneration.h"
#include "LWPOISurvivors.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWPOIExpansionTest,"LethalWorld.Generation.ExpandedPOIReservations",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWPOIExpansionTest::RunTest(const FString& Parameters){

 for(int Type:{0,1,3,4,10,11,12,13,14,16,58,59,60,61,62,63}){LWGen::FSite S;S.Type=Type;TestTrue(TEXT("Appropriate roadside POI eligible"),LWPOISurvivors::Eligible(S));S.SettlementBuilding=true;TestFalse(TEXT("Settlement cast not duplicated"),LWPOISurvivors::Eligible(S));}
 for(int Type:{2,5,18,19,20,21,22,24,31,32,33,53,54,55,56,57}){LWGen::FSite S;S.Type=Type;TestFalse(TEXT("Authored or dangerous site excluded"),LWPOISurvivors::Eligible(S));}
 for(uint32 Id:{303000u,303001u,909090u}){TSet<FString> Names;for(int Role=0;Role<3;Role++){const FString N=LWPOISurvivors::Name(Id,Role);TestEqual(TEXT("Survivor name deterministic"),N,LWPOISurvivors::Name(Id,Role));Names.Add(N);}TestEqual(TEXT("Three distinct survivor names"),Names.Num(),3);}
 TSet<int> Types;
 for(int Seed:{198706,-97})for(int Y=-8;Y<=8;Y++)for(int X=-8;X<=8;X++){
  TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Region(FIntPoint(X,Y),Seed,Roads,Sites);
  for(const auto& S:Sites)if(LWPlaces::Expanded(S.Type)){
   Types.Add(S.Type);
   TestEqual(TEXT("Declared footprint"),S.Size,LWPlaces::Size(S.Type));
   TestTrue(TEXT("Connected entrance"),Roads.ContainsByPredicate([&](const auto& Road){return Road.B.Equals(LWGen::Entrance(S),.1);}));
   TestFalse(TEXT("No roads through expanded footprint"),Roads.ContainsByPredicate([&](const auto& Road){return LWGen::RoadOverlaps(S,Road);}));
   TestFalse(TEXT("No intersecting local parcel"),Sites.ContainsByPredicate([&](const auto& Other){return Other.Id!=S.Id&&LWGen::ParcelsOverlap(S,Other);}));
   for(int DY=-1;DY<=1;DY++)for(int DX=-1;DX<=1;DX++)if(DX||DY){
    TArray<LWGen::FRoad> AdjRoads;TArray<LWGen::FSite> AdjSites;LWGen::Region(FIntPoint(X+DX,Y+DY),Seed,AdjRoads,AdjSites);
    TestFalse(TEXT("No neighboring reservation collision"),AdjSites.ContainsByPredicate([&](const auto& Other){return LWGen::ParcelsOverlap(S,Other);}));
    TestFalse(TEXT("No neighboring road collision"),AdjRoads.ContainsByPredicate([&](const auto& Road){return LWGen::RoadOverlaps(S,Road);}));
   }
   TestEqual(TEXT("Level foundation"),LWGen::Height(S.Position,Roads,Sites),0.f);
  }
 }
 for(int Type=58;Type<=63;Type++)TestTrue(*FString::Printf(TEXT("Type %d appears naturally"),Type),Types.Contains(Type));
 return true;
}
#endif
