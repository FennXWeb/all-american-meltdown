#pragma once
#include "CoreMinimal.h"
class ALWChunk;class ALWWorld;
namespace LWGen {struct FRoad;struct FSite;}
namespace LWSyracuse73 {
FVector2D Warp(FVector2D P);FVector2D Unwarp(FVector2D P);
bool Contains(FVector2D P);
float LakeDepth(FVector2D P);
void Landmarks(TArray<LWGen::FSite>& Sites);
void Streets(TArray<LWGen::FRoad>& Roads);
void Buildings(TArray<LWGen::FSite>& Sites,const TArray<LWGen::FRoad>& Roads);
void Build(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S);
FVector2D Start();
struct FFair73 {FVector2D P,Size;float Yaw;const TCHAR* Name;};
TArrayView<const FFair73> FairBuildings();
const TCHAR* StreetAt(FVector2D P);
}
