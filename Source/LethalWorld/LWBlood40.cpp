#include "LWBlood40.h"
#include "LWWorld.h"
#include "Engine/StaticMesh.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "Engine/World.h"

ALWBlood40::ALWBlood40(){
 PrimaryActorTick.bCanEverTick=true;PrimaryActorTick.TickInterval=.033f;
 Mesh=CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Blood"));SetRootComponent(Mesh);
 Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);Mesh->SetCanEverAffectNavigation(false);Mesh->SetCastShadow(false);
}
void ALWBlood40::Init(ALWWorld* W,FVector P,FVector Dir,float Damage){
 Tags.Add(TEXT("BloodFX22"));SetActorLocation(P);SetLifeSpan(25);
 Mesh->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));
 Mesh->SetMaterial(0,W->Material(TEXT("Blood22")));
 for(int i=0;i<8;++i){
  Positions.Add(P+FMath::VRand()*8);
  Velocities.Add(Dir*FMath::Clamp(Damage*5,90.f,500.f)+FMath::VRand()*130+FVector(0,0,100));
  Mesh->AddInstance(FTransform(FQuat::Identity,Positions.Last(),FVector(.018,.018,.025)*FMath::FRandRange(.6f,1.5f)),true);
 }
 // Object filtering matters: a trace on the WorldStatic CHANNEL still hits pawns
 // and ragdolls which block that channel. Stains must rest on fixed geometry.
 const FCollisionObjectQueryParams Surfaces(ECC_WorldStatic);
 FCollisionQueryParams Q(NAME_None,false,this);
 for(int i=0;i<5;++i){
  FHitResult H;const FVector At=P+FVector(FMath::FRandRange(-22.f,22.f),FMath::FRandRange(-22.f,22.f),0);
  if(GetWorld()->LineTraceSingleByObjectType(H,At,At-FVector(0,0,500),Surfaces,Q)&&!H.bStartPenetrating&&H.ImpactNormal.Z>.4f)
   Mesh->AddInstance(FTransform(FRotationMatrix::MakeFromZ(H.ImpactNormal).ToQuat(),H.ImpactPoint+H.ImpactNormal*.5,FVector(FMath::FRandRange(.18f,.5f),FMath::FRandRange(.12f,.4f),.008)),true);
 }
}
void ALWBlood40::Tick(float Dt){
 Super::Tick(Dt);Age+=Dt;
 const FCollisionObjectQueryParams Surfaces(ECC_WorldStatic);
 FCollisionQueryParams Q(NAME_None,false,this);bool Moving=false;
 for(int i=0;i<Positions.Num();++i){
  if(Velocities[i].IsNearlyZero())continue;
  FTransform T;Mesh->GetInstanceTransform(i,T,true);
  // Bound effect cost without leaving live droplets frozen in mid-air.
  if(Age>=4.f){Velocities[i]=FVector::ZeroVector;T.SetScale3D(FVector::ZeroVector);}
  else {
   const float Step=FMath::Min(Dt,.1f);Velocities[i].Z-=980*Step;
   const FVector End=Positions[i]+Velocities[i]*Step;FHitResult H;
   if(GetWorld()->LineTraceSingleByObjectType(H,Positions[i],End,Surfaces,Q)){
    Velocities[i]=FVector::ZeroVector;
    if(H.bStartPenetrating)T.SetScale3D(FVector::ZeroVector);
    else {
     Positions[i]=H.ImpactPoint+H.ImpactNormal*.25f;
     T.SetRotation(FRotationMatrix::MakeFromZ(H.ImpactNormal).ToQuat());
     T.SetScale3D(FVector(.055,.04,.004));
    }
   }else{Positions[i]=End;Moving=true;}
   T.SetLocation(Positions[i]);
  }
  Mesh->UpdateInstanceTransform(i,T,true,false,true);
 }
 Mesh->MarkRenderStateDirty();if(!Moving)SetActorTickEnabled(false);
}
