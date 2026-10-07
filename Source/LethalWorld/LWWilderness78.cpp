#include "LWGeography84.h"
#include "LWWorld.h"
#include "LWCampaign76.h"
#include "LWStreaming68.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"

void ALWChunk::Wilderness78(ALWWorld* W,const TArray<LWGen::FRoad>& Roads,const TArray<LWGen::FSite>& Sites){
 const FVector Origin=GetActorLocation();if(LWGeography84::Canada(FVector2D(Origin)))return;
 FRandomStream Random(LWGen::Hash(Coordinate.X,Coordinate.Y,W->Seed,78001));
 if(Random.FRand()>.72f)return;
 // One small, deterministic scene per eligible chunk; never an unbounded actor scatter.
 for(int Attempt=0;Attempt<12;Attempt++){
  FVector2D XY=FVector2D(Origin)+FVector2D(Random.FRandRange(1400,LWGen::ChunkSize-1400),Random.FRandRange(1400,LWGen::ChunkSize-1400));
  const float Z=LWGen::Height(XY,Roads,Sites);
  auto Clear=[&](FVector2D P){return LWNY69::WaterDepth(P)<=0&&!LWStory::Reserved(P)&&!LWCampaign76::Reserved(P)&&!Roads.ContainsByPredicate([&](const auto& R){return LWGen::DistanceToSegment(P,R)<R.Width*.5+850;})&&!Sites.ContainsByPredicate([&](const auto& S){return (P-S.Position).Size()<S.Size.Size()*.5+1100;});};
  if(!Clear(XY))continue;
  bool Flat=true;for(int X:{-1,1})for(int Y:{-1,1}){const auto P=XY+FVector2D(X*450,Y*450);Flat&=Clear(P)&&FMath::Abs(LWGen::Height(P,Roads,Sites)-Z)<38;}
  if(!Flat)continue;
  const FRotator Rotation(0,Random.FRandRange(0,360),0);const int Style=Random.RandRange(0,5);
  auto Prop=[&](FName Mesh,FVector Offset,float Yaw=0){if(auto* M=W->Mesh(Mesh)){FVector P=FVector(XY,0)+Rotation.RotateVector(Offset);P.Z=LWGen::Height(FVector2D(P),Roads,Sites)-M->GetBoundingBox().Min.Z;Add(W,Mesh,NAME_None,P-Origin,FVector(1),Rotation+FRotator(0,Yaw,0));}};
  if(Style==0||Style==3){Prop(TEXT("TrailTent78"),FVector(-230,100,0));Prop(TEXT("Chair65"),FVector(100,-180,0),40);Prop(TEXT("Barrel"),FVector(240,140,0));}
  else if(Style==1){Prop(TEXT("Pallet65"),FVector(-220,100,0));Prop(TEXT("Barrel"),FVector(-230,-130,0));Prop(TEXT("Crate65"),FVector(260,180,0),15);}
  else if(Style==2){Prop(TEXT("DiningTable65"),FVector(-170,0,0));Prop(TEXT("Chair65"),FVector(-170,-160,0),90);Prop(TEXT("Chair65"),FVector(-170,160,0),-90);}
  else if(Style==4){Prop(TEXT("TrailTent78"),FVector(-300,120,0));Prop(TEXT("Pallet65"),FVector(220,220,0));Prop(TEXT("Crate65"),FVector(310,220,0),-15);}
  else {Prop(TEXT("Barrel"),FVector(-150,0,0));Prop(TEXT("Crate65"),FVector(-280,120,0),20);}
  const FName Id(*FString::Printf(TEXT("wild78_%d_%d"),Coordinate.X,Coordinate.Y));
  FVector Loot=FVector(XY,0)+Rotation.RotateVector(FVector(210,-220,0));Loot.Z=LWGen::Height(FVector2D(Loot),Roads,Sites);
  LWGen::FSite Context;Context.Id=LWGen::Hash(Coordinate.X,Coordinate.Y,W->Seed,78002);Context.Type=Style==1?13:8;Context.Position=XY;
  auto Spawn=[this,W,Id,Context,Loot,Rotation](){W->EnsureSiteContainer(Id,Context,Loot,0);if(auto* O=W->SpawnObject(ELWObjectKind::Container,Id,Loot,Rotation)){if(auto* M=W->Mesh(TEXT("Crate65"))){O->Body->SetStaticMesh(M);O->AddActorWorldOffset(FVector(0,0,-M->GetBoundingBox().Min.Z));}Residents.Add(O);}};
  if(Plan68)Plan68->Population.Add(MoveTemp(Spawn));else Spawn();
  return;
 }
}
