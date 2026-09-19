#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWZombie.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
void ALWCharacter::Punch(){
 if(bSafehouse||ALWWorld::IsSafePosition(GetActorLocation())||AttackTimer>0||bTrigger)return;
 if(Stamina<8){Notify(TEXT("EXHAUSTED"));return;}
 bTrigger=true;PunchLeft=!PunchLeft;Stamina-=8;AttackDuration=AttackTimer=.48f;
 const FVector A=Camera->GetComponentLocation(),D=Camera->GetForwardVector();
 World->Sound(TEXT("PunchSwing"),A,.6f);World->Noise(A,350);
 FHitResult H;FCollisionQueryParams Q(NAME_None,true,this);
 if(GetWorld()->SweepSingleByChannel(H,A,A+D*145,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeSphere(16),Q)){
  if(!ALWWorld::IsSafePosition(H.ImpactPoint))UGameplayStatics::ApplyPointDamage(H.GetActor(),12*CombatMultiplier(),D,H,Controller,this,nullptr);
  World->Sound(TEXT("PunchHit"),H.ImpactPoint,.8f);HitMarker=.15f;
  if(auto* Z=Cast<ALWZombie>(H.GetActor()))Z->Stagger=.12f;
 }
}
void ALWCharacter::TickUnarmed(float Dt){
 AttackTimer=FMath::Max(0.f,AttackTimer-Dt);const bool Visible=CanAct();
 WeaponRoot->SetVisibility(Visible,false);WeaponMesh->SetVisibility(false);
 WeaponRoot->SetRelativeLocationAndRotation(FVector(35,0,-20),FRotator::ZeroRotator);
 const float T=AttackDuration>0?1-AttackTimer/AttackDuration:1;
 const float Reach=AttackTimer>0?FMath::Sin(T*PI):0;
 for(int I=0;I<2;I++){
  auto* Hand=I?LoadingHand.Get():Arms.Get();if(!Hand)continue;
  Hand->AttachToComponent(WeaponRoot,FAttachmentTransformRules::KeepRelativeTransform);
  Hand->SetStaticMesh(World->Mesh(I?TEXT("LeftHand35"):TEXT("RightHand35")));
  const float Swing=((I==1)==PunchLeft)?Reach:0;
  Hand->SetRelativeLocationAndRotation(FVector(Swing*27,I?-13:13,3+Swing*9),FRotator(0,I?5:-5,I?40:-40));
  Hand->SetRelativeScale3D(FVector(.9f));Hand->SetVisibility(Visible);
 }
}
