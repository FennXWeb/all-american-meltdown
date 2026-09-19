#include "Misc/AutomationTest.h"
#include "LWSurface26.h"
#include "LWSaveGame.h"
#include "Kismet/GameplayStatics.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWSurface26Test,"LethalWorld.Geometry.CoplanarSubtraction",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWSurface26Test::RunTest(const FString&){using namespace LWSurface26;Polygon A{{0,0},{20,0},{20,20},{0,20}},B{{5,5},{15,5},{15,15},{5,15}};auto Pieces=Subtract(A,B);double Sum=0;for(auto& P:Pieces)Sum+=Area(P);TestTrue(TEXT("overlap area removed without losing surrounding floor"),FMath::IsNearlyEqual(Sum,300.,.01));TestTrue(TEXT("identical faces leave no duplicate"),Subtract(A,A).IsEmpty());Polygon Far{{30,30},{40,30},{40,40},{30,40}};Sum=0;for(auto P:Subtract(A,Far))Sum+=Area(P);TestEqual(TEXT("disjoint floor remains intact"),Sum,400.);return true;}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWDiscoverySave26,"LethalWorld.Exploration.DiscoverySave",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWDiscoverySave26::RunTest(const FString&){auto* S=NewObject<ULWSaveGame>();S->RPG.KnownPlaces.Add(4000000000LL,FVector(1234,5678,0));S->RPG.PlaceNames.Add(4000000000LL,TEXT("Test Town"));TArray<uint8> Data;TestTrue(TEXT("serialize"),UGameplayStatics::SaveGameToMemory(S,Data));auto* L=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Data));if(!TestNotNull(TEXT("restore"),L))return false;TestTrue(TEXT("unsigned site ID preserved"),L->RPG.KnownPlaces.Contains(4000000000LL));TestEqual(TEXT("name preserved"),L->RPG.PlaceNames.FindRef(4000000000LL),FString(TEXT("Test Town")));return true;}
#endif
