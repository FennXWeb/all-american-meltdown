#pragma once
#include "LWGeneration.h"
namespace LWBoss48 {
// Percent of streamed chunks: 2% behemoth, 0.3% colossus, 0.75% world eater.
inline int Kind(uint32 Hash){int R=Hash%10000;return R<75?11:R<105?10:R<305?9:INDEX_NONE;}
inline float Radius(int K){return K==10?585.f:K==11?550.f:195.f;}
inline float HalfHeight(int K){return K==10?1368.f:K==11?650.f:456.f;}
inline bool Clear(FVector2D P,int K,float Yaw,const TArray<LWGen::FRoad>& Roads,const TArray<LWGen::FSite>& Sites){
 const float Margin=Radius(K)+250;
 if(P.Size()<22000||LWStory::Reserved(P,K==11?6500:Margin))return false;
 // Check a chain of circles for the worm's long body, instead of only its head.
 for(int Segment=0;Segment<(K==11?10:1);++Segment){FVector2D Q=P+FVector2D(-Segment*900.f,0).GetRotated(Yaw);float R=K==11?1000:Margin;
  if(LWStory::Reserved(Q,R)||Q.Size()<22000)return false;
  for(const auto& S:Sites){FVector2D Local=(Q-S.Position).GetRotated(-S.Yaw);FVector2D Outside(FMath::Max(0.,FMath::Abs(Local.X)-S.Size.X*.5),FMath::Max(0.,FMath::Abs(Local.Y)-S.Size.Y*.5));if(Outside.Size()<R+(S.Friendly?1600:300))return false;}
  for(const auto& Road:Roads)if(LWGen::DistanceToSegment(Q,Road)<Road.Width*.5+R)return false;
 }return true;
}
inline FVector2D Candidate(FIntPoint Chunk,FRandomStream& R){return FVector2D(Chunk.X*LWGen::ChunkSize+R.FRandRange(1600,LWGen::ChunkSize-1600),Chunk.Y*LWGen::ChunkSize+R.FRandRange(1600,LWGen::ChunkSize-1600));}
}
