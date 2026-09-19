#pragma once
#include "CoreMinimal.h"
#include "LWVehicleState.generated.h"
USTRUCT()
struct FLWVehicleDent {
    GENERATED_BODY()
 UPROPERTY() FVector Point=FVector::ZeroVector;
 UPROPERTY() FVector Direction=FVector::ForwardVector;
 UPROPERTY() float Radius=70;
 UPROPERTY() float Depth=5;
};
USTRUCT()
struct FLWVehicleRecord {
 GENERATED_BODY()
 UPROPERTY() bool Owned45=false;
 UPROPERTY() bool Stored45=false;
 UPROPERTY() int32 GarageBay45=-1;
 UPROPERTY() int32 Paint45=-1;
 UPROPERTY() float Replacement45=0;
 UPROPERTY() TSet<FName> Mods45;
 UPROPERTY() FGuid VIN;
 UPROPERTY() FName Model=TEXT("sedan");
 UPROPERTY() FVector Position=FVector::ZeroVector;
 UPROPERTY() FRotator Rotation=FRotator::ZeroRotator;
 UPROPERTY() int32 LockTier=0;
 UPROPERTY() bool Unlocked=false;
 UPROPERTY() bool Hotwired=false;
 UPROPERTY() float Health=200;
 // Negative fuel migrates older saves once, using the model's tank capacity.
 UPROPERTY() float FuelLitres=-1;
 UPROPERTY() bool LastDriven=false;
 UPROPERTY() bool FuelLootInitialized=false;
 UPROPERTY() float FireRemaining=0;
 UPROPERTY() bool Exploded=false;
 UPROPERTY() int32 Attempts=0;
 UPROPERTY() TArray<FLWVehicleDent> Dents;
};
namespace LWSecurity {
 inline const TCHAR* Tier(int T){static const TCHAR* N[]={TEXT("UNLOCKED"),TEXT("NOVICE"),TEXT("TRAINED"),TEXT("EXPERT"),TEXT("MASTER")};return N[FMath::Clamp(T,0,4)];}
 inline float Window(int T){return T<=1?16.f:T==2?10.f:T==3?6.f:3.f;}
 inline float TurnLimit(float Angle,float Secret,int T){float E=FMath::Abs(Angle-Secret);return E<=Window(T)?90.f:FMath::Clamp(75.f-(E-Window(T))*1.2f,4.f,75.f);}
}
