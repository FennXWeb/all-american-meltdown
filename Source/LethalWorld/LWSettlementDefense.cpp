#include "LWResident.h"
#include "LWWeaponEffect.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWThreatAwareness.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "Engine/World.h"
bool ALWResident::IsTownHostile()const{auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));return P&&P->SettlementHostile(SettlementId);}
void ALWResident::AlertTown(){
 if(!World||SettlementId.IsNone())return;
 if(auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0)))P->ChangeReputation(SettlementId,-15,true);
 for(TActorIterator<ALWResident> It(GetWorld());It;++It)if(It->SettlementId==SettlementId){It->ChatterTime=30;It->Say(TEXT("Hostile in town! Take cover!"));}
 if(auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0))){P->ClosePanels();P->Notify(TEXT("SETTLEMENT ALERTED // REPUTATION LOST"));P->RequestSave40();}
}
bool ALWResident::DefendSettlement(ALWCharacter* P){
 if(SettlementId.IsNone()||!World||!P||P->bSafehouse)return false;
 AActor* Target=nullptr;float Best=4200;
 auto Consider=[&](AActor* A){float D=FVector::Dist(GetActorLocation(),A->GetActorLocation());if(D>=Best)return;FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,this);const FVector Eye=GetActorLocation()+FVector(0,0,45);if(!GetWorld()->LineTraceSingleByChannel(Hit,Eye,A->GetActorLocation(),ECC_Visibility,Q)||Hit.GetActor()==A){Target=A;Best=D;}};
 if(IsTownHostile()&&P->Health>0)Consider(P);
 for(TActorIterator<ALWZombie> It(GetWorld());It;++It)if(!Cast<ALWResident>(*It)
  &&(LWThreatAwareness::Engaged(*It,this)||LWThreatAwareness::Engaged(*It,P)))Consider(*It);
 if(Gun)Gun->SetVisibility(Target!=nullptr||NpcRole==TEXT("recruit"));
 if(!Target)return false;
 GetCharacterMovement()->StopMovementImmediately();
 const FVector Eye=GetActorLocation()+FVector(0,0,45),Dir=(Target->GetActorLocation()-Eye).GetSafeNormal();SetActorRotation(FRotator(0,Dir.Rotation().Yaw,0));
 if(FireTime<=0&&CanUseGun()){
  const FVector Muzzle=ALWWeaponEffect::GunMuzzle(this,Gun);
  const FVector Shot=Target==P?ALWWeaponEffect::EnemyAim(Muzzle,Target->GetActorLocation(),GetVelocity().Size2D(),P->GetVelocity().Size2D()):(Target->GetActorLocation()-Muzzle).GetSafeNormal();
  FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,this);
  GetWorld()->LineTraceSingleByChannel(Hit,Muzzle,Muzzle+Shot*4300,ECC_Visibility,Q);
  // Do not shoot through a settler crossing the firing line.
  if(Hit.GetActor()!=Target&&Cast<ALWResident>(Hit.GetActor())){FireTime=.15f;return true;}
  if(Hit.GetActor()==Target)UGameplayStatics::ApplyPointDamage(Target,Target==P?7.f:18.f,Shot,Hit,GetController(),this,nullptr);
  ALWWeaponEffect::Gunfire(this,World,Muzzle,Hit.bBlockingHit?Hit.ImpactPoint:Muzzle+Shot*4300);
  World->Noise(Muzzle,4500);FireTime=.9f+Voice*.17f;
 }
 return true;
}
