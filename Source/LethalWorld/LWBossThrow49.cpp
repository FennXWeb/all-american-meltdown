#include "LWZombie.h"
#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWVoice44.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
bool ALWZombie::TickCarAttack49(float Dt,ALWCharacter* P){
 if(Kind!=ELWEnemyKind::Behemoth&&Kind!=ELWEnemyKind::Colossus)return false;
 ThrowCooldown49=FMath::Max(0.f,ThrowCooldown49-Dt);
 if(auto* Car=HeldCar49.Get()){
  if(Car->Grabber49.Get()!=this){HeldCar49.Reset();ThrowCooldown49=12;return false;}
  GetCharacterMovement()->StopMovementImmediately();GrabTimer49+=Dt;for(int Arm:{3,4})if(!Missing(Arm))Parts[Arm]->SetRelativeRotation(FRotator(-FMath::Lerp(20.f,115.f,FMath::Clamp(GrabTimer49/1.6f,0.f,1.f)),0,0));
  if(Missing(3)||Missing(4)||Stagger>0){Car->Throw49(FVector::ZeroVector);HeldCar49.Reset();ThrowCooldown49=12;return false;}
  if(GrabTimer49>=1.8f){FVector Target=P->GetActorLocation()+P->GetVelocity()*.4f;
   if(P->Vehicle==Car){FVector Away=(Car->GetActorLocation()-GetActorLocation()).GetSafeNormal2D();if(Away.IsNearlyZero())Away=GetActorForwardVector();Target=GetActorLocation()+Away*(Kind==ELWEnemyKind::Colossus?6500:4500);Target.Z=World->HeightAt(FVector2D(Target))+150;FHitResult Ground;FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(Car);Q.AddIgnoredActor(P);if(GetWorld()->LineTraceSingleByChannel(Ground,FVector(Target.X,Target.Y,FMath::Max(Target.Z,Car->GetActorLocation().Z)+2000),Target-FVector(0,0,1000),ECC_WorldStatic,Q))Target.Z=Ground.ImpactPoint.Z+150;}
   FVector Delta=Target-Car->GetActorLocation();const float Time=FMath::Clamp(float(Delta.Size2D()/1800),.8f,3.f);Car->Throw49(Delta/Time+FVector(0,0,490*Time));HeldCar49.Reset();ThrowCooldown49=Kind==ELWEnemyKind::Colossus?16:20;AttackCooldown=3;World->Sound(LWVoice44::Enemy(Kind),GetActorLocation(),1);}
  return true;
 }
 if(ThrowCooldown49>0||AttackWindup>0||Stagger>0||Missing(3)||Missing(4)||FVector::Dist2D(GetActorLocation(),P->GetActorLocation())>8000)return false;
 ThrowCooldown49=2;ALWVehicle* Best=nullptr;float Score=BIG_NUMBER;
 for(TActorIterator<ALWVehicle> I(GetWorld());I;++I){auto* V=*I;if(V->Grabber49.IsValid()||V->Airborne49||V->SpawnPlacementPending||!V->Record()||V->Record()->Exploded||V->Record()->Stored45||ALWWorld::IsSafePosition(V->GetActorLocation()))continue;
  const float D=FVector::Dist2D(V->GetActorLocation(),GetActorLocation());if(D>(Kind==ELWEnemyKind::Colossus?2200:1100)+V->Spec().HalfLength||FMath::Abs((GetActorLocation().Z-GetCapsuleComponent()->GetScaledCapsuleHalfHeight())-(V->GetActorLocation().Z-75))>350)continue;
  FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(P);FVector From=GetActorLocation()-FVector(0,0,GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*.5f);if(GetWorld()->LineTraceSingleByChannel(H,From,V->GetActorLocation(),ECC_Visibility,Q)&&H.GetActor()!=V)continue;
  float S=D*(P->Vehicle==V?.25f:1);if(S<Score){Best=V;Score=S;}
 }
 if(Best&&Best->Grab49(this)){HeldCar49=Best;GrabTimer49=0;World->Sound(LWVoice44::Enemy(Kind),GetActorLocation(),1);return true;}return false;
}
