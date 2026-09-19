#pragma once
#include "LWZombie.h"
#include "LWResident.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"

// Shared by friendly targeting and music. Alert alone also represents distant noises.
namespace LWThreatAwareness {
inline bool SameLevel(const ACharacter* A,const ACharacter* B){
 const float AFoot=A->GetActorLocation().Z-A->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
 const float BFoot=B->GetActorLocation().Z-B->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
 return FMath::Abs(AFoot-BFoot)<180;
}
inline bool Visible(const AActor* Observer,const AActor* Target){
 FCollisionQueryParams Q(NAME_None,false,Observer);FHitResult Hit;
 const FVector Eye=Observer->GetActorLocation()+FVector(0,0,35);
 for(float Height:{0.f,35.f})
  if(!Observer->GetWorld()->LineTraceSingleByChannel(Hit,Eye,Target->GetActorLocation()+FVector(0,0,Height),ECC_Visibility,Q)||Hit.GetActor()==Target)return true;
 return false;
}
inline bool Engaged(const ALWZombie* Enemy,const ACharacter* Protected){
 // Apply allegiance here too, including when a previously hostile target becomes peaceful.
 if(const auto* Resident=Cast<ALWResident>(Enemy);Resident&&(!Resident->IsTownHostile()||Resident->DownTime>0))return false;
 return IsValid(Enemy)&&!Enemy->bDead&&Enemy->Health>0&&Enemy->Alert>0
  &&(Enemy->Kind!=ELWEnemyKind::Mannequin||Enemy->bAggressive)
  &&SameLevel(Enemy,Protected)
  &&FVector::Dist2D(Enemy->Interest,Protected->GetActorLocation())<1000
  &&FMath::Abs(Enemy->Interest.Z-Protected->GetActorLocation().Z)<180;
}
}
