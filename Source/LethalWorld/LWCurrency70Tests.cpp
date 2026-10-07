#include "LWCurrency70.h"
#include "LWGeneration.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCurrency70Test,"LethalWorld.Update70.CurrencyAndToronto",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCurrency70Test::RunTest(const FString&){
 int64 CR=1000,CAD=0,Out=0;
 TestTrue(TEXT("Exchange credits to CAD"),LWCurrency70::Convert(CR,CAD,100,true,1.25,Out));TestEqual(TEXT("CAD payout after spread"),CAD,int64(121));TestEqual(TEXT("Credits debited"),CR,int64(900));
 TestTrue(TEXT("Exchange CAD back"),LWCurrency70::Convert(CR,CAD,121,false,1.25,Out));TestTrue(TEXT("Round trip cannot print money"),CR<1000&&CAD==0);
 const int64 Before=CR;TestFalse(TEXT("Reject negative input"),LWCurrency70::Convert(CR,CAD,-1,true,1.25,Out));TestFalse(TEXT("Reject insufficient balance"),LWCurrency70::Convert(CR,CAD,10000,true,1.25,Out));TestEqual(TEXT("Failed trades do not debit"),CR,Before);
 CAD=MAX_int64;TestFalse(TEXT("Overflow rejected atomically"),LWCurrency70::Convert(CR,CAD,100,true,1.25,Out));TestEqual(TEXT("Overflow retains source"),CR,Before);
 for(int Seed:{7,198706,-917}){
  bool Changed=false;double Last=LWCurrency70::Rate(0,Seed);
  for(int I=1;I<1000;I++){double Now=LWCurrency70::Rate(I*.01,Seed);TestTrue(TEXT("Rate remains in configured range"),Now>=.85&&Now<=1.65);TestTrue(TEXT("Rate changes smoothly"),FMath::Abs(Now-Last)<.03);Changed|=Now!=Last;Last=Now;}
  TestTrue(TEXT("Market fluctuates"),Changed);TestEqual(TEXT("Reload does not reroll exchange rate"),LWCurrency70::Rate(8.21,Seed),LWCurrency70::Rate(8.21,Seed));
  TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(LWGen::CanadaCity68(),Seed,Roads,Sites);
  int Exchanges=0;for(const auto& S:Sites)if(S.Type==65){Exchanges++;TestEqual(TEXT("Exchange has unique Toronto name"),S.PlaceName69,FString(TEXT("TORONTO CURRENCY EXCHANGE")));TestEqual(TEXT("Exchange directions match physical parcel"),S.Position,LWGen::CanadaCity68()+FVector2D(13500,-13500));TestFalse(TEXT("Exchange does not block roads"),Roads.ContainsByPredicate([&](const auto& R){return LWGen::RoadOverlaps(S,R);}));}
  TestEqual(TEXT("Exactly one exchange per city"),Exchanges,1);
  const FVector2D Lake=LWNY69::Project(43.70,-77.70);Roads.Empty();Sites.Empty();LWGen::Gather(Lake,Seed,Roads,Sites);TestTrue(TEXT("Lake Ontario crosses near Canadian border"),LWNY69::WaterDepth(Lake)>0);TestTrue(TEXT("Canadian lake has submerged terrain"),LWGen::Height(Lake,Roads,Sites)<-100);
  TestFalse(TEXT("No roads through Canadian lake"),Roads.ContainsByPredicate([](const auto& R){return LWNY69::WaterDepth((R.A+R.B)*.5)>0;}));
  TestEqual(TEXT("Toronto remains on land"),LWNY69::WaterDepth(LWGen::CanadaCity68()),0.f);for(const auto& C:LWGeography84::Checkpoints())TestFalse(TEXT("Customs approach stays outside restricted territory"),LWGeography84::Restricted(C.Position+FVector2D(-1100,0).GetRotated(C.Yaw)));
 }
 auto* Save=NewObject<ULWSaveGame>();TestEqual(TEXT("Old/new wallets default to zero CAD"),Save->RPG.Canada68.CanadianDollars,int64(0));Save->RPG.Canada68.CanadianDollars=9876543210LL;
 TArray<uint8> Bytes;TestTrue(TEXT("Serialize CAD wallet"),UGameplayStatics::SaveGameToMemory(Save,Bytes));auto* Loaded=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));TestTrue(TEXT("CAD round-trips through save format"),Loaded&&Loaded->RPG.Canada68.CanadianDollars==9876543210LL);
 return true;
}
#endif
