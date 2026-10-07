#include "LWVehicleExplosion68.h"
#include "LWBorderGuard51.h"
#include "LWBorder51.h"
#include "EngineUtils.h"
#include "EngineUtils.h"
#include "Engine/StaticMesh.h"
#include "LWWeaponEffect.h"
#include "LWCharacter.h"
#include "LWWeaponMods.h"
#include "LWWorld.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/DamageEvents.h"
#include "UObject/StrongObjectPtr.h"

FVector ALWWeaponEffect::GunMuzzle(AActor* Shooter,UStaticMeshComponent* Gun){
 if(Gun&&Gun->GetStaticMesh()){
  const FBox B=Gun->GetStaticMesh()->GetBoundingBox();
  return Gun->GetComponentTransform().TransformPosition(FVector(B.Max.X+4,0,0));
 }
 return Shooter->GetActorLocation()+Shooter->GetActorForwardVector()*80+FVector(0,0,38);
}
FVector ALWWeaponEffect::EnemyAim(FVector Muzzle,FVector Target,float ShooterSpeed,float TargetSpeed,float Persona){
 // Movement and target motion degrade aim; no perfect tracking or guaranteed hits.
 const float Degrees=FMath::Clamp((4.8f+ShooterSpeed*.008f+TargetSpeed*.005f)*Persona,4.f,12.f);
 return FMath::VRandCone((Target-Muzzle).GetSafeNormal(),FMath::DegreesToRadians(Degrees));
}
void ALWWeaponEffect::Gunfire(AActor* Shooter,ALWWorld* World,FVector Muzzle,FVector End,bool PlayAudio){
 if(World&&Cast<ALWCharacter>(Shooter)&&LWGeography84::NearCheckpoint(FVector2D(Muzzle),20000))for(TActorIterator<ALWBorderGuard51> I(World->GetWorld());I;++I)if(!I->bDead&&FMath::PointDistToSegment(I->GetActorLocation(),Muzzle,End)<(I->Mech51?500:220)){I->Alarm51();break;}

 if(!Shooter||!World)return;
 if(PlayAudio)World->Sound(TEXT("RifleFire"),Muzzle,1.f,1.f,true);
 // Reuse a resident engine mesh: no synchronous per-shot asset load.
 static TStrongObjectPtr<UStaticMesh> Sphere(LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));
 for(int M=3;M<=4;++M){
  auto* FX=Shooter->GetWorld()->SpawnActor<ALWWeaponEffect>(Muzzle,(End-Muzzle).Rotation());if(!FX)continue;
  FX->SetOwner(Shooter);FX->Mode=M;FX->ShotStart=Muzzle;FX->ShotEnd=End;
  FX->Duration=M==3?FMath::Clamp(float(FVector::Dist(Muzzle,End)/45000),.025f,.16f):.045f;
  FX->SetLifeSpan(FX->Duration);FX->Visual->SetStaticMesh(Sphere.Get());FX->Visual->SetCanEverAffectNavigation(false);
  if(auto* Base=World->Material(TEXT("WeaponFX24"))){FX->Tint=UMaterialInstanceDynamic::Create(Base,FX);FX->Visual->SetMaterial(0,FX->Tint);FX->Tint->SetVectorParameterValue(TEXT("Tint"),M==3?FLinearColor(.09,.055,.025):FLinearColor(1,.48,.12));}
  FX->SetActorScale3D(M==3?FVector(.01):FVector(.15,.045,.045));
  FX->Light->SetIntensity(M==4?35000:0);FX->Light->SetAttenuationRadius(450);FX->Light->SetLightColor(FLinearColor(1,.65,.28));
 }
}

