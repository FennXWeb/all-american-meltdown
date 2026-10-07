#pragma once
#include "LWVehicleState.h"
namespace LWGarage45 {
struct FMod {FName Id,Slot,Stat;FString Name,Models;int Cost;float Value;};
const TArray<FMod>& Mods();
bool Compatible(const FMod& M,FName Model);
float Stat(const FLWVehicleRecord* R,FName Key);
inline int Bays(FName Model){return Model==TEXT("rv")||Model==TEXT("solstice_rv")||Model==TEXT("bus")?3:1;}
int FreeBay(const TMap<FName,FLWVehicleRecord>& Cars,FName Model);
inline FVector BayPosition(int Bay,int Size){int Row=Bay/5;float X=-1600+(Bay%5)*800+(Size-1)*400;return FVector(-1700+X,1700+(Row?1500:-1500),-7060+95);}
}
