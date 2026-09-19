#include "LWBlood40.h"
#include "LWZombie.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/DamageEvents.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "EngineUtils.h"
#include "PhysicsEngine/PhysicsConstraintComponent.h"

namespace {
UStaticMeshComponent* Drop(AActor* A,ALWWorld* W,FVector P,FVector Scale){
 auto* C=NewObject<UStaticMeshComponent>(A);A->AddInstanceComponent(C);
 C->SetStaticMesh(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));
 C->SetMaterial(0,W->Material(TEXT("Blood22")));C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 C->SetCanEverAffectNavigation(false);C->SetCastShadow(false);C->RegisterComponent();
 C->SetWorldLocation(P);C->SetWorldScale3D(Scale);return C;
}

FVector Contact(ALWZombie* Z,const FDamageEvent& E){
 if(E.IsOfType(FPointDamageEvent::ClassID))return static_cast<const FPointDamageEvent&>(E).HitInfo.ImpactPoint;
 return Z->Parts[0]->Bounds.Origin;
}
}
void ALWZombie::BloodHit(const FDamageEvent& E,AActor* Causer,float Damage){
 if(!World||Kind==ELWEnemyKind::Mannequin||GetWorld()->GetTimeSeconds()-BloodTime<.12f)return;
 BloodTime=GetWorld()->GetTimeSeconds();World->BloodEffects40.RemoveAll([](const auto& P){return !P.IsValid();});if(World->BloodEffects40.Num()>=48)return;
 const FVector P=Contact(this,E),Dir=Causer?(P-Causer->GetActorLocation()).GetSafeNormal():GetActorForwardVector();
 if(auto* FX=GetWorld()->SpawnActor<ALWBlood40>()){World->BloodEffects40.Add(FX);FX->Init(World,P,Dir,Damage);}
}
void ALWZombie::Dismember(const FDamageEvent& E,AActor* Causer,float Damage){
 if(Kind==ELWEnemyKind::WorldEater)return;
 if(!World||(!bDead&&!E.IsOfType(FPointDamageEvent::ClassID)&&!E.IsOfType(FRadialDamageEvent::ClassID)))return;
 const FVector P=Contact(this,E);int Pick=-1;double Best=DBL_MAX;
 if(E.IsOfType(FPointDamageEvent::ClassID)){
  const auto* C=static_cast<const FPointDamageEvent&>(E).HitInfo.GetComponent();
  for(int I=0;I<Parts.Num();I++)if(C==Parts[I]){if(I==0||I==2||Missing(I))return;Pick=I;break;}
 }
 const bool Exact=Pick>=0;
 for(int I:{1,3,4,5,6})if(!Exact&&!(SeveredMask&(1<<I))){double D=FVector::DistSquared(P,Parts[I]->Bounds.Origin);if(D<Best){Best=D;Pick=I;}}
 if(Pick<0)return;LimbDamage[Pick]+=Damage;if(LimbDamage[Pick]<(bDead?45.f:(Pick==1?145.f:95.f)*FMath::Sqrt(BodyMassScale())*(1.f+Stars*.8f)))return;
 SeveredMask|=1<<Pick;UStaticMeshComponent* Part=Parts[Pick];
 if(!bDead){
  Part->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
  Part->SetCollisionObjectType(ECC_PhysicsBody);Part->SetCollisionResponseToAllChannels(ECR_Ignore);Part->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);Part->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);Part->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Block);
  Part->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);Part->SetSimulatePhysics(true);Part->SetUseCCD(true);Part->SetLinearDamping(.4f);Part->SetAngularDamping(1.8f);Part->SetMassOverrideInKg(NAME_None,(Pick==1?5:Pick>=5?11:4)*BodyMassScale());
  if(Pick==1){HeadlessTime=FMath::FRandRange(3.f,10.f);HeadlessDirection=GetActorForwardVector();}
  if(Gun&&!CanUseGun())Gun->SetVisibility(false);
 }
 for(UPhysicsConstraintComponent* Joint:RagdollJoints)if(Joint&&Joint->ComponentTags.Contains(FName(*FString::FromInt(Pick))))Joint->BreakConstraint();
 Part->AddImpulse((Causer?(Part->GetComponentLocation()-Causer->GetActorLocation()).GetSafeNormal():GetActorForwardVector())*300+FVector(0,0,100),NAME_None,true);
 if(Kind!=ELWEnemyKind::Mannequin){
  const FVector Seam=Part->GetComponentLocation()+(Pick==1?Part->GetUpVector()*-27:FVector::ZeroVector);
  for(auto* Parent:{Parts[Pick].Get(),Parts[Pick>=5?2:0].Get()}){
   auto* Cap=Drop(this,World,Seam,FVector(.075,.075,.02));Cap->SetWorldRotation(FRotationMatrix::MakeFromZ(Part->GetUpVector()).Rotator());Cap->AttachToComponent(Parent,FAttachmentTransformRules::KeepWorldTransform);
  }
 }
}
