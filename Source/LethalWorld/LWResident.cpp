#include "LWVoice44.h"
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
void ALWResident::ConfigureResident(FName Id,FName Job,FString Name,int32 V){
 GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);ResidentId=Id;PersistentId=GetTypeHash(Id);NpcRole=Job;DisplayName=Name;Voice=FMath::Abs(V)%3;ConfigureKind(ELWEnemyKind::Raider);Health=150;Home=GetActorLocation();
 const bool Female=Id==TEXT("story_mara")||Id==TEXT("story_inez")||Id==TEXT("story_elsie")||Id==TEXT("story_voss")||(!Id.ToString().StartsWith(TEXT("story_"))&&(FCrc::StrCrc32(*Id.ToString())&1));
 const TCHAR* Suffix[]={TEXT("Torso"),TEXT("Head"),TEXT("Pelvis"),TEXT("Arm"),TEXT("Arm"),TEXT("Leg"),TEXT("Leg")};
 for(int I=0;I<Parts.Num();I++)Parts[I]->SetStaticMesh(World->Mesh(FName(*(FString(Female?TEXT("ResidentFemale"):TEXT("Resident"))+Suffix[I]+TEXT("32")))));
 LWAppearance::StyleNPC(this,Parts,Id,Female);
 if(Gun)Gun->SetVisibility(Job==TEXT("recruit"));
}
void ALWResident::Say(const FString& Line){Subtitle=Line;SubtitleTime=5;auto* Audio=World?World->Sound(LWVoice44::Human(Appearance35.Body==1),GetActorLocation()+FVector(0,0,65),.65f):nullptr;if(LifeAnimation)LifeAnimation->Speak(Audio,Line);}
float ALWResident::TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C){
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||P->bSafehouse||P->bMenu||DownTime>0)return 0;

 if(D<=0)return 0;
 if(!SettlementId.IsNone()&&(Cast<ALWCharacter>(C)||(I&&I==P->Controller&&!Cast<ALWResident>(C))))AlertTown();
 BloodHit(E,C,D);Health-=FMath::Max(0.f,D);if(Health<=0){Health=1;DownTime=30*FMath::Max(.3f,1-P->Stat(TEXT("revive")));Say(TEXT("I am hit. Give me a moment!"));GetCharacterMovement()->StopMovementImmediately();}return D;
}
void ALWResident::Tick(float Dt){
 ACharacter::Tick(Dt);auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||!P->bStarted||P->bMenu||!World)return;
 if(P->Health<=0){GetCharacterMovement()->StopMovementImmediately();return;}
 if(TickStatus(Dt))return;
 SubtitleTime=FMath::Max(0.f,SubtitleTime-Dt);ChatterTime-=Dt;FireTime-=Dt;
 if(DownTime>0){DownTime-=Dt;Parts[0]->SetRelativeRotation(FRotator(55,0,0));if(DownTime<=0){Health=150;Parts[0]->SetRelativeRotation(FRotator::ZeroRotator);}return;}
 if(IsTownHostile()){TickBrain(Dt,P);return;}
 if(!IsTownHostile()&&(P->Speaker==this||(Shop&&P->OpenObject==Shop))){GetCharacterMovement()->StopMovementImmediately();SetActorRotation((P->GetActorLocation()-GetActorLocation()).Rotation());return;}
 const auto* Crew=P->RPG.Crew.FindByPredicate([&](const auto& R){return R.Id==ResidentId;});
 if(!IsTownHostile()&&ChatterTime<0&&FVector::Dist(P->GetActorLocation(),GetActorLocation())<850){Say(Crew?TEXT("Still with you. Keep your eyes on the street."):NpcRole==TEXT("warden")?TEXT("Looking for work? We have contracts."):NpcRole==TEXT("merchant")?TEXT("Supplies for sale. Fair prices."):NpcRole==TEXT("medic")?TEXT("The clinic is open. Stay safe out there."):NpcRole==TEXT("civilian")? (Activity%3==0?TEXT("Rain is coming. I can feel it."):Activity%3==1?TEXT("Someone fixed the water pump."):TEXT("I used to live north of here.")):TEXT("Need another pair of hands?"));ChatterTime=22+Voice*7;}
 if(Crew&&!Crew->Following&&!Crew->Station.IsNone()&&DefendSettlement(P))return;
 if(!Crew){if(DefendSettlement(P))return;TickSocial(Dt,P,false,-1);return;}
 if(Riding){TickSocial(Dt,P,Crew->Following,Crew->Bedroom);return;}
 if(Crew->Following)RecoverFollow45(Dt,P);
 if(Crew->Following&&!P->bSafehouse){if(TickCompanionThreat(Dt,P))return;}else CompanionThreat.Reset();
 TickSocial(Dt,P,Crew->Following,Crew->Bedroom);
}

void ALWResident::EndPlay(const EEndPlayReason::Type R){if(Riding)LeaveVehicle();if(IsValid(Shop))Shop->Destroy();if(IsValid(World))World->ZombieCount++;Super::EndPlay(R);}
