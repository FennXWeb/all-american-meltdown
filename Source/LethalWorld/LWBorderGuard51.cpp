#include "LWBorderGuard51.h"
#include "LWBorder51.h"
#include "LWCanada68.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWNPCLife.h"
#include "LWWeaponEffect.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
void ALWBorderGuard51::Setup51(bool Mech,uint32 Id){
 ResidentId=FName(*FString::Printf(TEXT("canada_guard_%u"),Id));NpcRole=TEXT("border68");DisplayName=Mech?TEXT("Sentinel"):TEXT("Border officer");
 GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);Mech51=Mech;PersistentId=Id;LegendaryInitialized=true;Stars=0;CustomName=Mech?TEXT("CANADIAN SENTINEL"):TEXT("CANADIAN BORDER GUARD");
 ConfigureKind(ELWEnemyKind::Raider);Health=MaximumHealth=Mech?3600:320;
 if(Mech){LifeAnimation->SetComponentTickEnabled(false);if(Gun)Gun->SetVisibility(false);GetCapsuleComponent()->SetCapsuleSize(125,225);const TCHAR* N[]={TEXT("SentinelTorso51"),TEXT("SentinelHead51"),TEXT("SentinelPelvis51"),TEXT("SentinelArm51"),TEXT("SentinelArm51"),TEXT("SentinelLeg51"),TEXT("SentinelLeg51")};FVector V[]={FVector(0,0,140),FVector(0,0,225),FVector(0,0,45),FVector(0,-135,165),FVector(0,135,165),FVector(0,-46,0),FVector(0,46,0)};for(int J=0;J<7;J++){Parts[J]->SetStaticMesh(World->Mesh(N[J]));Parts[J]->EmptyOverrideMaterials();Parts[J]->SetRelativeLocation(V[J]-FVector(0,0,50));Parts[J]->SetRelativeScale3D(FVector(1));Parts[J]->SetRelativeRotation(FRotator::ZeroRotator);}for(auto* C:GetComponentsByTag(USceneComponent::StaticClass(),TEXT("CharacterHair32")))C->DestroyComponent();}
 else{for(int J:{0,2,3,4,5,6})Parts[J]->SetMaterial(0,World->Material(TEXT("CanadaArmor51")));for(auto* C:GetComponentsByTag(USceneComponent::StaticClass(),TEXT("CharacterHair32")))C->DestroyComponent();for(int J:{0,1}){auto* Kit=NewObject<UStaticMeshComponent>(this);Kit->SetupAttachment(Parts[J]);Kit->SetStaticMesh(World->Mesh(J==0?TEXT("GuardVest51"):TEXT("GuardHelmet51")));Kit->SetCollisionEnabled(ECollisionEnabled::NoCollision);Kit->RegisterComponent();AddInstanceComponent(Kit);}}
 Post51=Home=GetActorLocation();GetCharacterMovement()->MaxWalkSpeed=Mech?95:120;
}
void ALWBorderGuard51::Alarm51(){for(TActorIterator<ALWBorderGuard51> It(GetWorld());It;++It)if(FVector::DistSquared(It->Post51,Post51)<FMath::Square(16000.f))It->Hostile51=35;}
float ALWBorderGuard51::TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C){if(D<=0||bDead)return 0;Alarm51();if(!Mech51)return Super::TakeDamage(D,E,I,C);Health=FMath::Max(0.f,Health-D);if(Health<=0){bDead=true;GetCharacterMovement()->StopMovementImmediately();for(auto& Part:Parts){Part->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);Part->SetCollisionResponseToAllChannels(ECR_Block);Part->SetSimulatePhysics(true);Part->SetMassOverrideInKg(NAME_None,250);}GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);}return D;}
void ALWBorderGuard51::Tick(float Dt){
 ACharacter::Tick(Dt);if(bDead||!World)return;auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||!P->bStarted||P->bMenu||P->Health<=0)return;
 if(!Mech51&&(TickStatus(Dt)||SeveredMask||HeadlessTime>0)){TickInjuries(Dt,P);return;}
 float Distance=FVector::Dist2D(GetActorLocation(),P->GetActorLocation());Hostile51=FMath::Max(0.f,Hostile51-Dt);if(LWGeography84::Restricted(FVector2D(P->GetActorLocation()))&&!LWCanada68::Authorized(P)&&!(P->Vehicle&&(P->Vehicle->IsHelicopter57()||P->Vehicle->IsAircraft84()))&&Distance<15000)Hostile51=35;
 SubtitleTime=FMath::Max(0.f,SubtitleTime-Dt);if(P->Speaker==this&&Hostile51<=0){GetCharacterMovement()->StopMovementImmediately();return;}HasVisual=Hostile51>0;Alert=Hostile51;Interest=Hostile51>0?P->GetActorLocation():Post51;Fire51-=Dt;Patrol51+=Dt;FVector Target=Post51+FVector(0,FMath::Sin(Patrol51*.045f+(PersistentId%19))*340,0);
 if(Hostile51>0&&Distance<16000){Target=GetActorLocation();FVector Aim=P->GetActorLocation()+FVector(0,0,25);FRotator Face=(Aim-GetActorLocation()).Rotation();Face.Pitch=Face.Roll=0;SetActorRotation(FMath::RInterpTo(GetActorRotation(),Face,Dt,Mech51?2.f:5.f));
 if(Fire51<=0){Fire51=Mech51?.32f:.8f;FVector Muzzle=Mech51?Parts[3+(int(Patrol51*3)%2)]->GetComponentLocation()+GetActorForwardVector()*155-FVector(0,0,65):ALWWeaponEffect::GunMuzzle(this,Gun);FHitResult Hit;FCollisionQueryParams Q(NAME_None,false,this);FVector Shot=ALWWeaponEffect::EnemyAim(Muzzle,Aim,GetVelocity().Size2D(),P->GetVelocity().Size2D(),1.8f);FVector End=Muzzle+Shot*16000;bool Block=GetWorld()->LineTraceSingleByChannel(Hit,Muzzle,End,ECC_Visibility,Q);ALWWeaponEffect::Gunfire(this,World,Muzzle,Block?Hit.ImpactPoint:End);if(Block&&(Hit.GetActor()==P||Hit.GetActor()==P->Vehicle))UGameplayStatics::ApplyPointDamage(Hit.GetActor(),Mech51?26:13,(End-Muzzle).GetSafeNormal(),Hit,GetController(),this,nullptr);}}
 else if(FVector::Dist2D(GetActorLocation(),Target)>50){FVector D=(Target-GetActorLocation()).GetSafeNormal2D();AddMovementInput(D,1,true);SetActorRotation(FMath::RInterpTo(GetActorRotation(),D.Rotation(),Dt,2.f));}
 if(Mech51){float Move=FMath::Clamp(GetVelocity().Size2D()/90.f,0.f,1.f),T=Patrol51*2.7f;for(int J=5;J<7;J++)Parts[J]->SetRelativeRotation(FRotator(FMath::Sin(T+(J%2)*PI)*12*Move,0,0));Parts[1]->SetRelativeRotation(FRotator(0,Hostile51>0?0:FMath::Sin(Patrol51*.35f)*30,0));for(int J=3;J<5;J++)Parts[J]->SetRelativeRotation(FRotator(Hostile51>0?FMath::Clamp(float((P->GetActorLocation()-Parts[J]->GetComponentLocation()).Rotation().Pitch),-35.f,35.f):FMath::Sin(T)*Move*3,0,0));}
}
