#include "LWZombie.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"
float ALWZombie::InjurySpeed()const{return Missing(5)&&Missing(6)?.32f:Missing(5)||Missing(6)?.68f:1.f;}
float ALWZombie::InjuryAttackDelay()const{return Missing(3)&&Missing(4)?1.9f:Missing(3)||Missing(4)?1.35f:1.f;}
bool ALWZombie::CanUseGun()const{return !Missing(4)&&!Missing(1);}
void ALWZombie::TickInjuries(float Dt,ALWCharacter* P){
 if(bDead)return;
 if(HeadlessTime>0){HeadlessTime-=Dt;if(HeadlessTime<=0){FDamageEvent E;Die(E,InjuryCauser.Get(),60,InjuryInstigator.Get());return;}}
 if(!P||P->bSafehouse||P->Health<=0){ClearTarget();return;}
 const bool Crawl=(Missing(5)&&Missing(6))||((Missing(5)||Missing(6))&&PersistentId%2==0);
 const bool Humanoid=Kind!=ELWEnemyKind::Dog&&Kind!=ELWEnemyKind::Moose&&Kind!=ELWEnemyKind::Scorpion&&Kind!=ELWEnemyKind::Karen;
 if(Crawl&&Humanoid&&!bCrawling){bCrawling=true;float Old=GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight(),New=GetCapsuleComponent()->GetUnscaledCapsuleRadius()+8;GetCapsuleComponent()->SetCapsuleHalfHeight(New);AddActorWorldOffset(FVector(0,0,New-Old));GetCharacterMovement()->MaxStepHeight=18;}
 const float T=GetWorld()->GetTimeSeconds();if(!Missing(1)){TickBrain(Dt,P);}else{AttackCooldown=FMath::Max(0.f,AttackCooldown-Dt);PathClock-=Dt;
 FVector Goal=P->GetActorLocation();if(Missing(1)){HeadlessDirection=FRotator(0,FMath::Sin(T*2)*Dt*55,0).RotateVector(HeadlessDirection);Goal=GetActorLocation()+HeadlessDirection*500;}
 if(PathClock<=0){FindRoute(Goal);PathClock=.7f;}
 while(Path.Num()&&FVector::Dist2D(GetActorLocation(),Path[0])<60)Path.RemoveAt(0);
 FVector Target=Path.Num()?Path[0]:Goal,Dir=(Target-GetActorLocation()).GetSafeNormal2D();
 GetCharacterMovement()->MaxWalkSpeed=(Missing(1)?360.f:Kind==ELWEnemyKind::Dog?400.f:290.f)*InjurySpeed()*(Crawl?.85f:1.f);
 if(!CrossesSafehouse(GetActorLocation(),GetActorLocation()+Dir*120)){AddMovementInput(Dir,1,true);SetActorRotation(FMath::RInterpTo(GetActorRotation(),Dir.Rotation(),Dt,5));}
 const float Dist=FVector::Dist(GetActorLocation(),P->GetActorLocation());FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);
 const FVector Eye=GetActorLocation()+FVector(0,0,bCrawling?15:45);
 bool See=GetWorld()->LineTraceSingleByChannel(H,Eye,P->GetActorLocation(),ECC_Visibility,Q)&&H.GetActor()==P;
 if(!Missing(1)&&AttackCooldown<=0&&See){
  if(Kind==ELWEnemyKind::Raider&&CanUseGun()&&Dist<3500){
   if(RaiderRounds<=0){RaiderRounds=12;AttackCooldown=3*InjuryAttackDelay();World->Sound(TEXT("RifleMagIn"),Eye);}
   else {FVector Shot=FMath::VRandCone((P->GetActorLocation()-Eye).GetSafeNormal(),Missing(3)?.09f:.04f);if(GetWorld()->LineTraceSingleByChannel(H,Eye,Eye+Shot*4000,ECC_Visibility,Q))UGameplayStatics::ApplyPointDamage(H.GetActor(),8,Shot,H,nullptr,this,nullptr);RaiderRounds--;World->Sound(TEXT("RifleFire"),Eye,.8f,1,true);AttackCooldown=.4f*InjuryAttackDelay();}
  }else if(Dist<175){UGameplayStatics::ApplyDamage(P,Missing(3)&&Missing(4)?5:14,nullptr,this,nullptr);AttackCooldown=1.35f*InjuryAttackDelay();}
 }
 }
 if(Gun){Gun->SetVisibility(CanUseGun());if(bCrawling)Gun->SetRelativeLocation(FVector(45,8,12));}
 for(int I=0;I<Parts.Num();I++)if(!Missing(I)){
  if(bCrawling){const FVector Pose[]={FVector(15,0,0),FVector(60,0,12),FVector(-30,0,0),FVector(35,-20,4),FVector(35,20,4),FVector(-50,-12,0),FVector(-50,12,0)};Parts[I]->SetRelativeLocation(Pose[I]);Parts[I]->SetRelativeRotation(FRotator(I==0||I==2?80:I==1?-10:I>=5?86+FMath::Sin(T*5+I)*3:-78+FMath::Sin(T*5+I)*8,0,0));}
  else if(I>=3)Parts[I]->SetRelativeRotation(FRotator(Gun&&CanUseGun()&&I<5?(I==3?-60.f:-75.f):FMath::Sin(T*(Missing(5)||Missing(6)?4:8)+I)*20,0,0));
 }
 if(bCrawling){
  const float Floor=GetActorLocation().Z-GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
  const float Shift=FMath::Clamp(float(Floor+3-Parts[0]->Bounds.GetBox().Min.Z),-80.f,80.f);
  for(int I=0;I<Parts.Num();I++)if(!Missing(I))Parts[I]->SetRelativeLocation(Parts[I]->GetRelativeLocation()+FVector(0,0,Shift));
  if(Gun)Gun->SetRelativeLocation(Gun->GetRelativeLocation()+FVector(0,0,Shift));
 }
}
void ALWZombie::RagdollForce(FVector Velocity,FVector Contact,float Strength){
 if(!bDead)return;Velocity=Velocity.GetClampedToMaxSize(2300)*FMath::Clamp(Strength,0.f,2.f);
 float Mass=0;for(auto& P:Parts)if(P->IsSimulatingPhysics())Mass+=P->GetMass();
 const float Transfer=FMath::Clamp(80.f/FMath::Max(30.f,Mass),.2f,1.5f);
 for(auto& P:Parts)if(P->IsSimulatingPhysics()){
  const FVector Center=P->GetCenterOfMass();float Weight=FMath::Exp(-FVector::DistSquared(Center,Contact)/90000.f);
  P->WakeAllRigidBodies();P->SetUseCCD(true);P->SetPhysicsMaxAngularVelocityInDegrees(720);
  P->AddImpulse(Velocity*Transfer*(.3f+.7f*Weight),NAME_None,true);
  P->AddImpulseAtLocation(Velocity*Transfer*P->GetMass()*.18f,Center+(Contact-Center).GetClampedToMaxSize(35));
 }
}
