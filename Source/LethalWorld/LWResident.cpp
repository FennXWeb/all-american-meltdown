#include "LWGeography84.h"
#include "LWSettlement82.h"
#include "LWVoice44.h"
#include "LWDialogue59.h"
#include "LWResident.h"
#include "LWNPCLife.h"
#include "LWAppearance.h"
#include "Misc/Crc.h"
#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
void ALWResident::ConfigureResident(FName Id,FName Job,FString Name,int32 V,int32 BodyOverride){
 GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);ResidentId=Id;PersistentId=GetTypeHash(Id);NpcRole=Job;DisplayName=Name;Voice=FMath::Abs(V)%3;ConfigureKind(ELWEnemyKind::Raider);Health=150;Home=GetActorLocation();
 const bool Female=BodyOverride>=0?BodyOverride==1:Id==TEXT("story_mara")||Id==TEXT("story_inez")||Id==TEXT("story_elsie")||Id==TEXT("story_voss")||Id==TEXT("story_tessa")||(!Id.ToString().StartsWith(TEXT("story_"))&&(FCrc::StrCrc32(*Id.ToString())&1));
 const TCHAR* Suffix[]={TEXT("Torso"),TEXT("Head"),TEXT("Pelvis"),TEXT("Arm"),TEXT("Arm"),TEXT("Leg"),TEXT("Leg")};
 for(int I=0;I<Parts.Num();I++)Parts[I]->SetStaticMesh(World->Mesh(FName(*(FString(Female?TEXT("ResidentFemale"):TEXT("Resident"))+Suffix[I]+TEXT("32")))));
 LWAppearance::StyleNPC(this,Parts,Id,Female);
 if(ULWDialogue59::Available())ULWDialogue59::Channel(this)->Assign(ResidentId,Female);
 if(Gun)Gun->SetVisibility(Job==TEXT("recruit"));
}
void ALWResident::Say(const FString& Line){Subtitle=Line;SubtitleTime=FMath::Max(5.f,Line.Len()*.055f);if(ULWDialogue59::Available()){ULWDialogue59::Channel(this)->Say(ResidentId,Appearance35.Body==1,Line);return;}auto* Audio=World?World->Sound(LWVoice44::Human(Appearance35.Body==1),GetActorLocation()+FVector(0,0,65),.65f):nullptr;if(LifeAnimation)LifeAnimation->Speak(Audio,Line);}
float ALWResident::TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C){
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||P->bSafehouse||P->bMenu||DownTime>0)return 0;

 if(D<=0)return 0;
 if(!P->RPG.Claims82.Contains(SettlementId)&&!SettlementId.IsNone()&&(Cast<ALWCharacter>(C)||(I&&I==P->Controller&&!Cast<ALWResident>(C))))AlertTown();
 BloodHit(E,C,D);Health-=FMath::Max(0.f,D);if(Health<=0){Health=1;DownTime=30*FMath::Max(.3f,1-P->Stat(TEXT("revive")));Say(TEXT("I am hit. Give me a moment!"));GetCharacterMovement()->StopMovementImmediately();}return D;
}
void ALWResident::Tick(float Dt){
 ACharacter::Tick(Dt);auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||!P->bStarted||P->bMenu||!World)return;
 if(P->Health<=0){GetCharacterMovement()->StopMovementImmediately();return;}
 if(TickStatus(Dt))return;
 SubtitleTime=FMath::Max(0.f,SubtitleTime-Dt);ChatterTime-=Dt;FireTime-=Dt;
 if(DownTime>0){DownTime-=Dt;Parts[0]->SetRelativeRotation(FRotator(55,0,0));if(DownTime<=0){Health=150;Parts[0]->SetRelativeRotation(FRotator::ZeroRotator);}return;}
 if(IsTownHostile()){TickBrain(Dt,P);return;}
 if(!IsTownHostile()&&(P->Speaker==this||(Shop&&P->OpenObject==Shop))){GetCharacterMovement()->StopMovementImmediately();if(!HomeRV66)SetActorRotation((P->GetActorLocation()-GetActorLocation()).Rotation());return;}
 const auto* Crew=P->RPG.Crew.FindByPredicate([&](const auto& R){return R.Id==ResidentId;});
 if(!Crew&&ResidentId.ToString().StartsWith(TEXT("ca_"))){const bool Near=FVector::DistSquared(P->GetActorLocation(),GetActorLocation())<FMath::Square(16000.);GetCharacterMovement()->SetComponentTickEnabled(Near);if(!Near){GetCharacterMovement()->StopMovementImmediately();SetActorTickInterval(1);return;}SetActorTickInterval(.1f);}
 // Context comes from actual conditions, never a random weather claim or a fixed bark.
 if(ChatterTime<=0&&NpcRole!=TEXT("story")&&NpcRole!=TEXT("encounter")){
  FName Group=TEXT("settler_idle");bool Combat=false;
  if(Crew&&Crew->Following){
   Combat=CompanionThreat.IsValid()&&!CompanionThreat->bDead;
   Group=Combat?TEXT("companion_combat"):Health<55?TEXT("companion_hurt"):Riding?TEXT("companion_vehicle"):TEXT("companion_travel");
   if(!Combat&&!Riding&&Health>=55&&!P->bSafehouse&&FMath::FRand()<.35f){if(World->RainAmount>.2f)Group=TEXT("companion_rain");else if(World->TimeOfDay<6||World->TimeOfDay>20)Group=TEXT("companion_night");}
  }else if(LWGeography84::Canada(FVector2D(GetActorLocation())))Group=TEXT("settler_canada");
  else if(FMath::FRand()<.5f){if(NpcRole==TEXT("merchant"))Group=TEXT("settler_merchant");else if(NpcRole==TEXT("warden"))Group=TEXT("settler_warden");else if(NpcRole==TEXT("medic"))Group=TEXT("settler_medic");}
  const bool Spoke=ULWDialogue59::Channel(this)->Chatter75(ResidentId,Appearance35.Body==1,Group,Combat);
  ChatterTime=Spoke?FMath::FRandRange(Combat?16.f:45.f,Combat?25.f:80.f):FMath::FRandRange(3.f,7.f);
 }
 if(P->Settlement82&&P->Settlement82->ResidentTick(this,Dt))return;
 if(Crew&&!Crew->Following&&!Crew->Station.IsNone()&&DefendSettlement(P))return;
 if(!Crew){if(DefendSettlement(P))return;TickSocial(Dt,P,false,-1);return;}
 if(Riding&&(!Crew||Crew->Following||Crew->HomeVehicle66.IsNone())){TickSocial(Dt,P,Crew->Following,Crew->Bedroom);return;}
 if(TickRVHome66(P))return;
 if(Crew->Following)RecoverFollow45(Dt,P);
 if(Gun&&Crew&&!Riding)Gun->SetVisibility(!LWGeography84::Canada(FVector2D(GetActorLocation())));
 if(Crew->Following&&!P->bSafehouse&&!LWGeography84::Canada(FVector2D(GetActorLocation()))){if(TickCompanionThreat(Dt,P))return;}else CompanionThreat.Reset();
 TickSocial(Dt,P,Crew->Following,Crew->Bedroom);
}

void ALWResident::EndPlay(const EEndPlayReason::Type R){if(Riding)LeaveVehicle();if(IsValid(Shop))Shop->Destroy();if(IsValid(World))World->ZombieCount++;Super::EndPlay(R);}
