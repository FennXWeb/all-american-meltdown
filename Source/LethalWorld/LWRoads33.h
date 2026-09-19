#pragma once
#include "CoreMinimal.h"
#include "LWGeneration.h"
class ALWVehicle;
namespace LWRoads33 {
struct FApproach {FVector2D Out;float Width=780;};
struct FJunction {FVector2D Position;TArray<FApproach> Arms;FVector2D Main;float Radius=0;uint32 Id=0;bool Signals=false;};
int Lanes(const LWGen::FRoad& R);
TArray<FJunction> Junctions(const TArray<LWGen::FRoad>& Roads,int Seed);
// 0 red, 1 amber, 2 green. Main and cross traffic never share green.
int Phase(const FJunction& J,FVector2D Approach,double Seconds);
bool InJunction(FVector2D P,const TArray<FJunction>& Junctions,float Extra=0);
float TrafficSpeed(ALWVehicle* Vehicle,float Desired,float Dt);
}
