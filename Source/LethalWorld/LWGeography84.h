#pragma once
#include "CoreMinimal.h"
namespace LWGen {struct FRoad;struct FSite;}
class ALWWorld;class ALWChunk;class ULWSaveGame;
namespace LWGeography84 {
struct FCheckpoint {FVector2D Position;float Yaw;const TCHAR* Name;};
float WaterDepth(FVector2D P);
bool Canada(FVector2D P);
bool Restricted(FVector2D P);
FVector2D BorderNearest(FVector2D P);
const TArray<LWGen::FRoad>& Border();
const TArray<FCheckpoint>& Checkpoints();
bool NearCheckpoint(FVector2D P,float Distance=4000);
void Roads(TArray<LWGen::FRoad>& Out);
void FillParcels(TArray<LWGen::FSite>& Sites,TArray<LWGen::FRoad>& Roads);
void BuildBorder(ALWChunk* C,ALWWorld* W);
void Migrate(ULWSaveGame* Save);
}
