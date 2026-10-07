#include "LWNewGame72.h"
#include "LWNewYork69.h"
#include "LWGeneration.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWStart72,"LethalWorld.Update72.Start",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWStart72::RunTest(const FString&){
 const auto P=LWNewGame72::StartXY();TestTrue(TEXT("Starts within Syracuse"),(P-LWNY69::Project(43.0481,-76.1474)).Size()<200000);
 TestEqual(TEXT("Dry ground"),LWNY69::WaterDepth(P),0.f);
 LWGen::FSite Spawn;Spawn.Position=P;Spawn.Size=FVector2D(180);
 for(const auto& S:LWNY69::Sites())if(S.Type!=75)TestFalse(TEXT("Spawn clear of building parcels"),LWGen::ParcelsOverlap(Spawn,S)); // The public plaza is the intentional spawn surface.
 for(int I=0;I<9;I++)TestFalse(TEXT("No old campaign reservation"),LWStory::Reserved(LWStory::Site(I),6000));
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWRetire72,"LethalWorld.Update72.LegacySave",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWRetire72::RunTest(const FString&){
 FLWRPGState R;R.Story.Enabled=true;R.Story.GearHeld=true;R.Story.Stage=23;R.XP=1234;
 auto Gun=LWItems::Make(TEXT("rifle"));auto Mag=LWItems::Make(TEXT("mag_rifle30"));Mag.Slot=TEXT("Loaded");Mag.Rounds=17;Gun.LoadedMagazine=Mag.Id;R.Story.Confiscated={Gun,Mag};
 R.KnownPlaces.Add(0xEF320008u,FVector(1));R.KnownPlaces.Add(123,FVector(2));
 R.Respawn.Enabled=true;R.Respawn.Position=FVector(LWStory::Site(8),100);
 TArray<FLWItemInstance> Inv,Stash,Overflow;LWNewGame72::RetireStory(R,Inv,Stash,Overflow);
 TestFalse(TEXT("Campaign disabled"),R.Story.Enabled);TestFalse(TEXT("Prison escrow cleared"),R.Story.GearHeld);
 TestEqual(TEXT("All possessions returned"),Inv.Num()+Stash.Num()+Overflow.Num(),2);
 const auto* M=Inv.FindByPredicate([&](const auto& V){return V.Id==Mag.Id;});TestTrue(TEXT("Magazine ammunition preserved"),M&&M->Rounds==17);
 TestFalse(TEXT("Removed fortress cannot be respawn"),R.Respawn.Enabled);TestEqual(TEXT("Progression retained"),R.XP,int64(1234));
 TestFalse(TEXT("Story map pin removed"),R.KnownPlaces.Contains(0xEF320008u));TestTrue(TEXT("Other map pins retained"),R.KnownPlaces.Contains(123));
 R.Respawn.Enabled=true;R.Respawn.Position=FVector(LWStory::Site(8),100);LWNewGame72::RetireStory(R,Inv,Stash,Overflow);TestTrue(TEXT("New non-story respawn at reused land retained"),R.Respawn.Enabled);TestEqual(TEXT("Migration idempotent"),Inv.Num()+Stash.Num()+Overflow.Num(),2);
 return true;
}
#endif
