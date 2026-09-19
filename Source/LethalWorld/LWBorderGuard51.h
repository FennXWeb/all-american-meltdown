#pragma once
#include "LWZombie.h"
#include "LWBorderGuard51.generated.h"
UCLASS()
class LETHALWORLD_API ALWBorderGuard51 : public ALWZombie {
 GENERATED_BODY()
public:
 virtual void Tick(float Dt) override;
 virtual float TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C) override;
 void Setup51(bool Mech,uint32 Id);
 void Alarm51();bool Mech51=false;float Hostile51=0,Fire51=0,Patrol51=0;FVector Post51;
};
