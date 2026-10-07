#include "LWPrompts78.h"
#include "LWCampaign76.h"
#include "LWCampaign76Corridor.h"
#include "LWNewYork69.h"
#include "LWBoss48.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWPrompts78Test,"LethalWorld.Update78.ActionPrompts",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWPrompts78Test::RunTest(const FString&){
 auto* Input=NewObject<ULWPlayerInput51>();Input->Bindings={{EKeys::E,EKeys::F,TEXT("Interact")},{EKeys::F,EKeys::H,TEXT("Flashlight")},{EKeys::SpaceBar,EKeys::SpaceBar,TEXT("Jump")},{EKeys::Tab,EKeys::M,TEXT("Map")}};
 TestEqual(TEXT("Actions map once, including combined stand-up prompt"),LWPrompts78::Format(*Input,TEXT("[E] Use [F] Light [E / SPACE] Stand [TAB] Map")),FString(TEXT("[F] Use [H] Light [F / SPACE BAR] Stand [M] Map")));
 Input->Bindings[0].Physical=EKeys::G;TestEqual(TEXT("Remapped interaction propagates"),LWPrompts78::Format(*Input,TEXT("[E] Talk")),FString(TEXT("[G] Talk")));
 TestEqual(TEXT("Numbers and ordinary prose remain untouched"),LWPrompts78::Format(*Input,TEXT("[3/5] FUEL")),FString(TEXT("[3/5] FUEL")));
 Input->Controller58=true;TestEqual(TEXT("Controller action hint"),LWPrompts78::Format(*Input,TEXT("[E]")),TEXT("[")+Input->Prompt58(EKeys::E)+TEXT("]"));return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWApproach78Test,"LethalWorld.Update78.StartingNeighborhood",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWApproach78Test::RunTest(const FString&){
 const FVector2D Start=LWCampaign76::Site(TEXT("bellwether"));float RoadDistance=FLT_MAX;int Neighbors=0;
 for(const auto& R:LWNY69::Roads())RoadDistance=FMath::Min(RoadDistance,LWGen::DistanceToSegment(Start,R));
 TestTrue(TEXT("Public road within 30m of starting building"),RoadDistance<3000);
 for(const auto& S:LWNY69::Sites())if(S.Id>=0xEC78A000u&&S.Id<0xEC78A004u){Neighbors++;TestTrue(TEXT("Useful neighbor within 70m"),(S.Position-Start).Size()<7000);TestEqual(TEXT("New parcel is dry"),LWNY69::WaterDepth(S.Position),0.f);for(const auto& R:LWNY69::Roads())TestFalse(TEXT("Road does not cross new building footprint"),LWGen::RoadOverlaps(S,R));}
 TestEqual(TEXT("Four nearby destinations"),Neighbors,4);return true;
}
#endif
