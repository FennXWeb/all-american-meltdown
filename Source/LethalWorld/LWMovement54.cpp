#include "LWCharacter.h"
#include "LWWorld.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

void ALWCharacter::OnStartCrouch(float H,float S){Super::OnStartCrouch(H,S);if(Camera){FVector Eye=Camera->GetRelativeLocation()+FVector(0,0,S);Camera->SetRelativeLocation(Eye);CameraOffset54=Eye.Z-(Prone54?22:34);}}
void ALWCharacter::OnEndCrouch(float H,float S){Super::OnEndCrouch(H,S);if(Camera){FVector Eye=Camera->GetRelativeLocation()-FVector(0,0,S);Camera->SetRelativeLocation(Eye);CameraOffset54=Eye.Z-66;}}
bool ALWCharacter::Stand54(){
 auto* M=GetCharacterMovement();const float H=GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
 FCollisionQueryParams Q(SCENE_QUERY_STAT(LWStand54),false,this);
 const FVector Center=GetActorLocation()+FVector(0,0,88-H+2);
 if(GetWorld()->OverlapBlockingTestByChannel(Center,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(32,87),Q)){Notify(TEXT("NOT ENOUGH HEADROOM"),1);return false;}
 Prone54=false;M->SetCrouchedHalfHeight(52);UnCrouch();return true;
}
void ALWCharacter::ToggleProne54(){
 if(!CanAct()||!GetCharacterMovement()->IsMovingOnGround())return;
 if(Prone54){Stand54();return;}
 Sliding54=bSprint=bAim=false;Prone54=true;
 // Resize from an existing crouch while preserving the feet's world height.
 if(bIsCrouched){float Old=GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();GetCapsuleComponent()->SetCapsuleHalfHeight(32);AddActorWorldOffset(FVector(0,0,32-Old));CameraOffset54+=Old-32;}
 GetCharacterMovement()->SetCrouchedHalfHeight(32);Crouch();
 World->Sound(TEXT("Cloth54"),GetActorLocation(),.5f);
}
bool ALWCharacter::TryTraverse54(){
 if(!CanAct()||Stamina<15||Prone54||GetCharacterMovement()->Velocity.Z < -500)return false;
 const FVector F=GetActorForwardVector().GetSafeNormal2D(),Here=GetActorLocation();
 const float Half=GetCapsuleComponent()->GetScaledCapsuleHalfHeight();const FVector Feet=Here-FVector(0,0,Half);
 FCollisionQueryParams Q(SCENE_QUERY_STAT(LWMantle54),false,this);FHitResult Wall,Top;
 if(!GetWorld()->SweepSingleByChannel(Wall,Feet+FVector(0,0,55),Feet+FVector(0,0,55)+F*120,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeSphere(12),Q)||Wall.ImpactNormal.Z>.3f)return false;
 FVector Probe=Wall.ImpactPoint+F*48;
 if(!GetWorld()->LineTraceSingleByChannel(Top,FVector(Probe.X,Probe.Y,Feet.Z+230),FVector(Probe.X,Probe.Y,Feet.Z+35),ECC_Pawn,Q)||Top.ImpactNormal.Z<.72f)return false;
 const float Height=Top.ImpactPoint.Z-Feet.Z;if(Height<40||Height>220||Stamina<(Height<115?15:24))return false;
 if(bIsCrouched&&!Stand54())return false;
 FVector End=Top.ImpactPoint+FVector(0,0,90);
 // Vault a low narrow obstacle; a deep obstacle becomes a climb onto its top.
 if(Height<115){FHitResult Landing;FVector Beyond=Top.ImpactPoint+F*150;
  if(GetWorld()->LineTraceSingleByChannel(Landing,Beyond+FVector(0,0,30),Beyond-FVector(0,0,Height+80),ECC_Pawn,Q)&&Landing.ImpactNormal.Z>.72f)End=Landing.ImpactPoint+FVector(0,0,90);
 }
 FVector Lift=Here;Lift.Z=FMath::Max(End.Z,Top.ImpactPoint.Z+90)+15;FVector Across(End.X,End.Y,Lift.Z);FHitResult Block;
 const auto Shape=FCollisionShape::MakeCapsule(32,87);
 if(GetWorld()->OverlapBlockingTestByChannel(End,FQuat::Identity,ECC_Pawn,Shape,Q))return false;
 for(const auto& Segment:TArray<TPair<FVector,FVector>>{{Here,Lift},{Lift,Across},{Across,End}})
  if(GetWorld()->SweepSingleByChannel(Block,Segment.Key,Segment.Value,FQuat::Identity,ECC_Pawn,Shape,Q))return false;
 CancelReload();StopAttack();bAim=bSprint=Sliding54=false;Traversing54=true;TraversalTime54=0;
 TraversalStart54=Here;TraversalLift54=Lift;TraversalEnd54=End;TraversalDuration54=Height<115?.58f:.95f;Stamina-=Height<115?15:24;
 GetCharacterMovement()->StopMovementImmediately();GetCharacterMovement()->SetMovementMode(MOVE_Flying);
 World->Sound(TEXT("Climb54"),Wall.ImpactPoint,.65f);return true;
}
void ALWCharacter::EndTraverse54(){Traversing54=false;GetCharacterMovement()->SetMovementMode(MOVE_Falling);}
void ALWCharacter::GroundImpact54(FVector At,float Strength,float Radius){
 const float D=FVector::Distance(Camera->GetComponentLocation(),At);if(D>=Radius||!bStarted||IsUIOpen())return;
 float Amount=Strength*FMath::Square(1-D/Radius);Rumble54=FMath::Min(2.f,FMath::Max(Rumble54,Amount));RumbleTime54=.45f;
}
float ALWCharacter::Spread54()const{
 float Spread=1+FMath::Clamp(float(GetVelocity().Size2D())/600.f,0.f,1.f)*(bAim?.35f:1.1f)+RecoilBloom54;
 if(GetCharacterMovement()->IsFalling())Spread+=1.2f;
 if(Prone54)Spread*=.62f;else if(bIsCrouched)Spread*=.8f;
 return Spread;
}
void ALWCharacter::WeaponMotion54(FVector& Position,FRotator& Rotation,float Dt){
 const FVector Local=GetActorTransform().InverseTransformVectorNoScale(GetVelocity());
 FRotator Target(FMath::Clamp(-LookY*.35f,-5.f,5.f),FMath::Clamp(-LookX*.4f,-6.f,6.f),FMath::Clamp(float(-Local.Y)*.007f,-3.f,3.f));
 Sway54=FMath::RInterpTo(Sway54,Target,Dt,12);Rotation+=Sway54*(bAim?.2f:1.f);
 const float SprintTarget=bSprint&&!bAim&&!bReloading&&GetVelocity().Size2D()>100?1.f:0.f;
 Handling54=FMath::FInterpTo(Handling54,SprintTarget,Dt,12);Position+=FVector(-3,-2,-5)*Handling54;Rotation+=FRotator(-16,14,-12)*Handling54;
 if(Sliding54)Rotation.Roll+=8;
 if(Prone54){Position.Z-=1;Rotation.Roll+=FMath::Sin(GetWorld()->GetTimeSeconds()*7)*FMath::Clamp(float(GetVelocity().Size2D())/95.f,0.f,1.f)*3;}
 Position.Z-=Landing54*2;LookX=LookY=0;
}
void ALWCharacter::TickMovement54(float Dt){
 if(!CanAct()){CrouchHeld54=false;CrouchHold56=0;}
 if(CrouchHeld54&&!CrouchHoldUsed56){CrouchHold56+=Dt;if(CrouchHold56>=.4f){CrouchHoldUsed56=true;if(!Prone54)ToggleProne54();}}
 if(!World||!bStarted||bMenu)return;Dt=FMath::Min(Dt,.1f);auto* M=GetCharacterMovement();
 SlideCooldown54=FMath::Max(0.f,SlideCooldown54-Dt);JumpBuffer54=FMath::Max(0.f,JumpBuffer54-Dt);RecoilBloom54=FMath::Max(0.f,RecoilBloom54-Dt*1.8f);
 const bool Ground=M->IsMovingOnGround();GroundGrace54=Ground?.1f:FMath::Max(0.f,GroundGrace54-Dt);
 if(!Ground)LastFallSpeed54=FMath::Max(LastFallSpeed54,float(-GetVelocity().Z));
 if(Ground&&!WasGrounded54){Landing54=FMath::Clamp((LastFallSpeed54-150)/500,0.f,1.5f);if(Landing54>.15f)World->Sound(TEXT("Land54"),GetActorLocation(),FMath::Clamp(Landing54,.2f,1.f));LastFallSpeed54=0;if(JumpBuffer54>0&&CanAct()&&Stamina>12){JumpBuffer54=0;Jump();Stamina-=10;}}
 WasGrounded54=Ground;Landing54=FMath::FInterpTo(Landing54,0.f,Dt,10);
 if(Traversing54){
  if(IsUIOpen()||Vehicle||Health<=0){EndTraverse54();}
  else{TraversalTime54+=Dt;float T=FMath::Clamp(TraversalTime54/TraversalDuration54,0.f,1.f);FVector Across(TraversalEnd54.X,TraversalEnd54.Y,TraversalLift54.Z),Goal;
   auto Ease=[](float V){return V*V*(3-2*V);};
   if(T<.35f)Goal=FMath::Lerp(TraversalStart54,TraversalLift54,Ease(T/.35f));else if(T<.8f)Goal=FMath::Lerp(TraversalLift54,Across,Ease((T-.35f)/.45f));else Goal=FMath::Lerp(Across,TraversalEnd54,Ease((T-.8f)/.2f));
   FHitResult H;SetActorLocation(Goal,true,&H);if(H.bBlockingHit||T>=1)EndTraverse54();
  }
 }
 if(Sliding54){SlideTime54+=Dt;if(!Ground||IsUIOpen()||Vehicle||Health<=0||SlideTime54>1.15f||GetVelocity().Size2D()<150){Sliding54=false;SlideCooldown54=.65f;if(!CrouchHeld54)UnCrouch();}
  else{FVector Steering=GetActorRightVector()*InputRight54*.12f;SlideDirection54=(SlideDirection54+Steering*Dt).GetSafeNormal2D();M->MaxWalkSpeed=760;M->Velocity=SlideDirection54*FMath::Max(150.f,720-SlideTime54*460)+FVector(0,0,M->Velocity.Z);}
 }
 M->GroundFriction=Sliding54?.4f:8.f;M->BrakingDecelerationWalking=Sliding54?200:2200;M->MaxAcceleration=Sliding54?100:2600;M->MaxWalkSpeedCrouched=Sliding54?760:Prone54?95:180;M->AirControl=.38f;
 if(!Vehicle&&!SittingChair&&!bStoryLocked){
  CameraOffset54=FMath::FInterpTo(CameraOffset54,0.f,Dt,13);const float Target=Prone54?22:bIsCrouched?34:66;
  FVector Eye=Camera->GetRelativeLocation();Eye.Z=FMath::FInterpTo(float(Eye.Z),Target+CameraOffset54-Landing54*2,Dt,16);Camera->SetRelativeLocation(Eye);
  if(RumbleTime54>0){RumbleTime54-=Dt;Rumble54=FMath::FInterpTo(Rumble54,0.f,Dt,7);float T=GetWorld()->GetTimeSeconds();Camera->AddAdditiveOffset(FTransform(FRotator(FMath::Sin(T*57)*Rumble54,0,FMath::Cos(T*43)*Rumble54*.6f),FVector(0,0,FMath::Sin(T*65)*Rumble54*1.5f)),0);}
 }
}
