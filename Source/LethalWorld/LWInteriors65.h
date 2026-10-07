#pragma once
#include "CoreMinimal.h"
#include "LWGeneration.h"
class ALWChunk;class ALWWorld;
namespace LWInteriors65 {
struct FReport {int Type=0,Surfaces=0,FloorCells=0,Furniture=0,WallDecor=0,CeilingDecor=0,Rejected=0;double Milliseconds=0;};
extern TArray<FReport> Reports;
void RecordStep68(struct FScope& Scope,TFunctionRef<void()> Work);
void Warm(ALWWorld* W);
void Mall(ALWChunk* C,ALWWorld* W,const LWGen::FSite& S);
FName Replacement(FName Name);
FName Surface(FName Material);
void CaptureBox(ALWChunk* C,FName Material,FVector P,FVector Size,FRotator R,bool Solid);
void CaptureMesh(ALWChunk* C,ALWWorld* W,FName Name,FVector P,FVector Scale,FRotator R,bool Solid);
FName SupportedModel(ALWChunk* C,FName Name,FVector P);
/** A construction transaction records support geometry before furnishing it. */
struct FScope {
 FScope(ALWChunk* Chunk,ALWWorld* World,const LWGen::FSite& Site,bool Capture=true);
 ~FScope();
 FScope(const FScope&)=delete;
 struct FPart {FBox Bounds;FName Name;bool Slab=false,Wall=false,Reserved=false;};
 ALWChunk* C;ALWWorld* W;LWGen::FSite S;FScope* Previous=nullptr;
 FTransform Frame;TArray<FPart> Parts;TMap<FIntVector,TArray<int>> Index;TArray<int> Large;
 bool Finishing=false,WasSurfaces=false;int StartResident=0,Serial=0;FReport Report;
 FBox LocalBounds(const FBox& Bounds,const FTransform& World)const;
 void Record(FPart P);void Reindex();TArray<int> Query(const FBox& Box)const;
 bool Free(const FBox& Bounds,int Ignore=-1)const;
 bool Place(FName Model,FVector P,float Yaw,FName Interaction=NAME_None,bool Ground=true,int Ignore=-1);
 void Finish();
 bool Capturing68=true,Prepared68=false,Completed68=false;
 struct FSample68 {int Floor;FIntPoint Point;};
 TArray<FSample68> Samples68;int NextSample68=0,FloorBudget68=0,EndResident68=MAX_int32;
 TSet<FIntVector> Visited68;TMap<int,int> DeckFurniture68,DeckCeiling68;
 void Prepare68();void Decorate68(int I,FIntPoint Sample);bool Step68();
};
}
