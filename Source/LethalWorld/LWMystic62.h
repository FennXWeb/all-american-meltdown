#pragma once
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "LWMystic62.generated.h"
UCLASS()
class LETHALWORLD_API ULWMystic62:public USceneComponent {
 GENERATED_BODY()
public:
 ULWMystic62();void Initialize(class ALWWorld* W,int Weapon,bool Preview=false);
 virtual void TickComponent(float Dt,ELevelTick Tick,FActorComponentTickFunction* Fn)override;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Drops;
 int Theme=0,WeaponIndex=0;bool IsPreview=false;float Clock=0;FVector Origin=FVector::ZeroVector;
};
