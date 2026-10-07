#include "LWBoss48.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWBossRates48,"LethalWorld.Boss48.RatesAndClearance",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWBossRates48::RunTest(const FString&){int Count[3]={};for(uint32 H=0;H<10000;++H){int K=LWBoss48::Kind(H);if(K>=9)Count[K-9]++;}TestEqual(TEXT("behemoth chunk rolls"),Count[0],15);TestEqual(TEXT("colossus chunk rolls"),Count[1],2);TestEqual(TEXT("worm chunk rolls"),Count[2],3);
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::FSite S;S.Position=FVector2D(1000000,1000000);S.Size=FVector2D(2000,1000);S.Yaw=45;Sites.Add(S);
 TestFalse(TEXT("reject inside POI"),LWBoss48::Clear(S.Position,9,0,Roads,Sites));TestTrue(TEXT("clear outskirts no blanket 70m exclusion"),LWBoss48::Clear(S.Position+FVector2D(4000,0),9,0,Roads,Sites));
 TestFalse(TEXT("worm tail cannot cross POI"),LWBoss48::Clear(S.Position+FVector2D(8100,0),11,0,Roads,Sites));
 Roads.Add({S.Position+FVector2D(-20000,4000),S.Position+FVector2D(20000,4000),700,false});TestFalse(TEXT("reject roadway"),LWBoss48::Clear(S.Position+FVector2D(0,4000),10,0,Roads,Sites));return true;}
#endif
