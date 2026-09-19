#include "LWSlotMachine.h"
#include "LWGeneration.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCasinoRules,"LethalWorld.Casino.SlotsAndPlacement",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCasinoRules::RunTest(const FString&){
 const int W[]={5,4,3,2,1};double Return=0;for(int A=0;A<5;A++)for(int B=0;B<5;B++)for(int C=0;C<5;C++){int Award=ALWSlotMachine::Payout(A,B,C);TestTrue(TEXT("bounded nonnegative payout"),Award>=0&&Award<=2000);Return+=Award*W[A]*W[B]*W[C]/3375.;}
 TestTrue(TEXT("slot expected return remains below stake"),Return>9&&Return<10);TestEqual(TEXT("jackpot"),ALWSlotMachine::Payout(4,4,4),2000);TestEqual(TEXT("no match"),ALWSlotMachine::Payout(0,1,2),0);
 int Casinos=0;for(int Y=-6;Y<=6;Y++)for(int X=-6;X<=6;X++){TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Region({X,Y},198706,Roads,Sites);for(const auto& S:Sites)if(S.Type==21){Casinos++;TestTrue(TEXT("resort is a friendly destination"),S.Friendly);TestFalse(TEXT("resort footprint clears roads"),Roads.ContainsByPredicate([&](const auto& R){return LWGen::RoadOverlaps(S,R);}));}}
 AddInfo(FString::Printf(TEXT("Resorts in 169 regions: %d"),Casinos));TestTrue(TEXT("rare resorts survive generation"),Casinos>0&&Casinos<12);for(int Seed:{1,42,198706,999999}){int Nearby=0;TArray<LWGen::FSite> All;TArray<LWGen::FRoad> AllRoads;for(int Y=-2;Y<=2;Y++)for(int X=-2;X<=2;X++)LWGen::Region({X,Y},Seed,AllRoads,All);for(const auto& S:All)if(S.Type==21){Nearby++;TestFalse(TEXT("new resort clears neighboring roads"),AllRoads.ContainsByPredicate([&](const auto& A){return LWGen::RoadOverlaps(S,A);}));TestFalse(TEXT("new resort clears neighboring parcels"),All.ContainsByPredicate([&](const auto& A){return A.Id!=S.Id&&LWGen::ParcelsOverlap(S,A);}));}TestTrue(TEXT("resorts are no longer guaranteed around spawn"),Nearby<4);AddInfo(FString::Printf(TEXT("Seed %d nearby resorts %d"),Seed,Nearby));}return true;
}
#endif
