#include "LWAviationSky84.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWGeography84.h"
#include "ProceduralMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "Async/Async.h"
ALWAviationSky84::ALWAviationSky84(){Horizon=CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Regional terrain"));SetRootComponent(Horizon);Horizon->SetCollisionEnabled(ECollisionEnabled::NoCollision);Horizon->SetCastShadow(false);Horizon->SetCanEverAffectNavigation(false);}
void ALWAviationSky84::Ensure(ALWWorld* W,ALWCharacter* P,float Dt){if(!W||!P)return;ALWAviationSky84* Sky=nullptr;for(TActorIterator<ALWAviationSky84> I(W->GetWorld());I;++I){Sky=*I;break;}if(!Sky&&P->Vehicle&&P->Vehicle->IsAircraft84())Sky=W->GetWorld()->SpawnActor<ALWAviationSky84>();if(Sky)Sky->Update(W,P,Dt);}
void ALWAviationSky84::Update(ALWWorld* W,ALWCharacter* P,float Dt){const bool Active=P->Vehicle&&P->Vehicle->IsAircraft84()&&P->GetActorLocation().Z>18000;SetActorHiddenInGame(!Active);if(!Active)return;const FVector Eye=P->GetActorLocation();FVector2D Next(FMath::GridSnap(Eye.X,100000.),FMath::GridSnap(Eye.Y,100000.));
 if(Pending.IsValid()&&Pending.IsReady()){auto Data=Pending.Get();Pending=TFuture<FLWFarTerrain84>();Center=Requested;Horizon->CreateMeshSection(0,Data.V,Data.T,Data.N,Data.UV,Data.C,TArray<FProcMeshTangent>(),false);Horizon->SetMaterial(0,W->Material(TEXT("FarTerrain84")));}
 if(!Pending.IsValid()&&!Center.Equals(Next,100)){Requested=Next;Pending=Async(EAsyncExecution::ThreadPool,[Next](){FLWFarTerrain84 D;constexpr int N=80;constexpr float Cell=25000;for(int X=0;X<=N;X++)for(int Y=0;Y<=N;Y++){const FVector2D At=Next+FVector2D((X-N/2)*Cell,(Y-N/2)*Cell);const bool Water=LWNY69::WaterDepth(At)>0,Canada=LWGeography84::Canada(At);D.V.Add(FVector(At,Water?-90:100+FMath::PerlinNoise2D(At*.000005)*200));D.N.Add(FVector::UpVector);D.UV.Add(At/8000);D.C.Add(Water?FColor(23,52,67):Canada?FColor(52,87,42):FColor(78,75,53));if(X<N&&Y<N&&FVector2D::DistSquared(At+FVector2D(Cell*.5),Next)>FMath::Square(65000.)){int I=X*(N+1)+Y;D.T.Append({I,I+1,I+N+1,I+1,I+N+2,I+N+1});}}return D;});}
 if(Clouds.IsEmpty())for(int I=0;I<3;I++){auto* M=NewObject<UStaticMeshComponent>(this);M->SetupAttachment(Horizon);M->SetStaticMesh(W->Mesh(TEXT("Cube")));M->SetMaterial(0,W->Material(TEXT("CloudBed84")));M->SetCollisionEnabled(ECollisionEnabled::NoCollision);M->SetCastShadow(false);M->SetRelativeScale3D(FVector(120000,120000,.01));M->SetTranslucentSortPriority(I);M->RegisterComponent();Clouds.Add(M);}
 for(int I=0;I<Clouds.Num();I++)Clouds[I]->SetWorldLocation(FVector(Eye.X,Eye.Y,260000+I*15000));
}
