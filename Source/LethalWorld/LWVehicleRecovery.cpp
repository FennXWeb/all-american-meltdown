#include "LWVehicle.h"
#include "LWWorld.h"
#include "Components/BoxComponent.h"
#include "ProceduralMeshComponent.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
void ALWVehicle::RecoverTerrain(float Dt){
 if(Driver||ConvoyOwner||Chauffeur||AutoDriving||Boarding||FMath::Abs(Speed)>1)return;
 RecoveryClock+=Dt;if(RecoveryClock<.5f)return;RecoveryClock=0;
 const FVector At=GetActorLocation();FCollisionQueryParams Q(NAME_None,false,this);float Lift=0;
 for(float X:{-Spec().HalfLength,0.f,Spec().HalfLength})for(float Y:{-Spec().HalfWidth,Spec().HalfWidth}){
  const FVector Bottom=GetActorRotation().RotateVector(FVector(X,Y,-Chassis->GetUnscaledBoxExtent().Z));FHitResult H;FVector S=At+Bottom;
  if(GetWorld()->LineTraceSingleByChannel(H,S+FVector(0,0,700),S-FVector(0,0,600),ECC_Visibility,Q)){
   const auto* C=Cast<ALWChunk>(H.GetActor());if(C&&H.GetComponent()==C->Terrain&&H.ImpactNormal.Z>.65f)Lift=FMath::Max(Lift,float(H.ImpactPoint.Z-S.Z+5));
  }
 }
 if(Lift<1||Lift>400)return;
 FVector Next=At+FVector(0,0,FMath::Min(Lift,15.f));TArray<FOverlapResult> Hits;
 GetWorld()->OverlapMultiByChannel(Hits,Next,GetActorQuat(),ECC_Pawn,FCollisionShape::MakeBox(Chassis->GetUnscaledBoxExtent()),Q);
 for(const auto& H:Hits)if(H.bBlockingHit){const auto* C=Cast<ALWChunk>(H.GetActor());if(!C||H.GetComponent()!=C->Terrain)return;}
 SetActorLocation(Next,false,nullptr,ETeleportType::TeleportPhysics);SyncRecord();
}
