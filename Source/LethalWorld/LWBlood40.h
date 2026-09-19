#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWBlood40.generated.h"
UCLASS()
class ALWBlood40 : public AActor {
 GENERATED_BODY()
public:
 ALWBlood40();
 UPROPERTY() TObjectPtr<class UInstancedStaticMeshComponent> Mesh;
 TArray<FVector> Positions,Velocities;float Age=0;
 void Init(class ALWWorld* W,FVector Position,FVector Direction,float Damage);
 virtual void Tick(float Dt)override;
};
