#include "LWCampaign76.h"
#include "LWCampaign76Corridor.h"
#include "LWGeneration.h"
#include "LWNavigation.h"
#include "LWDestiny71.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCampaign76Continuity,"LethalWorld.Update76.BranchContinuity",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCampaign76Continuity::RunTest(const FString&){
 TSet<FName> Scenes,Stages,People;for(const auto& S:LWCampaign76::Scenes()){TestFalse(TEXT("Unique scene"),Scenes.Contains(S.Id));Scenes.Add(S.Id);}for(const auto& S:LWCampaign76::Stages()){TestFalse(TEXT("Unique stage"),Stages.Contains(S.Id));Stages.Add(S.Id);}for(const auto& C:LWCampaign76::Cast())People.Add(C.Id);
 TestTrue(TEXT("Bellwether is the opening"),Stages.Contains(TEXT("bell_lights")));
 for(const auto& S:LWCampaign76::Scenes()){
  TestTrue(TEXT("Scene continuation exists"),S.Next.IsNone()||Stages.Contains(S.Next));
  for(const auto& B:S.Beats)TestTrue(TEXT("Every speaker has a character record"),B.Who.IsNone()||People.Contains(B.Who));
  for(const auto& C:S.Choices){TestTrue(TEXT("Response scene exists"),C.Next.IsNone()||Scenes.Contains(C.Next));TestTrue(TEXT("Response stage exists"),C.Stage.IsNone()||Stages.Contains(C.Stage));}
 }
 for(const auto& S:LWCampaign76::Stages()){TestTrue(TEXT("Arrival scene exists"),S.Arrival.IsNone()||Scenes.Contains(S.Arrival));TestTrue(TEXT("Continuation exists"),S.Next.IsNone()||Stages.Contains(S.Next));for(const auto& A:S.Actions){TestTrue(TEXT("Action scene exists"),A.Scene.IsNone()||Scenes.Contains(A.Scene));TestTrue(TEXT("Action stage exists"),A.Next.IsNone()||Stages.Contains(A.Next));}}
 FLWCampaign76State S;LWCampaign76::Apply(S,{TEXT("heat=2"),TEXT("clinic=1"),TEXT("dead_lena"),TEXT("trust_mara+=2")});
 TestTrue(TEXT("Fuel predicates"),LWCampaign76::Meets(S,{TEXT("heat>=2"),TEXT("clinic=1"),TEXT("!comms")}));TestFalse(TEXT("Lena stays dead"),LWCampaign76::Alive(S,TEXT("lena")));
 const auto* After=LWCampaign76::Scene(TEXT("bell_after"));int Surviving=0,Dead=0;for(const auto& B:After->Beats)if(LWCampaign76::Meets(S,B.Needs)){Surviving+=B.Who==TEXT("lena");Dead+=B.Text.Contains(TEXT("Lena died"));}TestEqual(TEXT("Dead Lena cannot deliver the surviving witness line"),Surviving,0);TestEqual(TEXT("Death is acknowledged"),Dead,1);
 auto* Save=NewObject<ULWSaveGame>();Save->RPG.Campaign76=S;Save->RPG.Campaign76.Started=true;Save->RPG.Campaign76.Scene=TEXT("fuel");Save->RPG.Campaign76.Beat=3;Save->RPG.Campaign76.Health.Add(TEXT("mara"),47);Save->RPG.Campaign76.Defeated.Add(761);Save->RPG.Campaign76.Rewards.Add(TEXT("parcel"));
 TArray<uint8> Bytes;TestTrue(TEXT("Campaign uses existing save serialization"),UGameplayStatics::SaveGameToMemory(Save,Bytes));auto* Loaded=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));TestNotNull(TEXT("Snapshot loads"),Loaded);
 if(Loaded){const auto& C=Loaded->RPG.Campaign76;TestEqual(TEXT("Scene cursor survives"),C.Beat,3);TestEqual(TEXT("Scene identity survives"),C.Scene,FName(TEXT("fuel")));TestFalse(TEXT("Death survives roundtrip"),LWCampaign76::Alive(C,TEXT("lena")));TestEqual(TEXT("Wounds survive roundtrip"),C.Health.FindRef(TEXT("mara")),47.f);TestTrue(TEXT("Defeated attackers survive roundtrip"),C.Defeated.Contains(761));TestTrue(TEXT("Reward ledger survives"),C.Rewards.Contains(TEXT("parcel")));}
 TestTrue(TEXT("Existing survivors do not acquire campaign progress"),FLWCampaign76State().Stage.IsNone()&&!FLWCampaign76State().Started);
 LWCampaign76::Apply(S,{TEXT("trust_mara-=7")});TestEqual(TEXT("Betrayal subtracts trust"),S.Values.FindRef(TEXT("trust_mara")),-5);
 FLWCampaign76State End;LWCampaign76::Apply(End,{TEXT("richardson_dead"),TEXT("coalition=3"),TEXT("logistics=1"),TEXT("canal_control=2"),TEXT("guard_support=1"),TEXT("imani_leads"),TEXT("freehold_refuge=1"),TEXT("evidence_secured"),TEXT("reserves_secured")});TestEqual(TEXT("Viable independent relief"),LWCampaign76::Ending(End),FString(TEXT("Open Country")));
 End.Values.Add(TEXT("postwar_control"),1);TestEqual(TEXT("Concentrated power"),LWCampaign76::Ending(End),FString(TEXT("New Uniforms")));
 End.Values.Add(TEXT("civilian_losses"),3);TestEqual(TEXT("Losses cannot be erased by a coalition"),LWCampaign76::Ending(End),FString(TEXT("A Victory of Ash")));
 End=FLWCampaign76State();LWCampaign76::Apply(End,{TEXT("loyalist"),TEXT("leverage=1")});TestEqual(TEXT("Loyal success is possible"),LWCampaign76::Ending(End),FString(TEXT("First Citizen")));
 End.Values.Add(TEXT("leverage"),0);TestEqual(TEXT("Discarded only without leverage or support"),LWCampaign76::Ending(End),FString(TEXT("Useful Until Dawn")));
 LWCampaign76::Apply(End,{TEXT("takeover"),TEXT("richardson_dead"),TEXT("leverage=3"),TEXT("loyal_personnel"),TEXT("reserve_control")});TestEqual(TEXT("Earned takeover"),LWCampaign76::Ending(End),FString(TEXT("The Inheritance")));
 End.Values.Add(TEXT("reserve_control"),0);TestNotEqual(TEXT("A last-minute answer cannot invent reserves"),LWCampaign76::Ending(End),FString(TEXT("The Inheritance")));
 End.Values.Add(TEXT("remain_canada"),1);TestEqual(TEXT("Safe optional ending"),LWCampaign76::Ending(End),FString(TEXT("The Other Shore")));
 TSet<int32> EntryRoutes;for(const auto& C:LWCampaign76::Scene(TEXT("northbank_intake"))->Choices){FLWCampaign76State V;LWCampaign76::Apply(V,C.Effects);EntryRoutes.Add(V.Values.FindRef(TEXT("entry_route")));}TestEqual(TEXT("Three alternative routes"),EntryRoutes.Num(),3);
 S.Values.Add(TEXT("dead_simon"),1);int Available=0;for(const auto& C:LWCampaign76::Scene(TEXT("weller_intro"))->Choices)if(LWCampaign76::Meets(S,C.Needs)){Available++;TestTrue(TEXT("Dead witness has an alternate source"),C.Effects.Contains(TEXT("witness_lost")));}TestEqual(TEXT("Witness death does not softlock"),Available,1);
 S.Values.Add(TEXT("dead_rook"),1);Available=0;for(const auto& C:LWCampaign76::Scene(TEXT("rook_confront"))->Choices)if(LWCampaign76::Meets(S,C.Needs)){Available++;TestTrue(TEXT("Replacement command has distinct consequences"),C.Effects.Contains(TEXT("guard_fragmented")));}TestEqual(TEXT("Guard leader death does not softlock"),Available,1);
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCampaign76Geography,"LethalWorld.Update76.Corridor",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCampaign76Geography::RunTest(const FString&){
 for(const auto& Stop:LWCampaign76::Corridor()){
  TestTrue(*FString::Printf(TEXT("%s has an authored parcel"),*Stop.Id.ToString()),LWCampaign76::SiteId(Stop.Id)!=0);
  TestTrue(*FString::Printf(TEXT("%s stays on the American side"),*Stop.Id.ToString()),!LWGeography84::Canada(Stop.Position));
  TestEqual(*FString::Printf(TEXT("%s is dry land"),*Stop.Id.ToString()),LWNY69::WaterDepth(Stop.Position),0.f);
  float RoadDistance=FLT_MAX;for(const auto& R:LWNY69::Roads())RoadDistance=FMath::Min(RoadDistance,LWGen::DistanceToSegment(Stop.Position,R));
  TestTrue(*FString::Printf(TEXT("%s road access within parcel approach"),*Stop.Id.ToString()),RoadDistance<22000);
  bool Connected=false;LWNavigation::FindPath(LWCampaign76::Corridor()[0].Position,Stop.Position,17,&Connected);TestTrue(FString::Printf(TEXT("%s connected to story road network"),*Stop.Id.ToString()),Connected);
 }
 const int I=LWCampaign76::MarketZone();TestTrue(TEXT("Destiny has a valid clinic storefront"),I!=INDEX_NONE);
 if(I!=INDEX_NONE){const auto& Z=LWDestiny71::Data().Zones[I];for(int X:{-1,0,1})for(int Y:{-1,0,1})TestTrue(TEXT("Market set is supported by a real floor"),LWDestiny71::FloorAt(Z.Center+FVector2D(X*850,Y*650),Z.Level));}
 TestTrue(TEXT("Toronto lies in Canada"),LWGeography84::Canada(FVector2D(LWCampaign76::Frame(TEXT("canada")).GetLocation())));
 TArray<LWGen::FRoad> CanadianRoads;TArray<LWGen::FSite> CanadianSites;LWGen::Gather(LWGen::CanadaCity68(),42,CanadianRoads,CanadianSites);TestTrue(TEXT("Northbank has a physical reserved parcel"),CanadianSites.ContainsByPredicate([](const auto& S){return S.Id==0xEC76FF00u;}));
 return true;
}
#endif
