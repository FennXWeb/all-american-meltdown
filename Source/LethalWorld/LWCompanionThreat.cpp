#include "LWWeaponEffect.h"
#include "LWResident.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWThreatAwareness.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

bool ALWResident::TickCompanionThreat(float Dt,ALWCharacter* P){
 ThreatScanClock-=Dt;
 auto Relevant=[&](ALWZombie* Z){
  if(!IsValid(Z)||Z==this)return false;
  if(auto* R=Cast<ALWResident>(Z);R&&(!R->IsTownHostile()||R->DownTime>0))return false;
  return FVector::DistSquared(Z->GetActorLocation(),GetActorLocation())<FMath::Square(3200.)
   &&(LWThreatAwareness::Engaged(Z,P)||LWThreatAwareness::Engaged(Z,this));
 };
 if(ThreatScanClock<=0){
  ThreatScanClock=.25f+(PersistentId%5)*.025f;
  ALWZombie* Best=nullptr;float BestScore=MAX_flt;
  for(TActorIterator<ALWZombie> It(GetWorld());It;++It){
   if(!Relevant(*It))continue;
   const float Distance=FVector::Dist2D(GetActorLocation(),It->GetActorLocation());
   // Prefer immediate danger and retain an existing target to avoid aim oscillation.
   const float Score=Distance-(*It==CompanionThreat.Get()?250.f:0.f)
    -(FVector::Dist2D(P->GetActorLocation(),It->GetActorLocation())<450?500.f:0.f);
   if(Score<BestScore&&LWThreatAwareness::Visible(this,*It)){Best=*It;BestScore=Score;}
  }
  CompanionThreat=Best;
 }
 auto* Enemy=CompanionThreat.Get();
 if(!Relevant(Enemy)){CompanionThreat.Reset();return false;}
 if(!LWThreatAwareness::Visible(this,Enemy))return false;
 const FVector Here=GetActorLocation(),Eye=Here+FVector(0,0,35);
 const FVector Dir=(Enemy->GetActorLocation()-Eye).GetSafeNormal();
 if(Gun)Gun->SetVisibility(true);
 if(FireTime<=0&&CanUseGun()){
  FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,this);
  // Recheck the actual shot: a player or friendly crossing the muzzle cancels fire.
  if(GetWorld()->LineTraceSingleByChannel(Hit,Eye,Enemy->GetActorLocation(),ECC_Visibility,Q)&&Hit.GetActor()==Enemy){
   UGameplayStatics::ApplyPointDamage(Enemy,12*(1+P->Stat(TEXT("crewdamage"))),Dir,Hit,P->Controller,this,nullptr);
   ALWWeaponEffect::Gunfire(this,World,ALWWeaponEffect::GunMuzzle(this,Gun),Hit.ImpactPoint);World->Noise(Eye,3500);FireTime=.65f;
  }else FireTime=.15f;
 }
 // Keep following a moving player; only hold or retreat while still close to the squad.
 if(P->Vehicle||!LWThreatAwareness::SameLevel(this,P)||FVector::Dist2D(Here,P->GetActorLocation())>850)return false;
 const float Distance=FVector::Dist2D(Here,Enemy->GetActorLocation());
 if(Distance<500){
  const FVector Away=(Here-Enemy->GetActorLocation()).GetSafeNormal2D();
  FVector Retreat=Here+Away*300;
  if(FVector::Dist2D(Retreat,P->GetActorLocation())>900)Retreat=P->GetActorLocation();
  if(!NavigateCompanion(Retreat,Health<60?430.f:330.f,Dt))GetCharacterMovement()->StopMovementImmediately();
 }else GetCharacterMovement()->StopMovementImmediately();
 SetActorRotation(FRotator(0,Dir.Rotation().Yaw,0));return true;
}
