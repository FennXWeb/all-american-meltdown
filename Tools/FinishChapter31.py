from pathlib import Path
r=Path('Source/LethalWorld')
def e(n,a,b):
 p=r/n;s=p.read_text();assert a in s,(n,a);p.write_text(s.replace(a,b))
e('LWStory31Smoke.inl','if(D->InScene&&Stage!=19)','if(D->InScene&&P.RPG.Story.Stage==Stage)')
e('LWGameMode.cpp','*It!=TestEnemy&&!It->bDead&&!Cast<ALWResident>(*It)','*It!=TestEnemy&&!It->bDead&&!Cast<ALWResident>(*It)&&!Cast<ALWStoryEnemy>(*It)')
e('LWStory.h',' UPROPERTY() TObjectPtr<class UCameraComponent> Camera;',' UPROPERTY() TObjectPtr<class UCameraComponent> Camera;\n UPROPERTY() TObjectPtr<class UAudioComponent> SceneAudio;')
e('LWStory.cpp','ClearActors(bool All){','ClearActors(bool All){if(SceneAudio){SceneAudio->Stop();SceneAudio->DestroyComponent();SceneAudio=nullptr;}')
e('LWStoryScenes.cpp','#include "Components/StaticMeshComponent.h"','#include "Components/StaticMeshComponent.h"\n#include "Components/AudioComponent.h"')
e('LWStoryScenes.cpp','World->Sound(S==1?TEXT("CarRadio")','SceneAudio=World->Sound(S==1?TEXT("CarRadio")')
e('LWStoryScenes.cpp','void ALWStoryDirector::FinishScene(){','void ALWStoryDirector::FinishScene(){if(SceneAudio){SceneAudio->Stop();SceneAudio->DestroyComponent();SceneAudio=nullptr;}')
e('LWStory.cpp','if(N)for(auto& Part:N->Parts)if(Part)Part->SetMaterial(0,World->Material(TEXT("Steel")));','if(N){if(N->Gun)N->Gun->SetVisibility(true);for(auto& Part:N->Parts)if(Part)Part->SetMaterial(0,World->Material(TEXT("Steel")));}')
# A square, correctly UV-oriented banner surface uses the generated atlas material.
e('LWStorySites.cpp','#include "LWLootTable.h"','#include "LWLootTable.h"\n#include "ProceduralMeshComponent.h"')
e('LWStorySites.cpp',' int Serial=0;',''' int BannerIndex=0;
 auto Banner=[&](int Tile,FVector P,float Size=500){auto* Mesh=NewObject<UProceduralMeshComponent>(C);Mesh->SetupAttachment(C->GetRootComponent());Mesh->RegisterComponent();float H=Size*.5f;TArray<FVector> V{P+FVector(-H,0,-H),P+FVector(H,0,-H),P+FVector(H,0,H),P+FVector(-H,0,H)};Mesh->CreateMeshSection(0,V,{0,1,2,0,2,3},{FVector(0,-1,0),FVector(0,-1,0),FVector(0,-1,0),FVector(0,-1,0)},{{0,1},{1,1},{1,0},{0,0}},TArray<FColor>(),TArray<FProcMeshTangent>(),false);Mesh->SetMaterial(0,World->Material(FName(*FString::Printf(TEXT("Chapter31_%d"),Tile))));C->SurfaceMeshes.Add(FName(*FString::Printf(TEXT("StoryBanner%d"),BannerIndex++)),Mesh);};
 int Serial=0;''')
e('LWStorySites.cpp',' C->FlushSurfaces();',' if(I==1)Banner(0,FVector(-2350,-935,560),550);if(I>=3&&I<=7)Banner(1,FVector(0,2780,650),650);if(I==8){Banner(State().Stage>=27?0:2,FVector(-3350,-4210,420),600);Banner(State().Stage>=27?0:3,FVector(0,2010,650),600);}\n C->FlushSurfaces();')
