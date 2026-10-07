#include "Misc/AutomationTest.h"
#include "LWGeneration.h"
#include "LWPlayerInput51.h"
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWBorder51Test,"LethalWorld.Update51.NorthernBoundary",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWBorder51Test::RunTest(const FString&){
 for(const auto& C:LWGeography84::Checkpoints())for(int Seed:{198706,7,123456}){
  TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(C.Position,Seed,Roads,Sites);
  TestTrue(TEXT("Mapped crossing has a visible road connection"),Roads.ContainsByPredicate([&](const auto& R){return LWGen::DistanceToSegment(C.Position,R)<1000;}));
  for(const auto& S:Sites)TestEqual(TEXT("Border-adjacent sites stand on dry land"),LWNY69::WaterDepth(S.Position),0.f);
  TestFalse(TEXT("Guard approach is neutral"),LWGeography84::Restricted(C.Position+FVector2D(-1100,0).GetRotated(C.Yaw)));
  TestTrue(TEXT("Passing checkpoint stripe is restricted"),LWGeography84::Restricted(C.Position+FVector2D(700,0).GetRotated(C.Yaw)));
 }
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWKeys51Test,"LethalWorld.Update51.Keybindings",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWKeys51Test::RunTest(const FString&){auto* K=NewObject<ULWPlayerInput51>();K->Load51();for(auto& B:K->Bindings)B.Physical=B.Logical;int Fire=K->Bindings.IndexOfByPredicate([](const auto& B){return B.Logical==EKeys::LeftMouseButton;});TestTrue(TEXT("Fire binding exists"),Fire>=0);TestTrue(TEXT("Rebind attack to unused key"),K->Assign51(Fire,EKeys::P));TestTrue(TEXT("New key fires"),K->Translate51(EKeys::P)==EKeys::LeftMouseButton);TestFalse(TEXT("Former fire key no longer fires"),K->Translate51(EKeys::LeftMouseButton)==EKeys::LeftMouseButton);TestTrue(TEXT("Conflicts swap"),K->Assign51(Fire,EKeys::R));TestTrue(TEXT("R fires after swap"),K->Translate51(EKeys::R)==EKeys::LeftMouseButton);TestTrue(TEXT("Old physical key now reloads"),K->Translate51(EKeys::P)==EKeys::R);TestFalse(TEXT("Escape is reserved"),K->Assign51(Fire,EKeys::Escape));TestFalse(TEXT("Mouse axes cannot be bound to actions"),K->Assign51(Fire,EKeys::MouseX));TSet<FKey> Used;for(const auto& B:K->Bindings){TestFalse(TEXT("No duplicate physical bindings"),Used.Contains(B.Physical));Used.Add(B.Physical);}return true;}
