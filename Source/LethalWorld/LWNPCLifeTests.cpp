#include "LWNPCLife.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWLife34Test,"LethalWorld.Animation.ArticulationAndBlink",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWLife34Test::RunTest(const FString&){
 TestEqual(TEXT("blink starts open"),ULWNPCLife::BlinkShape(0),0.f);
 TestTrue(TEXT("blink fully closes"),ULWNPCLife::BlinkShape(.095f)>.99f);
 TestEqual(TEXT("blink finishes"),ULWNPCLife::BlinkShape(.3f),0.f);
 const FVector Shoulder(0,0,0),Hand(0,0,-60),Joint(0,0,-27);
 TestTrue(TEXT("elbow keeps upper arm fixed"),ULWNPCLife::Bend(Shoulder,-27,-90).Equals(Shoulder));
 auto Bent=ULWNPCLife::Bend(Hand,-27,-90);
 TestTrue(TEXT("elbow moves hand forward"),Bent.X>32);
 TestTrue(TEXT("elbow preserves forearm length"),FMath::IsNearlyEqual(FVector::Distance(Bent,Joint),33.,.001));
 TestTrue(TEXT("straight limb unchanged"),ULWNPCLife::Bend(Hand,-27,0).Equals(Hand));
 for(int A=-120;A<=120;A+=10)for(int Z=-85;Z<=0;Z++){auto V=ULWNPCLife::Bend(FVector(2,3,Z),-39,A);TestFalse(TEXT("finite articulated surface"),V.ContainsNaN());}
 return true;
}
#endif
