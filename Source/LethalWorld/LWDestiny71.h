#pragma once
#include "CoreMinimal.h"
class ALWChunk;class ALWWorld;
namespace LWGen {struct FSite;}
namespace LWDestiny71 {
struct FPanel71 {float Z=0,Thickness=24;FName Material;TArray<FVector2D> Vertices;TArray<int32> Indices;TArray<TArray<FVector2D>> Rings;};
struct FEdge71 {FVector2D A,B,In;bool Front=false;};
struct FZone71 {int Id=0,Level=0;float Area=0;FVector2D Center;TArray<FEdge71> Edges;TArray<FVector2D> Fixtures;};
struct FEscalator71 {int Level;FVector2D P,D;};
struct FRoute71 {FVector2D P,D;};
struct FRoad71 {FVector2D A,B;float Width;bool Parking;};
struct FData71 {TArray<FPanel71> Panels;TArray<FZone71> Zones;TArray<FVector2D> Shell,Entries,Bridge;TArray<FEscalator71> Escalators;TArray<FRoute71> Route;TArray<FRoad71> Roads;FVector2D Atrium,Carousel,CanyonA,CanyonB;};
const FData71& Data();
void Build(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S);
bool Inside(FVector2D P,const TArray<FVector2D>& Poly);
bool FloorAt(FVector2D P,int Level);
}
