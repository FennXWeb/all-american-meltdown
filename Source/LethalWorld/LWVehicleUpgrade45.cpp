#include "LWVehicle.h"
#include "LWGarage45.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "ProceduralMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Engine/OverlapResult.h"
bool ALWVehicle::TryIncline45(FVector Desired,FRotator Rotation){
 FCollisionQueryParams Q(NAME_None,false,this);if(Driver)Q.AddIgnoredActor(Driver);if(Chauffeur)Q.AddIgnoredActor(Chauffeur);for(const auto& N:Passengers)if(N)Q.AddIgnoredActor(N);
 const FCollisionShape Shape=FCollisionShape::MakeBox(Chassis->GetUnscaledBoxExtent()+FVector(2));
 for(float LiftAmount:{15.f,30.f,50.f,75.f,105.f,140.f,180.f}){
  FVector Target=Desired+FVector(0,0,LiftAmount);
  if(GetWorld()->OverlapBlockingTestByChannel(Target,Rotation.Quaternion(),ECC_WorldDynamic,Shape,Q))continue;
  bool Clear=true;
  for(int I=1;I<=4&&Clear;++I){
   TArray<FOverlapResult> Hits;GetWorld()->OverlapMultiByChannel(Hits,FMath::Lerp(GetActorLocation(),Target,I/4.f),Rotation.Quaternion(),ECC_WorldDynamic,Shape,Q);
   for(const auto& H:Hits)if(H.bBlockingHit){auto* C=Cast<ALWChunk>(H.GetActor());if(!C||!(H.GetComponent()==C->Terrain||(H.GetComponent()&&H.GetComponent()->ComponentHasTag(TEXT("RoadSurface"))))){Clear=false;break;}}
  }
  if(Clear){SetActorLocationAndRotation(Target,Rotation,false,nullptr,ETeleportType::TeleportPhysics);SyncRecord();return true;}
 }return false;
}
void ALWVehicle::ApplyGarage45(){
 if(!World||!Record())return;const auto* R=Record();const TCHAR* Colors[]={TEXT("sedan"),TEXT("police"),TEXT("boxtruck"),TEXT("rv"),TEXT("bus"),TEXT("van"),TEXT("pickup"),TEXT("dirtbike"),TEXT("suv"),TEXT("muscle"),TEXT("supercar")};
 if(R->Paint45>=0&&R->Paint45<11&&Body->GetStaticMesh()){
  PaintWet.Empty();
  for(int I=0;I<Body->GetNumMaterials();++I)if(auto* Original=Body->GetStaticMesh()->GetMaterial(I))if(Original->GetName().Contains(TEXT("V42_Paint"))){
   Body->SetMaterial(I,World->Material(FName(*(FString(TEXT("V42_Paint_"))+Colors[R->Paint45]))));auto* M=Body->CreateDynamicMaterialInstance(I);PaintWet.Add(M);if(DentedBody)DentedBody->SetMaterial(I,M);
  }
 }
 for(auto& L:Headlamps)L->SetIntensity(900000*(1+LWGarage45::Stat(R,TEXT("light"))));
 if(auto* C=World->Containers.Find(RecordId)){C->Height=FMath::Max(C->Height,Spec().CargoH+int(LWGarage45::Stat(R,TEXT("cargo"))));}
}

