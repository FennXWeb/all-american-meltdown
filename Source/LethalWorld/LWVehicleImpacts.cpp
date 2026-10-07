#include "LWVehicle.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
void ALWVehicle::HitPedestrians(FVector End,FRotator Rotation){
 const float ImpactSpeed=FMath::Abs(Speed);if(ImpactSpeed<140)return;
 const float Now=GetWorld()->GetTimeSeconds();
 for(auto It=ImpactTimes.CreateIterator();It;++It)if(!It.Key().IsValid()||Now-It.Value()>2)It.RemoveCurrent();
 FCollisionQueryParams Q(NAME_None,false,this);if(Driver)Q.AddIgnoredActor(Driver);if(Chauffeur)Q.AddIgnoredActor(Chauffeur);for(auto& P:Passengers)if(P)Q.AddIgnoredActor(P);
 const FVector Start=Chassis->GetComponentLocation();const auto Shape=FCollisionShape::MakeBox(Chassis->GetScaledBoxExtent());
 // Clip the query at solid scenery: pedestrians behind walls cannot be struck.
 FCollisionObjectQueryParams Solid;Solid.AddObjectTypesToQuery(ECC_WorldStatic);Solid.AddObjectTypesToQuery(ECC_WorldDynamic);
 for(int I=0;I<12&&ImpactSpeed>=350;I++){FHitResult Prop;if(!GetWorld()->SweepSingleByObjectType(Prop,Start,End,Rotation.Quaternion(),Solid,Shape,Q))break;auto* Chunk=Cast<ALWChunk>(Prop.GetActor());if(!Chunk||!Chunk->BreakProp60(Prop.GetComponent(),End-Start,ImpactSpeed))break;Speed*=.96f;AddDent(Prop.ImpactPoint,-(End-Start).GetSafeNormal(),8);}
 FHitResult Wall;if(GetWorld()->SweepSingleByObjectType(Wall,Start,End,Rotation.Quaternion(),Solid,Shape,Q))End=FMath::Lerp(Start,End,Wall.Time);
 TArray<FHitResult> Hits;FCollisionObjectQueryParams Pawns;Pawns.AddObjectTypesToQuery(ECC_Pawn);Pawns.AddObjectTypesToQuery(ECC_PhysicsBody);
 const auto PedestrianShape=FCollisionShape::MakeBox(Chassis->GetScaledBoxExtent()+FVector(0,0,25));
 GetWorld()->SweepMultiByObjectType(Hits,Start,End,Rotation.Quaternion(),Pawns,PedestrianShape,Q);
 const FVector Direction=Rotation.Vector()*(Speed<0?-1:1);
 bool Impact=false;TSet<AActor*> Processed;
 for(const auto& H:Hits){auto* Z=Cast<ALWZombie>(H.GetActor());if(!Z||Processed.Contains(Z))continue;
  if(Z->bDead){if(const float* Last=ImpactTimes.Find(Z))if(Now-*Last<.25f)continue;ImpactTimes.Add(Z,Now);Processed.Add(Z);Z->RagdollForce(Direction*ImpactSpeed+FVector(0,0,FMath::Min(ImpactSpeed*.12f,180.f)),H.ImpactPoint,FMath::Clamp(Spec().HalfLength/205.f,.5f,2.f));Impact=true;continue;}
  if(auto* Part=Cast<UStaticMeshComponent>(H.GetComponent()))if(Part->IsSimulatingPhysics()){Part->WakeAllRigidBodies();Part->AddImpulse(Direction*ImpactSpeed*.6f,NAME_None,true);continue;}
  Processed.Add(Z);
  if(auto* N=Cast<ALWResident>(Z))if(N->Riding||N->DownTime>0)continue;
  if(const float* Last=ImpactTimes.Find(Z))if(Now-*Last<1)continue;
  ImpactTimes.Add(Z,Now);FPointDamageEvent E;E.Damage=FMath::Clamp((ImpactSpeed-100)*.14f,5.f,420.f);E.ShotDirection=Direction;E.HitInfo=H;E.HitInfo.Component.Reset();E.HitInfo.ImpactPoint=Z->Parts[0]->Bounds.Origin;
  ALWCharacter* CreditPlayer=Driver?Driver.Get():ConvoyOwner.Get();float Applied=Z->TakeDamage(E.Damage,E,CreditPlayer?CreditPlayer->Controller.Get():nullptr,this);
  if(Applied<=0)continue;Impact=true;
  if(Z->bDead)Z->RagdollForce(Direction*ImpactSpeed+FVector(0,0,180),E.HitInfo.ImpactPoint);
  else Z->LaunchCharacter(Direction*FMath::Min(ImpactSpeed*.8f,1200.f)+FVector(0,0,180),true,true);
 }
 if(Impact){AddDent(End+Direction*Spec().HalfLength,-Direction,ImpactSpeed*.015f);Speed*=.88f;if(World)World->Sound(TEXT("FleshHit"),End,1,.7f);}
}
