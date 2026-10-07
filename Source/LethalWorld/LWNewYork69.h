#pragma once
#include "CoreMinimal.h"
namespace LWGen { struct FRoad; struct FSite; }
namespace LWNY69 {
// Geographic distances are 1:10; all meshes and road widths remain centimetres at 1:1.
struct FTown { const TCHAR* Name; double Latitude,Longitude; int Blocks; };
LETHALWORLD_API const TArray<FTown>& Towns();
LETHALWORLD_API FVector2D Project(double Latitude,double Longitude);
LETHALWORLD_API void Region(FIntPoint R,TArray<LWGen::FRoad>& Roads,TArray<LWGen::FSite>& Sites);
LETHALWORLD_API const TArray<LWGen::FRoad>& Roads();
LETHALWORLD_API const TArray<LWGen::FSite>& Sites();
LETHALWORLD_API bool Settlement(FIntPoint R,FVector2D* Hub=nullptr,FString* Name=nullptr);
LETHALWORLD_API float WaterDepth(FVector2D P);
LETHALWORLD_API FVector2D RoadHub(FIntPoint R);
}
