#pragma once
#include "LWGeneration.h"
#include "Async/Future.h"
struct FLWChunkPlan68 {
 LWGen::FNeighborhood38 Neighborhood;
 TMap<FName,int32> InstanceCounts;
 TSharedPtr<struct FStreamableHandle> Warm;
 TArray<TFunction<void()>> Population;
 TArray<TFunction<bool()>> Furnishing;
 TArray<TFunction<void()>> Geometry;
 int TowerFloor68=-1,TowerSerial68=0;
 bool WildernessQueued68=false;
 bool RunningPopulation=false;
 TArray<LWGen::FRoad> Roads;
 TArray<LWGen::FSite> Sites;
 TArray<FVector> Vertices,Normals;
 TArray<FVector2D> UV;
 TArray<int32> Triangles;
};
struct FLWChunkJob68 {TFuture<TSharedPtr<FLWChunkPlan68>> Future;};
namespace LWStreaming68 {
 TSharedPtr<FLWChunkPlan68> Plan(FIntPoint Coordinate,int Seed,int Town,int Parcel,float Ruggedness);
}
