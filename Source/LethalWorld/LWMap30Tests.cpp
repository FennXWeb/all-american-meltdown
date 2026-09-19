#include "LWSiteIdentity.h"
#include "Misc/AutomationTest.h"
#include "Engine/Texture2D.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWMapIdentityTest,"LethalWorld.Map.PlaceIdentity",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWMapIdentityTest::RunTest(const FString&) {
 LWGen::FSite S;S.Id=1234;S.Type=1;S.Position=FVector2D(-125004,850100);
 const FString Name=LWSites::Name(S);
 TestEqual(TEXT("name is deterministic"),Name,LWSites::Name(S));
 auto Collision=S;Collision.Position.X+=2000;TestNotEqual(TEXT("same hash at another location remains distinct"),Name,LWSites::Name(Collision));
 Collision=S;Collision.Position.X*=-1;TestNotEqual(TEXT("negative and positive coordinates differ"),Name,LWSites::Name(Collision));
 TSet<FString> Names;
 for(int I=0;I<1000;I++){S.Position=FVector2D(I*2300.,(I%7)*1700.);S.Id=LWGen::Hash(I,7,913);S.Type=I%22;Names.Add(LWSites::Name(S));}
 TestEqual(TEXT("one thousand locations retain distinct full names"),Names.Num(),1000);
 S.Type=LWLandmarks::First;TestEqual(TEXT("authored singleton names preserved"),LWSites::Name(S),FString(LWLandmarks::Get(S.Type).Name));
 for(int I=0;I<LWPlaces::Count;I++){S.Type=I;TestFalse(TEXT("every type has a label"),LWSites::Label(S).IsEmpty());}
 return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWMapAtlasTest,"LethalWorld.Map.POIIconAsset",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWMapAtlasTest::RunTest(const FString&) {
 auto* Atlas=LoadObject<UTexture2D>(nullptr,TEXT("/Game/Art/Textures/T_POIAtlas30.T_POIAtlas30"));
 if(!TestNotNull(TEXT("generated POI atlas is imported"),Atlas))return false;
 #if WITH_EDITORONLY_DATA
 TestEqual(TEXT("square source atlas"),Atlas->Source.GetSizeX(),Atlas->Source.GetSizeY());TestTrue(TEXT("adequate source icon resolution"),Atlas->Source.GetSizeX()>=1024);
#endif
 TestTrue(TEXT("all POI types fit the atlas"),LWPlaces::Count<=64);
 return true;
}
#endif
