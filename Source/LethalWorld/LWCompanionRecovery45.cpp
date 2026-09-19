#include "LWResident.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWWorld.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
void ALWResident::RecoverFollow45(float Dt,ALWCharacter* P){
 if(!P||Riding||DownTime>0){FollowStuck45=0;return;}
 FollowCheck45+=Dt;if(FollowCheck45<1)return;const float Elapsed=FollowCheck45;FollowCheck45=0;
 const FVector Origin=P->GetActorLocation();const float Distance=FVector::Dist(GetActorLocation(),Origin);
 const bool Progress=FVector::Dist(GetActorLocation(),FollowPosition45)>80&&Distance<FollowDistance45-35;
 FHitResult Obstacle;FCollisionQueryParams Sight(NAME_None,false,this);Sight.AddIgnoredActor(P);if(P->Vehicle)Sight.AddIgnoredActor(P->Vehicle);
 const bool Obstructed=Distance>250&&GetWorld()->LineTraceSingleByChannel(Obstacle,GetActorLocation(),Origin,ECC_Visibility,Sight);
 FollowStuck45=((Distance>650||FMath::Abs(GetActorLocation().Z-Origin.Z)>220||Obstructed)&&!Progress)?FollowStuck45+Elapsed:0;
 FollowPosition45=GetActorLocation();FollowDistance45=Distance;
 if(FollowStuck45<(Distance>6000?5:12))return;
 FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(P);if(P->Vehicle)Q.AddIgnoredActor(P->Vehicle);
 const float Half=GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),Radius=GetCapsuleComponent()->GetScaledCapsuleRadius();
 for(int I=0;I<32;++I){const float Angle=P->GetActorRotation().Yaw+180+(I%8)*45;const float Range=240+(I/8)*180;FVector At=Origin+FRotator(0,Angle,0).Vector()*Range;FHitResult H;
  if(!GetWorld()->LineTraceSingleByChannel(H,At+FVector(0,0,100),At-FVector(0,0,220),ECC_Visibility,Q)||H.ImpactNormal.Z<.7f)continue;
  At=H.ImpactPoint+FVector(0,0,Half+4);if(P->Vehicle){FVector Local=P->Vehicle->GetActorTransform().InverseTransformPosition(At);if(FMath::Abs(Local.X)<P->Vehicle->Spec().HalfLength+Radius+30&&FMath::Abs(Local.Y)<P->Vehicle->Spec().HalfWidth+Radius+30)continue;}if(FMath::Abs(At.Z-Origin.Z)>160)continue;
  if(GetWorld()->OverlapBlockingTestByChannel(At,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(Radius+3,Half),Q))continue;
  // A clear line at body height keeps the recovery on the player's side of nearby walls.
  FHitResult Wall;if(GetWorld()->LineTraceSingleByChannel(Wall,Origin,At,ECC_Visibility,Q))continue;
  SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);GetCharacterMovement()->StopMovementImmediately();ResetCompanionNavigation();FollowTimer=0;FollowStuck45=0;FollowPosition45=At;return;
 }
 // Retry at a bounded rate if no safe floor is loaded yet.
 FollowStuck45=FMath::Max(0.f,FollowStuck45-2);
}
