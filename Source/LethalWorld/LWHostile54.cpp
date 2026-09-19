#include "LWZombie.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "LWVoice44.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"

void ALWZombie::SuppressAlong54(ALWCharacter* Shooter,FVector Start,FVector End){
 if(!Shooter||!Shooter->GetWorld())return;FCollisionQueryParams Q(SCENE_QUERY_STAT(LWSuppression54),false,Shooter);FHitResult H;
 if(Shooter->GetWorld()->LineTraceSingleByChannel(H,Start,End,ECC_Visibility,Q))End=H.ImpactPoint;
 for(TActorIterator<ALWZombie> It(Shooter->GetWorld());It;++It){auto* N=*It;if(N->bDead||Cast<ALWResident>(N)||FVector::DistSquared(Start,N->GetActorLocation())>FMath::Square(6500.f))continue;
  if(FMath::PointDistToSegment(N->GetActorLocation()+FVector(0,0,30),Start,End)<210){N->Suppression54=FMath::Min(1.f,N->Suppression54+.28f);N->Interest=Start;N->Alert=FMath::Max(N->Alert,8.f);}
 }
}
void ALWZombie::ReactTactics54(float Dt,ALWCharacter* P){
 if(!BrainReady54){BrainReady54=true;CombatRandom54.Initialize(PersistentId^GetTypeHash(Home)^54001);Strafe54=CombatRandom54.RandRange(0,1)?1:-1;}
 Suppression54=FMath::Max(0.f,Suppression54-Dt*.22f);TacticHold54-=Dt;CoverHold54-=Dt;AttackRecovery54=FMath::Max(0.f,AttackRecovery54-Dt);
 if(Alert<=0||AttackWindup>0||CreatureCharge>0)return;
 const FVector Here=GetActorLocation(),F=(Interest-Here).GetSafeNormal2D(),Side(-F.Y,F.X,0);const float D=FVector::Dist2D(Here,Interest);
 if(TacticHold54<=0){Strafe54=CombatRandom54.RandRange(0,1)?1:-1;TacticHold54=CombatRandom54.FRandRange(2.3f,5.5f);}
 if(!HasVisual){
  // Search only around the last heard/seen contact, with wider sweeps as memory ages.
  if(D<280&&ContactMemory<6){float A=TacticHold54+PersistentId%19;TacticalGoal=Interest+FVector(FMath::Cos(A),FMath::Sin(A),0)*(160+(6-ContactMemory)*55);}
  return;
 }
 if(Kind==ELWEnemyKind::Raider){
  const bool NeedCover=Suppression54>.3f||ReloadClock>0||Health<MaximumHealth*.45f;
  if(NeedCover&&CoverHold54<=0){
   FCollisionQueryParams Q(SCENE_QUERY_STAT(LWCover54),false,this);Q.AddIgnoredActor(P);FHitResult Hit;float Best=MAX_flt;bool Found=false;
   for(int I=0;I<6;++I){float A=CombatRandom54.FRand()*2*PI;FVector C=Here+FVector(FMath::Cos(A),FMath::Sin(A),0)*CombatRandom54.FRandRange(200,650);
    if(!GetWorld()->LineTraceSingleByChannel(Hit,C+FVector(0,0,120),C-FVector(0,0,200),ECC_WorldStatic,Q)||Hit.ImpactNormal.Z<.72f)continue;
    C.Z=Hit.ImpactPoint.Z+GetCapsuleComponent()->GetScaledCapsuleHalfHeight()+2;
    if(GetWorld()->SweepSingleByChannel(Hit,Here,C,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(30,70),Q))continue;
    if(!GetWorld()->LineTraceSingleByChannel(Hit,C+FVector(0,0,25),P->GetActorLocation()+FVector(0,0,35),ECC_Visibility,Q))continue;
    float Score=FVector::DistSquared(Here,C);if(Score<Best){Best=Score;Cover54=C;Found=true;}
   }
   CoverHold54=Found?CombatRandom54.FRandRange(2.8f,5.f):1.2f;if(!Found)Cover54=FVector::ZeroVector;
  }
  if(CoverHold54>0&&!Cover54.IsNearlyZero()){
   TacticalGoal=Cover54;Tactic=3;
   // Leave cover briefly to peek only after loading and recovering from incoming fire.
   if(ReloadClock<=0&&Suppression54<.3f&&FMath::Fmod(CoverHold54,2.f)<.7f)TacticalGoal+=Side*Strafe54*135;
  }else{
   const float Range=Health<MaximumHealth*.35f?1500:PersistentId%3==0?600:1100;
   TacticalGoal=Interest-F*Range+Side*Strafe54*(Suppression54>.3f?650:350);Tactic=2;
   if(D<350)TacticalGoal=Here-F*500+Side*Strafe54*200;
  }
 }else if(Kind==ELWEnemyKind::Dog||Kind==ELWEnemyKind::Deathclaw){
  TacticalGoal=D>430?Interest+Side*Strafe54*(Kind==ELWEnemyKind::Dog?320:220)-F*120:Interest;
 }else if(Kind==ELWEnemyKind::Zombie){
  // Distributed approach lanes collapse only at striking distance.
  TacticalGoal=Interest+Side*float(int(PersistentId%5)-2)*FMath::Clamp((D-160)*.22f,0.f,110.f);
 }
}
void ALWZombie::CreatureFootfall54(float Phase,ALWCharacter* P){
 const int Beat=FMath::FloorToInt(Phase/PI);if(Beat==FootSide54)return;FootSide54=Beat;
 if(!World||bDead||GetVelocity().Size2D()<25||!GetCharacterMovement()->IsMovingOnGround()||HeldCar49.IsValid()||BurrowPhase)return;
 const bool Giant=Kind==ELWEnemyKind::Titan||Kind==ELWEnemyKind::Behemoth||Kind==ELWEnemyKind::Colossus;
 const float Scale=GetActorScale3D().Z,Half=GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
 FVector Foot=GetActorLocation()+GetActorRightVector()*(Beat%2?1:-1)*GetCapsuleComponent()->GetScaledCapsuleRadius()*.6f+GetActorForwardVector()*20*Scale;
 FCollisionQueryParams Q(SCENE_QUERY_STAT(LWCreatureStep54),false,this);FHitResult Hit;
 if(!GetWorld()->LineTraceSingleByChannel(Hit,Foot,Foot-FVector(0,0,Half+180*Scale),ECC_WorldStatic,Q))return;
 Foot=Hit.ImpactPoint;
 if(Giant){World->Sound(Kind==ELWEnemyKind::Colossus?TEXT("ColossusStep54"):Kind==ELWEnemyKind::Behemoth?TEXT("BehemothStep54"):TEXT("TitanStep54"),Foot,1,CombatRandom54.FRandRange(.92f,1.06f));
  P->GroundImpact54(Foot,Kind==ELWEnemyKind::Colossus?2.f:Kind==ELWEnemyKind::Behemoth?1.3f:.55f,Kind==ELWEnemyKind::Colossus?6000:Kind==ELWEnemyKind::Behemoth?2800:1100);
 }else if(Kind!=ELWEnemyKind::Karen&&Kind!=ELWEnemyKind::WorldEater)World->Sound(Kind==ELWEnemyKind::Scorpion?TEXT("ScorpionStep54"):Kind==ELWEnemyKind::Deathclaw?TEXT("DeathclawStep54"):TEXT("StepEarth"),Foot,.5f,1+(PersistentId%9)*.02f);
}
