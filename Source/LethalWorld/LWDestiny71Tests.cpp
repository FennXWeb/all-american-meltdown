#include "LWDestiny71.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWDestiny71Layout,"LethalWorld.Update71.DestinyLayout",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWDestiny71Layout::RunTest(const FString&){
 using namespace LWDestiny71;const auto& D=Data();
 TestEqual(TEXT("Mapped 74-point exterior"),D.Shell.Num(),74);TestEqual(TEXT("Directory escalator banks"),D.Escalators.Num(),11);
 FBox2D Bounds(D.Shell);TestTrue(TEXT("Building retains near-real metric scale"),FMath::IsNearlyEqual(Bounds.GetSize().X,62848.16,2.)&&FMath::IsNearlyEqual(Bounds.GetSize().Y,36234.6,2.));
 TestTrue(TEXT("Mapped access roads and parking aisles retained"),D.Roads.Num()>50);TestEqual(TEXT("Hiawatha bridge footprint"),D.Bridge.Num(),4);
 int Bad=0;TSet<int> Levels;for(const auto& Z:D.Zones){Levels.Add(Z.Level);for(const auto& P:Z.Fixtures)if(!Inside(P,D.Shell))Bad++;}TestEqual(TEXT("Four public shopping levels"),Levels.Num(),4);TestEqual(TEXT("Furniture inside mapped building"),Bad,0);
 int Invalid=0;for(const auto& P:D.Panels){if(P.Indices.Num()%3)Invalid++;for(int I:P.Indices)if(!P.Vertices.IsValidIndex(I)||P.Vertices[I].ContainsNaN())Invalid++;}TestEqual(TEXT("Offline panel triangulation valid"),Invalid,0);
 TestTrue(TEXT("First-level atrium has a floor"),FloorAt(D.Atrium,1));TestFalse(TEXT("Second-level atrium stays open"),FloorAt(D.Atrium,2));TestFalse(TEXT("Third-level atrium stays open"),FloorAt(D.Atrium,3));
 int BadLanding=0;for(const auto& E:D.Escalators){if(!FloorAt(E.P-E.D*720,E.Level)||!FloorAt(E.P+E.D*720,E.Level+1))BadLanding++;}TestEqual(TEXT("Every escalator connects two supported landings"),BadLanding,0);
 return true;
}
#endif
