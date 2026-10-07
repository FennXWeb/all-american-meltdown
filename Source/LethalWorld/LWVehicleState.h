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
 UPROPERTY() bool Gear84=true;
 UPROPERTY() bool Autopilot84=false;
 UPROPERTY() uint8 FlightPhase84=0;
 UPROPERTY() FVector FlightVelocity84=FVector::ZeroVector;
 UPROPERTY() float Thrust84=0;
 UPROPERTY() FName Destination84;
 UPROPERTY() FName Departure84;
 UPROPERTY() FVector2D FlightGoal84=FVector2D::ZeroVector;
 UPROPERTY() bool HasFlightGoal84=false;
 UPROPERTY() bool FlightEngine84=false;
 UPROPERTY() float LoiterAngle84=0;
 UPROPERTY() float CabinBrightness84=.7f;
 UPROPERTY() FLinearColor CabinColor84=FLinearColor(1,.80,.57);
 UPROPERTY() TSet<int32> Bins84;
 UPROPERTY() TSet<int32> Reclined84;
 UPROPERTY() TSet<int32> OpenSuites84;
 UPROPERTY() FName HomeSettlement82;
 UPROPERTY() FName HomeParking82;
 UPROPERTY() int32 ParkingBay82=-1;
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
 UPROPERTY() int32 TurretRounds57=-1;
 // Negative fuel migrates older saves once, using the model's tank capacity.
 UPROPERTY() float FuelLitres=-1;
 // Stored independently of legacy petrol; absent fields migrate to a charged battery.
 UPROPERTY() float BatteryKWh74=-1;
 UPROPERTY() double SolarHours74=-1;
 UPROPERTY() bool LastDriven=false;
 UPROPERTY() bool FuelLootInitialized=false;
 UPROPERTY() float FireRemaining=0;
 UPROPERTY() bool Exploded=false;
 UPROPERTY() int32 Attempts=0;
 UPROPERTY() TArray<FLWVehicleDent> Dents;
 // Cabin equipment belongs to the VIN and survives streaming, delivery and saves.
 UPROPERTY() bool Slides66=false;
 UPROPERTY() bool Awning66=false;
 UPROPERTY() bool CabinLights66=true;
 UPROPERTY() bool BedroomLights66=true;
 UPROPERTY() bool Faucet66=false;
 UPROPERTY() int32 TV66=0;
 UPROPERTY() TSet<int32> Shades66;
};
namespace LWSecurity {
 inline const TCHAR* Tier(int T){static const TCHAR* N[]={TEXT("UNLOCKED"),TEXT("NOVICE"),TEXT("TRAINED"),TEXT("EXPERT"),TEXT("MASTER")};return N[FMath::Clamp(T,0,4)];}
 inline float Window(int T){return T<=1?16.f:T==2?10.f:T==3?6.f:3.f;}
 inline float TurnLimit(float Angle,float Secret,int T){float E=FMath::Abs(Angle-Secret);return E<=Window(T)?90.f:FMath::Clamp(75.f-(E-Window(T))*1.2f,4.f,75.f);}
}