ALWWeaponEffect::ALWWeaponEffect(){PrimaryActorTick.bCanEverTick=true;Visual=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Effect"));SetRootComponent(Visual);Visual->SetCollisionEnabled(ECollisionEnabled::NoCollision);Visual->SetCastShadow(false);Light=CreateDefaultSubobject<UPointLightComponent>(TEXT("Light"));Light->SetupAttachment(Visual);Light->SetCastShadows(false);}
void ALWWeaponEffect::Initialize(ALWCharacter* P,FVector Dir,int32 M,float Damage,FName Perk){
 SourceWeapon62=P->ActiveGun()?P->ActiveGun()->Definition:NAME_None;Perks39.Empty();if(!Perk.IsNone())Perks39.Add(Perk);for(FName N:{FName(TEXT("Life Steal")),FName(TEXT("Toxic")),FName(TEXT("Explosive"))})if(LWMods::Effect(P->ActiveGun(),N))Perks39.Add(N);
 SetOwner(P);SetInstigator(P);Mode=M;Power=Damage;Modifier=Perk;Duration=M==0?6:M==1?.55f:.35f;Velocity=Dir*(M==0?4500.f:1500.f);SetActorRotation(Dir.Rotation());
 Visual->SetStaticMesh(M==0?P->World->Mesh(TEXT("Missile24")):LoadObject<UStaticMesh>(nullptr,TEXT("/Engine/BasicShapes/Sphere.Sphere")));
 if(auto* Base=P->World->Material(TEXT("WeaponFX24"))){Tint=UMaterialInstanceDynamic::Create(Base,this);Visual->SetMaterial(0,Tint);Tint->SetVectorParameterValue(TEXT("Tint"),M==0?FLinearColor(1,.3f,.03f):FLinearColor(1,.13f,.005f));}
 SetActorScale3D(M==0?FVector(1):FVector(.12f));Light->SetIntensity(M==0?5000:16000);Light->SetAttenuationRadius(M==0?160:400);Light->SetLightColor(FLinearColor(1,.25f,.03f));SetLifeSpan(Duration+.1f);
}
void ALWWeaponEffect::Tick(float Dt){
 Super::Tick(Dt);Age+=Dt;
 if(Mode>=3){
  const float Fade=FMath::Clamp(1-Age/Duration,0.f,1.f);
  if(Mode==3){const FVector D=(ShotEnd-ShotStart).GetSafeNormal();const float Distance=FVector::Dist(ShotStart,ShotEnd),Front=FMath::Min(Distance,Age*45000.f),Length=FMath::Min(Front,220.f);SetActorLocation(ShotStart+D*(Front-Length*.5f));SetActorScale3D(FVector(Length/100.f,.0025f,.0025f));}
  else Light->SetIntensity(35000*Fade);
  if(Tint)Tint->SetVectorParameterValue(TEXT("Tint"),(Mode==3?FLinearColor(.09,.055,.025):FLinearColor(1,.48,.12))*Fade);
  return;
 }
 auto* P=Cast<ALWCharacter>(GetOwner());if(!P||!P->World){Destroy();return;}
 if(Mode==2){SetActorScale3D(FVector(.3f+Age*5));Light->SetIntensity(45000*FMath::Max(0.f,1-Age/Duration));return;}
 FHitResult H;FCollisionQueryParams Q(SCENE_QUERY_STAT(LWProjectile),true,this);Q.AddIgnoredActor(P);
 FVector End=GetActorLocation()+Velocity*Dt;
 if(GetWorld()->SweepSingleByChannel(H,GetActorLocation(),End,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeSphere(Mode==0?5:12+Age*40),Q)){
 SetActorLocation(H.ImpactPoint);if(Mode==0)Detonate();else{Impact(P,H,Velocity.GetSafeNormal(),Power,Modifier,15,&Perks39);Destroy();}return;
 }
 SetActorLocation(End);if(Mode==1){SetActorScale3D(FVector(.5f+Age*2.f,.1f+Age*.8f,.1f+Age));Velocity.Z+=Dt*120;Light->SetIntensity(9000*(1-Age/Duration));}if(Age>=Duration)Destroy();
}
void ALWWeaponEffect::Detonate(){
 auto* P=Cast<ALWCharacter>(GetOwner());if(!P||!P->World){Destroy();return;}
 TMap<TWeakObjectPtr<ALWZombie>,float> Victims;for(TActorIterator<ALWZombie> It(GetWorld());It;++It)if(!It->bDead&&!ALWWorld::IsSafePosition(It->GetActorLocation())&&FVector::DistSquared(It->GetActorLocation(),GetActorLocation())<FMath::Square(750.f))Victims.Add(*It,It->Health);
 TArray<AActor*> Ignore;Ignore.Add(this);for(TActorIterator<ALWZombie> It(GetWorld());It;++It)if(ALWWorld::IsSafePosition(It->GetActorLocation()))Ignore.Add(*It);
 const float Radius=Perks39.Contains(TEXT("Explosive"))?950.f:650.f;
 UGameplayStatics::ApplyRadialDamageWithFalloff(this,Power*(Perks39.Contains(TEXT("Explosive"))?1.65f:1.f),Power*.12f,GetActorLocation(),140,Radius,1,nullptr,Ignore,this,P->GetController(),ECC_Visibility);
 for(auto& V:Victims)if(auto* Z=V.Key.Get()){float Lost=FMath::Clamp(V.Value-Z->Health,0.f,V.Value);if(Lost>0){if(Perks39.Contains(TEXT("Life Steal")))P->Health=FMath::Min(P->MaxHealth(),P->Health+Lost*.35f);if(Perks39.Contains(TEXT("Toxic"))&&!Z->bDead){Z->PoisonTime=8;Z->StatusSource=P;}}}
 P->World->Sound(TEXT("FuelExplosion"),GetActorLocation(),1);P->World->Noise(GetActorLocation(),25000);
 ALWVehicleExplosion68::Burst(P->World,GetActorLocation(),1.25f,18);Destroy();
}
void ALWWeaponEffect::Impact(ALWCharacter* P,const FHitResult& H,FVector Dir,float Damage,FName Perk,int32 Weapon,const TSet<FName>* Effects39){
 auto Has39=[&](FName N){return Effects39?Effects39->Contains(N):Perk==N||LWMods::Effect(P?P->ActiveGun():nullptr,N);};
 AActor* A=H.GetActor();if(!P||!A||ALWWorld::IsSafePosition(H.ImpactPoint))return;TGuardValue<FName> WeaponGuard62(P->DamageWeapon62,LWParts39::WeaponId(Weapon));
 auto* Z=Cast<ALWZombie>(A);const bool Living=Z&&!Z->bDead;const float Before=Living?Z->Health:0;
 UGameplayStatics::ApplyPointDamage(A,Damage,Dir,H,P->GetController(),P,nullptr);
 if(Living){
  if(Has39(TEXT("Life Steal")))P->Health=FMath::Min(P->MaxHealth(),P->Health+FMath::Max(0.f,FMath::Min(Before,Before-Z->Health))*.35f);
  if(!Z->bDead){if(Weapon==14&&!Z->StunCredited62&&!Cast<ALWResident>(Z)){Z->StunCredited62=true;LWArsenal62::Credit(P,TEXT("taser"),false,false,true);}Z->StatusSource=P;if(Weapon==14)Z->ShockTime=FMath::Max(Z->ShockTime,5.f/FMath::Sqrt(Z->BodyMassScale()));if(Has39(TEXT("Toxic")))Z->PoisonTime=FMath::Max(Z->PoisonTime,8.f);if(Weapon==15)Z->BurnTime=FMath::Max(Z->BurnTime,4.f);}
  P->HitMarker=.15f;
 }
 if(Has39(TEXT("Explosive"))){TArray<AActor*> Ignore;UGameplayStatics::ApplyRadialDamageWithFalloff(P,Damage*.65f,Damage*.1f,H.ImpactPoint+H.ImpactNormal*8,35,210,1,nullptr,Ignore,P,P->GetController(),ECC_Visibility);}
 if(Has39(TEXT("Explosive")))ALWVehicleExplosion68::Burst(P->World,H.ImpactPoint+H.ImpactNormal*12,.35f,4);
 if(Weapon==14){auto* Fx=P->GetWorld()->SpawnActor<ALWWeaponEffect>(H.ImpactPoint,FRotator::ZeroRotator);if(Fx){Fx->Initialize(P,Dir,2,0);if(Weapon==14&&Fx->Tint){Fx->Tint->SetVectorParameterValue(TEXT("Tint"),FLinearColor(.1f,.4f,1));Fx->Light->SetLightColor(FLinearColor(.1f,.4f,1));}}}
}
bool ALWZombie::TickStatus(float Dt){
 if(bDead)return true;if(ALWWorld::IsSafePosition(GetActorLocation())){ShockTime=PoisonTime=BurnTime=0;return false;}
 if(PoisonTime>0||BurnTime>0){StatusClock+=Dt;if(StatusClock>=.5f){float Damage=(PoisonTime>0?8.f:0)+(BurnTime>0?6.f:0);StatusClock=0;FDamageEvent E;auto* P=StatusSource.Get();if(P){TGuardValue<FName> WeaponGuard62(P->DamageWeapon62,ChallengeWeapon62);TakeDamage(Damage,E,P->GetController(),P);}else TakeDamage(Damage,E,nullptr,nullptr);}PoisonTime=FMath::Max(0.f,PoisonTime-Dt);BurnTime=FMath::Max(0.f,BurnTime-Dt);}
 if(bDead)return true;if(ShockTime>0){ShockTime=FMath::Max(0.f,ShockTime-Dt);GetCharacterMovement()->StopMovementImmediately();AttackCooldown=FMath::Max(AttackCooldown,.3f);return true;}return false;
}
