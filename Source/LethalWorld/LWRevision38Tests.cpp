#include "LWWeaponEffect.h"
#include "LWNavigation.h"
#include "LWGeneration.h"
#include "Misc/AutomationTest.h"
#include "HAL/PlatformTime.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWAim38Test,"LethalWorld.Combat.EnemyAim38",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWAim38Test::RunTest(const FString&){
 int StillHits=0,MovingHits=0;constexpr int Shots=20000;
 for(int I=0;I<Shots;++I){
  for(int Moving=0;Moving<2;++Moving){const FVector D=ALWWeaponEffect::EnemyAim(FVector::ZeroVector,FVector(1800,0,0),Moving?250:0,Moving?500:0);
   TestTrue(TEXT("finite forward shot"),!D.ContainsNaN()&&D.X>0);
   const FVector At=D*(1800/D.X);if(FMath::Abs(At.Y)<35&&FMath::Abs(At.Z)<75){if(Moving)++MovingHits;else ++StillHits;}
  }
 }
 AddInfo(FString::Printf(TEXT("18m target hit rates: stationary %.1f%%, moving %.1f%%"),100.f*StillHits/Shots,100.f*MovingHits/Shots));
 TestTrue(TEXT("stationary enemies hit sometimes but miss most shots at 18m"),StillHits>Shots*.03&&StillHits<Shots*.5);
 TestTrue(TEXT("movement meaningfully reduces accuracy"),MovingHits<StillHits*.65&&MovingHits>0);return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWRoute38Test,"LethalWorld.Navigation.CorridorReuse38",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWRoute38Test::RunTest(const FString&){
 const FVector2D A(2000,3000),B(32000,25000);bool Complete=false;
 double T=FPlatformTime::Seconds();const auto First=LWNavigation::FindPath(A,B,7381,&Complete);double Cold=FPlatformTime::Seconds()-T;
 T=FPlatformTime::Seconds();const auto Again=LWNavigation::FindPath(A,B,7381);double Warm=FPlatformTime::Seconds()-T;
 TestTrue(TEXT("route found"),First.Num()>1);TestTrue(TEXT("cache preserves exact route"),First==Again);
 LWNavigation::FindPath(A+FVector2D(400,350),B+FVector2D(500,650),7381);
 TestTrue(TEXT("projections do not mutate cached topology"),First==LWNavigation::FindPath(A,B,7381));
 const int Old=LWGen::TownDensity;LWGen::TownDensity=0;LWNavigation::FindPath(A,B,7381);LWGen::TownDensity=Old;
 TestTrue(TEXT("settings roundtrip returns original topology"),First==LWNavigation::FindPath(A,B,7381));
 for(int I=0;I<5;++I)LWNavigation::FindPath(A,B,8000+I);
 TestTrue(TEXT("eviction does not change path"),First==LWNavigation::FindPath(A,B,7381));
 AddInfo(FString::Printf(TEXT("Route topology cold %.3f ms, reused %.3f ms"),Cold*1000,Warm*1000));return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWTerrain38Test,"LethalWorld.Generation.Neighborhood38",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWTerrain38Test::RunTest(const FString&){
 const FVector2D P(5000,5000);constexpr int Seed=38118;
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;
 double Start=FPlatformTime::Seconds();
 for(int Y=-1;Y<=1;++Y)for(int X=-1;X<=1;++X)LWGen::Region(FIntPoint(X,Y),Seed,Roads,Sites);
 Sites.RemoveAll([&](const auto& S){return Roads.ContainsByPredicate([&](const auto& R){return LWGen::RoadOverlaps(S,R);});});
 const double Uncached=FPlatformTime::Seconds()-Start;
 const auto& N=LWGen::Neighborhood38(P,Seed);
 TestEqual(TEXT("same road count"),N.Roads.Num(),Roads.Num());TestEqual(TEXT("same sites"),N.Sites.Num(),Sites.Num());
 for(int I=0;I<Sites.Num()&&I<N.Sites.Num();++I)TestEqual(TEXT("same site identity"),N.Sites[I].Id,Sites[I].Id);
 Start=FPlatformTime::Seconds();
 for(int I=0;I<480;++I){const FVector2D Sample=P+FVector2D(I*20,350);const auto& Cached=LWGen::Neighborhood38(Sample,Seed);TestEqual(TEXT("cached height matches original"),LWGen::Height(Sample,Cached.Roads,Cached.Sites),LWGen::Height(Sample,Roads,Sites));}
 AddInfo(FString::Printf(TEXT("Neighborhood first assembly %.3f ms; 480 cached height queries with reference comparison %.3f ms"),Uncached*1000,(FPlatformTime::Seconds()-Start)*1000));
 return true;
}
#endif
