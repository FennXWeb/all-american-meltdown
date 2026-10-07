#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWWeaponEffect.generated.h"
class ALWCharacter;
UCLASS()
class LETHALWORLD_API ALWWeaponEffect:public AActor {
 GENERATED_BODY()
public:
 ALWWeaponEffect();
 virtual void Tick(float Dt) override;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> Visual;
 UPROPERTY() TObjectPtr<class UPointLightComponent> Light;
 UPROPERTY() TObjectPtr<class UMaterialInstanceDynamic> Tint;
 FVector Velocity=FVector::ZeroVector;
 float Power=0,Age=0,Duration=1;
 int32 Mode=0; // 0 missile, 1 flame, 2 short-lived impact flash
 FName Modifier,SourceWeapon62;
 TSet<FName> Perks39;
 void Initialize(ALWCharacter* Player,FVector Direction,int32 EffectMode,float Damage,FName Perk=NAME_None);
 void Detonate();
 static FVector GunMuzzle(AActor* Shooter,class UStaticMeshComponent* Gun);
 static FVector EnemyAim(FVector Muzzle,FVector Target,float ShooterSpeed,float TargetSpeed,float Persona=1);
 static void Gunfire(AActor* Shooter,class ALWWorld* World,FVector Muzzle,FVector End,bool PlayAudio=true);
 FVector ShotStart=FVector::ZeroVector,ShotEnd=FVector::ZeroVector;
 static void Impact(ALWCharacter* P,const FHitResult& Hit,FVector Direction,float Damage,FName Perk,int32 Weapon,const TSet<FName>* Effects39=nullptr);
};
