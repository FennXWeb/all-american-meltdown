#include "LWVoice44.h"
#include "LWWeaponEffect.h"
#include "LWZombie.h"
#include "LWNPCLife.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
void ALWZombie::TickBrain(float Dt,ALWCharacter* P){
 if(int(Kind)>=9){TickBoss(Dt,P);return;}const int K=int(Kind);const FVector Here=GetActorLocation(),Eye=Here+FVector(0,0,K==2?15:40),Delta=P->GetActorLocation()-Here;const float Dist=Delta.Size2D(),Now=GetWorld()->GetTimeSeconds();if(Dist>12500){ClearTarget();return;}
 AttackCooldown-=Dt;Stagger=FMath::Max(0.f,Stagger-Dt);DecisionClock-=Dt;ContactMemory=FMath::Max(0.f,ContactMemory-Dt);PathClock-=Dt;VoiceClock-=Dt;
 FCollisionQueryParams Q(NAME_None,false,this);FHitResult Hit;
 auto Visible=[&](FVector A,FVector B){return !GetWorld()->LineTraceSingleByChannel(Hit,A,B,ECC_Visibility,Q)||Hit.GetActor()==P;};
 const float Persona=.85f+(PersistentId%31)*.01f;
 if(DecisionClock<=0){
  DecisionClock=.25f+(PersistentId%7)*.035f;
  HasVisual=Dist<World->VisibilityRange*(K==2?1.25f:1.f)&&(Alert>0||Dist<600||FVector::DotProduct(GetActorForwardVector(),Delta.GetSafeNormal2D())>.05f)&&Visible(Eye,P->GetActorLocation()+FVector(0,0,30));
  if(HasVisual){Interest=P->GetActorLocation();ContactMemory=10;Alert=14;}else Alert=FMath::Max(0.f,Alert-DecisionClock);
  if(K==3){if(HasVisual&&Dist<240)bAggressive=true;if(!bAggressive&&IsObserved(P)){ConsumeMovementInputVector();GetCharacterMovement()->StopMovementImmediately();return;}if(HasVisual||bAggressive)Alert=14;}
  if(Alert<=0){TacticalGoal=Home+FVector(FMath::Sin(Now*.06+PersistentId)*480,FMath::Cos(Now*.08+PersistentId)*480,0);Tactic=0;}
  else {
   // Chase last confirmed contact; never track the player through walls.
   TacticalGoal=Interest;Tactic=1;const FVector Forward=(Interest-Here).GetSafeNormal2D(),Side(-Forward.Y,Forward.X,0);const float Sign=PersistentId%2?1.f:-1.f;
   if(!HasVisual&&ContactMemory<6)TacticalGoal+=FVector(FMath::Sin(Now*.7+PersistentId),FMath::Cos(Now*.7+PersistentId),0)*(10-ContactMemory)*65;
   if(HasVisual&&(K==1||K==2||K==6||K==7)){Tactic=2;TacticalGoal=Interest+Side*Sign*(K==1?750:K==7?360:420)-Forward*(K==1?1100:K==2?200:80);}
   if(K==7&&HasVisual&&FMath::Fmod(Now+PersistentId,6.f)>2.f){TacticalGoal=Interest;Tactic=1;}
   // Neighbours hear a warning, but receive only this NPC's last seen position.
   if(HasVisual&&VoiceClock<=0){int Count=0;for(TActorIterator<ALWZombie> Z(GetWorld());Z&&Count<6;++Z)if(*Z!=this&&!Cast<ALWResident>(*Z)&&!Z->bDead&&Z->Kind==Kind&&FVector::DistSquared(Here,Z->GetActorLocation())<FMath::Square(900.f)){Z->Interest=Interest;Z->Alert=FMath::Max(Z->Alert,5.f);Count++;}}
  }
 }
 if(K==3&&!bAggressive&&IsObserved(P)){ConsumeMovementInputVector();GetCharacterMovement()->StopMovementImmediately();return;}
 if(VoiceClock<=0&&Alert>0){auto* VoiceAudio=World->Sound(LWVoice44::Enemy(Kind,Appearance35.Body),Here,.7f,Persona);if(LifeAnimation&&(K==0||K==1))LifeAnimation->Speak(VoiceAudio,TEXT("..."));VoiceClock=6+PersistentId%5;}
 ReactTactics54(Dt,P);
 if(Stagger>0)return;
 if(K==1&&RaiderRounds<=0&&ReloadClock<=0&&Alert>0){ReloadClock=CombatRandom54.FRandRange(2.4f,3.f);World->Sound(TEXT("RifleMagOut"),Here);}
 if(ReloadClock>0){ReloadClock-=Dt;if(ReloadClock<=0)RaiderRounds=12;}
 const float Reach=K==4||K==5?255:K==6||K==7?215:155;
 if(AttackWindup>0){AttackWindup-=Dt;GetCharacterMovement()->StopMovementImmediately();
  if(Parts.Num()>4&&K!=7&&K!=8){if(!Missing(3))Parts[3]->SetRelativeRotation(FRotator(-110,0,-15));if(!Missing(4))Parts[4]->SetRelativeRotation(FRotator(-100,0,15));}
  if(AttackWindup<=0){
   if((K==2||K==4||K==6||K==8)&&Dist>Reach){CreatureCharge=K==4?1.05f:K==6?.85f:.4f;World->Sound(K==6?TEXT("DeathclawCharge54"):LWVoice44::Enemy(Kind),Here,.85f);ChargeDirection=(CommittedAttack-Here).GetSafeNormal2D();}
   else if(Dist<Reach+35&&Visible(Eye,P->GetActorLocation())&&FVector::DotProduct(GetActorForwardVector(),Delta.GetSafeNormal2D())>.3f){const float Damage[]={18,14,19,28,38,45,40,25,28};UGameplayStatics::ApplyDamage(P,Damage[K]*(AttackStyle54==2?1.15f:AttackStyle54==1?.85f:1.f)*LegendaryDamage()*(World->Difficulty==0?.75f:World->Difficulty==2?1.2f:1.f),nullptr,this,nullptr);P->LastCombatTime=Now;if(!P->CombatTarget||P->CombatTargetTime<2){P->CombatTarget=this;P->CombatTargetTime=8;}if(K==7)P->Stamina=FMath::Max(0.f,P->Stamina-30);}
   AttackCooldown=CombatRandom54.FRandRange(.9f,1.6f)*InjuryAttackDelay();AttackRecovery54=.25f;
  }return;
 }
 if(CreatureCharge>0){CreatureCharge-=Dt;FVector End=Here+ChargeDirection*850*Dt;FCollisionQueryParams M=Q;M.AddIgnoredActor(P);if(CrossesSafehouse(Here,End)||GetWorld()->SweepSingleByChannel(Hit,Here,End,FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeCapsule(GetCapsuleComponent()->GetScaledCapsuleRadius(),GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*.65f),M))CreatureCharge=0;
  else {GetCharacterMovement()->MaxWalkSpeed=850*InjurySpeed();AddMovementInput(ChargeDirection,1,true);}if(Dist<Reach&&Visible(Eye,P->GetActorLocation())){UGameplayStatics::ApplyDamage(P,(K==2?20:35)*LegendaryDamage(),nullptr,this,nullptr);P->LastCombatTime=Now;if(!P->CombatTarget||P->CombatTargetTime<2){P->CombatTarget=this;P->CombatTargetTime=8;}CreatureCharge=0;}if(CreatureCharge<=0)AttackCooldown=1.8f;return;}
 if(K==1&&CanUseGun()&&HasVisual&&Dist<3800&&Suppression54<.82f&&AttackCooldown<=0&&ReloadClock<=0){
  if(RaiderRounds<=0){ReloadClock=2.8f;World->Sound(TEXT("RifleMagIn"),Here);}
  else if(Visible(Eye,P->GetActorLocation()+FVector(0,0,25))){const FVector Muzzle=ALWWeaponEffect::GunMuzzle(this,Gun);FVector Aim=P->GetActorLocation()+FVector(0,0,10);FVector Dir=ALWWeaponEffect::EnemyAim(Muzzle,Aim,GetVelocity().Size2D(),P->GetVelocity().Size2D()+Suppression54*500,Persona);if(GetWorld()->LineTraceSingleByChannel(Hit,Muzzle,Muzzle+Dir*6500,ECC_Visibility,Q)&&Hit.GetActor()==P){UGameplayStatics::ApplyPointDamage(P,11*LegendaryDamage(),Dir,Hit,nullptr,this,nullptr);P->LastCombatTime=Now;if(!P->CombatTarget||P->CombatTargetTime<2){P->CombatTarget=this;P->CombatTargetTime=8;}}ALWWeaponEffect::Gunfire(this,World,Muzzle,Hit.bBlockingHit?Hit.ImpactPoint:Muzzle+Dir*6500);RaiderRounds--;AttackCooldown=RaiderRounds%3==0?CombatRandom54.FRandRange(.65f,1.6f)+Suppression54:.19f;}
 }else if((K!=1||!CanUseGun())&&Alert>0&&HasVisual&&AttackCooldown<=0&&Dist<((K==2||K==4||K==6||K==8)?650:Reach)&&!(K==3&&!bAggressive)){
  AttackStyle54=CombatRandom54.RandRange(0,2);AttackWindup=(K==5?1.05f:K==7?.6f:.38f)*CombatRandom54.FRandRange(.9f,1.3f)*(AttackStyle54==2?1.2f:1.f);World->Sound(TEXT("Swing"),Here,.6f,K==5?.55f:K==6?.8f:1.f);CommittedAttack=P->GetActorLocation();SetActorRotation(Delta.Rotation());return;
 }
 FVector Target=TacticalGoal;FCollisionQueryParams MoveQ=Q;MoveQ.AddIgnoredActor(P);
 if(FMath::Abs(Here.Z-Target.Z)>70||GetWorld()->SweepSingleByChannel(Hit,Here,Target,FQuat::Identity,ECC_WorldStatic,FCollisionShape::MakeCapsule(FMath::Min(30.f,GetCapsuleComponent()->GetScaledCapsuleRadius()),40),MoveQ)){
  if(auto* Door=Cast<ALWWorldObject>(Hit.GetActor());Door&&Door->Kind==ELWObjectKind::Door&&(K==0||K==1||K==3)&&FVector::Dist(Here,Hit.ImpactPoint)<220)Door->RequestCompanionDoor(this);
  FindRoute(Target);while(Path.Num()&&FVector::Dist2D(Here,Path[0])<25&&FMath::Abs(Here.Z-Path[0].Z)<45)Path.RemoveAt(0);if(Path.Num())Target=Path[0];else Target=Here;
 }else {Path.Empty();HostileRoute54.Reset();}
 FVector Direction=(Target-Here).GetSafeNormal2D();if(AttackRecovery54>0)return;const float Speeds[]={245,260,470,520,340,235,420,300,290};GetCharacterMovement()->MaxWalkSpeed=(Alert>0?Speeds[K]*Persona:80)*InjurySpeed();
 if(FVector::Dist2D(Target,Here)>(Path.Num()?15:65)&&!CrossesSafehouse(Here,Here+Direction*130)){AddMovementInput(Direction,1,true);SetActorRotation(FMath::RInterpTo(GetActorRotation(),K==1&&HasVisual?Delta.Rotation():Direction.Rotation(),Dt,K==8?2:6));}
 const float G=Now*(K==2||K==7?13:8),Move=FMath::Clamp(GetVelocity().Size2D()/220,0.,1.);if(K!=8)for(int I=3;I<7;I++)if(!Missing(I))Parts[I]->SetRelativeRotation(K==7?FRotator(0,(I==3?-30:I==4?30:I==5?180:0)+FMath::Sin(G+(I%2)*PI)*8*Move,0):FRotator(K==1&&I<5?-65:FMath::Sin(G+(I%2)*PI)*25*Move,0,0));
}
