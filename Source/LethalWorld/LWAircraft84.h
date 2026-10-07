#pragma once
#include "CoreMinimal.h"
class ALWChunk;class ALWWorld;class ALWVehicle;class ALWCharacter;
namespace LWGen{struct FSite;}
namespace LWAviation84 {
enum class EPhase:uint8 {Parked,Taxi,Takeoff,Climb,Cruise,Approach,Final,Rollout,Loiter};
struct FRunway {FName Id;FString Name;FVector A,B,Apron;float Width=4600;FVector Direction()const{return (B-A).GetSafeNormal2D();}float Length()const{return FVector::Dist2D(A,B);} };
struct FSpec {float Floor,HalfCabin,Width,Takeoff,Cruise,Capacity;int Seats;};
inline FSpec Spec(FName Model){return Model==TEXT("private_jet")?FSpec{150,780,125,3300,12500,600,8}:FSpec{300,1400,172,4100,11500,2400,Model==TEXT("airbus")?100:16};}
inline float Burn(float Capacity,bool Autopilot,float Seconds){return FMath::Max(0.f,Seconds)*Capacity/(Autopilot?18000.f:3600.f);}
FVector Seat(FName Model,int Index);const TArray<FRunway>& Runways();const FRunway* Runway(FName Id);const FRunway* Closest(FVector2D P,float MinimumLength=90000,bool AllowCanada=true);
float Surface(FVector2D P,float Terrain);void BuildAirport(ALWChunk* C,ALWWorld* W,const LWGen::FSite& Site);void Ground(ALWChunk* C,ALWWorld* W);
bool UseService(ALWCharacter* P,FName Action);void OpenService(ALWCharacter* P,FVector Where);bool Recover(ALWCharacter* P,FName VehicleId,const FRunway& Airport);
}
