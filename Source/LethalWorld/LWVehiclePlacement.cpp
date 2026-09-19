#include "LWVehicle.h"
#include "LWWorld.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "ProceduralMeshComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"

namespace {
// Separating-axis test uses full model footprints, including long RV overhangs.
bool FootprintsOverlap(FVector A,FRotator AR,const LWTraffic::FSpec& AS,FVector B,FRotator BR,const LWTraffic::FSpec& BS){
 if(FMath::Abs(A.Z-B.Z)>FMath::Max(AS.Height,BS.Height)+100)return false;
 const FVector2D AX(FRotator(0,AR.Yaw,0).Vector()),AY(-AX.Y,AX.X),BX(FRotator(0,BR.Yaw,0).Vector()),BY(-BX.Y,BX.X),Delta(B-A);
 for(const FVector2D Axis:{AX,AY,BX,BY}){
  const double RA=(AS.HalfLength+40)*FMath::Abs(FVector2D::DotProduct(AX,Axis))+(AS.HalfWidth+40)*FMath::Abs(FVector2D::DotProduct(AY,Axis));
  const double RB=(BS.HalfLength+40)*FMath::Abs(FVector2D::DotProduct(BX,Axis))+(BS.HalfWidth+40)*FMath::Abs(FVector2D::DotProduct(BY,Axis));
  if(FMath::Abs(FVector2D::DotProduct(Delta,Axis))>=RA+RB)return false;
 }return true;
}
}
bool ALWVehicle::ResolveSpawnPlacement(){
 if(!World||!Record())return false;
 const FVector Origin=GetActorLocation();const FRotator Rotation=GetActorRotation();
 auto Clear=[&](FVector At){
  for(const auto& Pair:World->Vehicles)if(Pair.Key!=RecordId&&FootprintsOverlap(At,Rotation,Spec(),Pair.Value.Position,Pair.Value.Rotation,LWTraffic::Get(Pair.Value.Model)))return false;
  FCollisionQueryParams Q(NAME_None,false,this);TArray<FOverlapResult> Hits;
  // Test the entire body height; chassis-only queries miss coach roofs and walls.
  const FVector Centre=At+Rotation.RotateVector(FVector(0,0,Spec().Height*.5f-75));
  GetWorld()->OverlapMultiByChannel(Hits,Centre,Rotation.Quaternion(),ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(Spec().HalfLength+20,Spec().HalfWidth+20,Spec().Height*.5f-2)),Q);
  for(const auto& H:Hits)if(H.bBlockingHit){const auto* C=Cast<ALWChunk>(H.GetActor());if(C&&(H.GetComponent()==C->Terrain||(H.GetComponent()&&H.GetComponent()->ComponentHasTag(TEXT("RoadSurface")))))continue;return false;}
  return true;
 };
 if(Clear(Origin))return true;
 // Stored vehicles wait for their own bay to clear; never move an insured car onto the surface.
 if(Record()->Stored45)return false;
 for(int Ring=1;Ring<=12;Ring++)for(int Direction=0;Direction<16;Direction++){
  const float Angle=Direction*2*PI/16;FVector At=Origin+Rotation.RotateVector(FVector(FMath::Cos(Angle),FMath::Sin(Angle),0))*float(Ring*350);
  // Never relocate into an unloaded chunk whose obstacle geometry is unknown.
  if(!World->Chunks.Contains(LWGen::ChunkAt(FVector2D(At))))continue;
  float Low=MAX_flt,High=-MAX_flt;for(float X:{-Spec().HalfLength,0.f,Spec().HalfLength})for(float Y:{-Spec().HalfWidth,Spec().HalfWidth}){
   const float Z=World->HeightAt(FVector2D(At+Rotation.RotateVector(FVector(X,Y,0))));Low=FMath::Min(Low,Z);High=FMath::Max(High,Z);
  }
  if(High-Low>80)continue;At.Z=High+75;
  if(!Clear(At))continue;SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);SyncRecord();return true;
 }
 // A blocked spawn remains hidden and retries; never publish overlapping geometry.
 return false;
}
