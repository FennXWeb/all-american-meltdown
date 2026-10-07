#include "LWCampaignProduction77.h"
#include "LWCampaign76.h"
#include "LWNewYork69.h"
#include "LWBorder51.h"
#include "LWGeography84.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWProduction77Test,"LethalWorld.Update77.ProductionContinuity",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWProduction77Test::RunTest(const FString&){
 for(FVector Target:{FVector(35,0,-20),FVector(5,22,-45),FVector(0,0,-.1),FVector(100,0,0)}){
  FVector Elbow=LWProduction77::ArmElbow(FVector::ZeroVector,Target,FVector(0,1,0),27,29);TestTrue(TEXT("Elbow remains a fixed upper-arm length"),FMath::Abs(Elbow.Size()-27)<.05);
  if(Target.Size()>2.01&&Target.Size()<55.99)TestTrue(TEXT("Reachable target keeps forearm length"),FMath::Abs(FVector::Dist(Elbow,Target)-29)<.05);
  TestFalse(TEXT("No contact singularity"),Elbow.ContainsNaN());
 }
 TestEqual(TEXT("Contact eases in"),LWProduction77::ContactEnvelope(0,4),0.f);TestEqual(TEXT("Contact releases"),LWProduction77::ContactEnvelope(4,4),0.f);TestEqual(TEXT("Contact is firm at midpoint"),LWProduction77::ContactEnvelope(2,4),1.f);
 FVector Prev=LWProduction77::FerryPose(0).GetLocation();float MaxSpeed=0;
 for(int I=0;I<=FMath::RoundToInt(LWProduction77::FerryDuration()*10);I++){
  const auto Pose=LWProduction77::FerryPose(I*.1f);const FVector At=Pose.GetLocation();
  TestTrue(TEXT("Entire sailing path is real water"),LWNY69::WaterDepth(FVector2D(At))>0);TestFalse(TEXT("Sailing transform finite"),At.ContainsNaN());
  if(I)MaxSpeed=FMath::Max(MaxSpeed,float((At-Prev).Size()/.1));Prev=At;
  for(FVector Local:{FVector(-170,-300,112),FVector(160,-50,112),FVector(0,-600,112)})TestTrue(TEXT("Deck coordinates survive moving-frame conversion"),Pose.InverseTransformPosition(Pose.TransformPosition(Local)).Equals(Local,.001));
 }
 TestFalse(TEXT("Departure stays on the American side"),LWGeography84::Canada(FVector2D(LWProduction77::FerryPose(0).GetLocation())));
 TestTrue(TEXT("Ferry arrives in Ontario"),LWGeography84::Canada(FVector2D(LWProduction77::FerryPose(LWProduction77::FerryDuration()).GetLocation())));TestTrue(TEXT("Ferry never exceeds 12 m/s"),MaxSpeed<1200);
 auto* Save=NewObject<ULWSaveGame>();auto& S=Save->RPG.Campaign76;S.Started=true;S.Sailing77=true;S.FerrySeconds77=52.5;S.CarrierDamage77=6;S.Work77=TEXT("crane_lock");S.WorkSeconds77=2.3;S.Performance77=TEXT("handoff");S.Performer77=TEXT("chen");S.Recipient77=TEXT("lena");S.PerformanceTime77=1.8;S.PerformanceDuration77=4;S.Values.Add(TEXT("route_crowd77_final_evacuation_3"),4);
 TArray<uint8> Bytes;TestTrue(TEXT("Production state serializes"),UGameplayStatics::SaveGameToMemory(Save,Bytes));auto* Load=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));TestNotNull(TEXT("Production save loads"),Load);
 if(Load){const auto& R=Load->RPG.Campaign76;TestTrue(TEXT("Sailing survives reload"),R.Sailing77);TestEqual(TEXT("Sailing clock survives"),R.FerrySeconds77,52.5f);TestEqual(TEXT("Damage state survives"),R.CarrierDamage77,6.f);TestEqual(TEXT("Repair cursor survives"),R.Work77,S.Work77);TestEqual(TEXT("Handoff partner survives"),R.Recipient77,S.Recipient77);TestEqual(TEXT("Crowd route cursor survives"),R.Values.FindRef(TEXT("route_crowd77_final_evacuation_3")),4);}
 for(FName Id:{FName(TEXT("resistance_exit")),FName(TEXT("final_evacuation"))}){const auto* Stage=LWCampaign76::Stage(Id);TestTrue(TEXT("Evacuation completion requires physical arrival"),Stage&&Stage->Actions.ContainsByPredicate([](const auto& A){return A.Needs.Contains(TEXT("crowd_clear77"));}));}
 return true;
}
#endif
