#include "LWCreator35.h"
#include "LWAppearancePreset35.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWCreator35Test,"LethalWorld.Creator35.MorphsAndSavedAppearance",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWCreator35Test::RunTest(const FString&){
 FLWIdentity V;LWCreator35::Normalize(V);TestEqual(TEXT("24 initialized controls"),V.Shapes35.Num(),24);TestEqual(TEXT("six tattoo zones"),V.Tattoos35.Num(),6);
 const FVector Neutral(8,5,-12);TestTrue(TEXT("neutral head is unchanged"),LWCreator35::Morph(Neutral,1,V).Equals(Neutral,.001));
 V.Shapes35[12]=1;TestTrue(TEXT("jaw width changes actual head vertices"),LWCreator35::Morph(Neutral,1,V).Y>Neutral.Y);
 V.Top35=7;V.Bottom35=4;V.Shoes35=3;V.Headwear35=2;V.Gloves35=2;V.Beard35=9;V.Eyewear35=3;V.BeardColor35=7;V.Hair=18;V.Tattoos35[4]=16;V.TattooSize35[4]=.8;V.Shapes35[17]=.8;V.TattooPosition35[4]=FVector2D(.3,.7);V.TattooRotation35[4]=.65f;
 auto* Save=NewObject<ULWAppearancePreset35>();Save->Identity=V;TArray<uint8> Bytes;TestTrue(TEXT("serialize appearance"),UGameplayStatics::SaveGameToMemory(Save,Bytes));auto* Loaded=Cast<ULWAppearancePreset35>(UGameplayStatics::LoadGameFromMemory(Bytes));TestNotNull(TEXT("deserialize appearance"),Loaded);
 if(Loaded){TestTrue(TEXT("tattoo placement persists"),Loaded->Identity.TattooPosition35[4].Equals(FVector2D(.3,.7),.001));TestEqual(TEXT("tattoo rotation persists"),Loaded->Identity.TattooRotation35[4],.65f);TestEqual(TEXT("bottom persists"),Loaded->Identity.Bottom35,4);TestEqual(TEXT("shoes persist"),Loaded->Identity.Shoes35,3);TestEqual(TEXT("beard persists"),Loaded->Identity.Beard35,9);TestEqual(TEXT("gloves persist"),Loaded->Identity.Gloves35,2);TestEqual(TEXT("eyewear persists"),Loaded->Identity.Eyewear35,3);TestEqual(TEXT("beard color persists"),Loaded->Identity.BeardColor35,7);TestEqual(TEXT("tattoo persists"),Loaded->Identity.Tattoos35[4],16);TestEqual(TEXT("face slider persists"),Loaded->Identity.Shapes35[17],.8f);}
 FProcMeshSection TattooEdge;FProcMeshVertex Edge;Edge.Position=FVector(5,25,-37);TattooEdge.ProcVertexBuffer.Add(Edge);LWCreator35::Surface(TattooEdge,3,V);TestTrue(TEXT("tattoo edge UV stays within selected atlas cell"),TattooEdge.ProcVertexBuffer[0].UV1.X>.75&&TattooEdge.ProcVertexBuffer[0].UV1.Y>.75);
 for(int I=0;I<24;I++){LWCreator35::SetShape(V,I,1);for(int Part=0;Part<7;Part++)TestFalse(TEXT("extreme shape remains finite"),LWCreator35::Morph(Neutral,Part,V).ContainsNaN());}
 V.Top35=900;V.Hair=-5;V.Bottom35=-3;V.Tattoos35[0]=500;LWCreator35::Normalize(V);TestEqual(TEXT("invalid top clamped"),V.Top35,8);TestEqual(TEXT("invalid hair clamped"),V.Hair,0);TestEqual(TEXT("invalid tattoo clamped"),V.Tattoos35[0],16);
 auto A=LWCreator35::ResidentIdentity(TEXT("resident_test"),false,false,false),B=LWCreator35::ResidentIdentity(TEXT("resident_test"),false,false,false);TestTrue(TEXT("NPC appearance stable across reload"),A.Shapes35==B.Shapes35&&A.Top35==B.Top35&&A.Hair==B.Hair);
 return true;
}
#endif
