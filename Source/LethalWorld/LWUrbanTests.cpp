#include "LWGeneration.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWUrbanTest,"LethalWorld.Generation.UrbanDistricts",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWUrbanTest::RunTest(const FString&){
 int Cities=0,Dense=0,Blocked=0,Partial=0,Full=0,Housing=0;TSet<int> Types;
 for(int Seed:{198706,731904,-97})for(int Y=-4;Y<=4;Y++)for(int X=-4;X<=4;X++){
  TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Region({X,Y},Seed,Roads,Sites);int Towers=0;
  for(const auto& S:Sites){Types.Add(S.Type);Housing+=S.District==1;
   TestFalse(TEXT("No building intersects a road"),Roads.ContainsByPredicate([&](const auto& R){return LWGen::RoadOverlaps(S,R);}));
   if(LWPlaces::IsTower(S.Type)){++Towers;Blocked+=S.AccessibleFloors==0;Partial+=S.AccessibleFloors>0&&S.AccessibleFloors<S.Floors;Full+=S.AccessibleFloors==S.Floors;TestTrue(TEXT("Tower access bounded by height"),S.AccessibleFloors>=0&&S.AccessibleFloors<=S.Floors&&S.Floors>=12);TestTrue(TEXT("Towers occur in city districts"),LWGen::IsCity({X,Y},Seed)&&S.District==4);}
  }
  Cities+=LWGen::IsCity({X,Y},Seed);Dense+=Towers>=3;
 }
 AddInfo(FString::Printf(TEXT("Cities=%d dense=%d blocked=%d partial=%d full=%d housing=%d types=%d"),Cities,Dense,Blocked,Partial,Full,Housing,Types.Num()));
 TestTrue(TEXT("Dense downtown groups survive placement"),Dense>=5);TestTrue(TEXT("All access variants exist"),Blocked>0&&Partial>0&&Full>0);TestTrue(TEXT("Housing neighborhoods exist"),Housing>50);for(int T=0;T<21;T++)TestTrue(TEXT("All ordinary urban POI categories remain available"),Types.Contains(T));return true;
}
#endif
