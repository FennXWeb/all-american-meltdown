#include "LWWeaponMods.h"
#include "LWAppearance.h"
#include "LWCreator35.h"
#include "Engine/StaticMesh.h"
#include "Misc/AutomationTest.h"
#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWOptics63,"LethalWorld.Visual63.SightCompatibilityAndAim",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWOptics63::RunTest(const FString&){
 const FName Sights[]={TEXT("att_micro63"),TEXT("att_tube63"),TEXT("att_prism63"),TEXT("att_combat63"),TEXT("att_marksman63")};float Last=100;
 for(FName Id:Sights){auto G=LWItems::Make(TEXT("rifle"));G.Attachments.Add(TEXT("Optic"),Id);TestEqual(TEXT("sight is obtainable attachment"),LWItems::Def(Id).Category,FName(TEXT("Attachment")));TestTrue(TEXT("rifle compatibility"),LWMods::Compatible(G.Definition,Id));TestFalse(TEXT("bat rejects sights"),LWMods::Compatible(TEXT("bat"),Id));TestTrue(TEXT("each magnification narrows FOV"),LWMods::Fov(&G)<Last);Last=LWMods::Fov(&G);TestTrue(TEXT("exactly one aiming presentation"),LWMods::OpenSight(&G)!=LWMods::Scope(&G));
 if(LWMods::OpenSight(&G)){FTransform Mesh(FRotator(-2,3,1),FVector(5,2,-3));FVector Rest(20,0,0);FRotator Rot;LWMods::AlignOpenSight(&G,6,Mesh,Rest,Rot);FVector Centre=Rot.RotateVector(Mesh.TransformPosition(LWMods::Position(6,TEXT("Optic"))+FVector(0,0,LWMods::SightHeight(&G))))+Rest;TestTrue(TEXT("reticle axis centred"),FMath::Abs(Centre.Y)<.001&&FMath::Abs(Centre.Z)<.001);}}
 TestFalse(TEXT("pistol rejects large marksman scope"),LWMods::Compatible(TEXT("desert_eagle"),TEXT("att_marksman63")));TestTrue(TEXT("pistol accepts micro optic"),LWMods::Compatible(TEXT("desert_eagle"),TEXT("att_micro63")));return true;
}
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FLWFace63,"LethalWorld.Visual63.FaceUVPreservation",EAutomationTestFlags::EditorContext|EAutomationTestFlags::EngineFilter)
bool FLWFace63::RunTest(const FString&){FLWIdentity V;LWCreator35::Normalize(V);FProcMeshSection S;FProcMeshVertex X;X.Position=FVector(9,3.2,-3.52);X.UV0=FVector2D(.345,.444);S.ProcVertexBuffer.Add(X);V.Shapes35[17]=1;V.Shapes35[20]=0;LWCreator35::Surface(S,1,V);TestTrue(TEXT("morph carries UV with geometry"),S.ProcVertexBuffer[0].UV0.Equals(X.UV0));TestTrue(TEXT("eye spacing morph remains active"),!S.ProcVertexBuffer[0].Position.Equals(X.Position));for(const TCHAR* Sex:{TEXT("Male"),TEXT("Female")}){auto* M=LWAppearance::Mesh61(FString(Sex)+TEXT("AnatomicalHead35"));TestTrue(TEXT("corrected head library and CPU animation"),M&&M->bAllowCPUAccess&&M->GetPathName().Contains(TEXT("Models63")));}return true;}
#endif
