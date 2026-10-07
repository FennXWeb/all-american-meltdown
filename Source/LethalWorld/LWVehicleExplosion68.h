#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWVehicleExplosion68.generated.h"
UCLASS()
class ALWVehicleExplosion68:public AActor {
 GENERATED_BODY()
public:
 ALWVehicleExplosion68();
 void Initialize(class ALWWorld* W,float Scale);
 virtual void Tick(float Dt)override;
 UPROPERTY() TObjectPtr<class UProceduralMeshComponent> Plume;
 UPROPERTY() TObjectPtr<class UPointLightComponent> Flash;
 struct FParticle {FVector P,V;float Radius,Life,Age,Spin;int Kind;};
 TArray<FParticle> Particles;
 float Age=0,Size=1;
 UPROPERTY() TObjectPtr<class ALWWorld> World84;
 float FireDuration84=25,FireDamageClock84=0,FireZ84=-85;
 static ALWVehicleExplosion68* Burst(class ALWWorld* W,FVector Where,float Scale,float FireSeconds=25);
 static void Spawn(class ALWVehicle* V);
};
