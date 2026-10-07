#include "LWVehicle.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInterface.h"

void ALWVehicle::RepairCoach80(){
 if(!IsCamper74())return;
 const bool Electric=IsSolarRV74();
 for(auto& C:Details){
  if(!IsValid(C)||!C->GetStaticMesh())continue;
  const FString Name=C->GetStaticMesh()->GetName();
  if(!Electric&&Name==TEXT("SM_RV66_Interior")){C->EmptyOverrideMaterials();C->SetStaticMesh(World->Mesh(TEXT("RV80_Interior")));}
  if(!Electric&&Name.StartsWith(TEXT("SM_Dial42"))){FVector At=C->GetRelativeLocation();At.X=119;C->SetRelativeLocation(At);C->SetVisibility(true);}
 }
 auto Add=[&](FName Mesh,FVector At,FRotator Rotation=FRotator::ZeroRotator){
  Part(Mesh,At,FVector(1),Rotation);auto* C=Details.Last().Get();
  C->SetCollisionEnabled(ECollisionEnabled::QueryOnly);C->SetCollisionResponseToAllChannels(ECR_Ignore);C->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
  if(Electric)for(int I=0;I<C->GetNumMaterials();I++)if(auto* Mat=C->GetMaterial(I)){
   const FString Name=Mat->GetName();
   if(Name.Contains(TEXT("RV66_Ivory")))C->SetMaterial(I,World->Material(TEXT("EV74_Ivory")));
   else if(Name.Contains(TEXT("RV66_Black")))C->SetMaterial(I,World->Material(TEXT("EV74_Carbon")));
   else if(Name.Contains(TEXT("RV66_Walnut")))C->SetMaterial(I,World->Material(TEXT("EV74_Walnut")));
  }
  return C;
 };
 Add(TEXT("RV80_Joinery"),FVector::ZeroVector);
 if(Electric)Add(TEXT("RV80_ElectricLowerDash"),FVector::ZeroVector);
 for(int Side:{-1,1})SlideSeals80.Add(Add(TEXT("RV80_SlideSeal"),FVector(-255,Side*110,64),FRotator(0,Side<0?180:0,0)));
 if(!Electric){
  for(auto& C:GaugeNeedles42){FVector At=C->GetRelativeLocation();At.X=118.7f;C->SetRelativeLocation(At);C->SetVisibility(true);}
  for(auto& C:GaugeLamps42){FVector At=C->GetRelativeLocation();At.X=118.4f;C->SetRelativeLocation(At);}
  if(GearDisplay42){GearDisplay42->SetRelativeLocation(FVector(118.4f,-46,164));GearDisplay42->SetVisibility(true);}
  SteeringWheel->SetRelativeLocation(FVector(103,-55,162));
 }
 const TPair<FName,FVector> Controls[]={
  {TEXT("radio"),FVector(118,4,168)},{TEXT("glove"),FVector(113,73,138)},
  {TEXT("ignition"),FVector(119,-90,156)},{TEXT("lights"),FVector(119,-95,172)},
  {TEXT("wipers"),FVector(119,30,171)},{TEXT("left"),FVector(105,-55,162)},
  {TEXT("right"),FVector(105,-55,162)}};
 for(const auto& Control:Controls)for(auto& C:Details)if(IsValid(C)&&C->ComponentHasTag(Control.Key)){
  FVector At=Control.Value+(Electric?FVector(-7,0,-23):FVector::ZeroVector);
  if(Electric&&Control.Key==TEXT("glove"))At=FVector(107,77,138);
  if(Control.Key==TEXT("left")||Control.Key==TEXT("right"))At=Electric?FVector(117,-55,170):FVector(105,-55,162);
  C->SetRelativeLocation(At);C->SetVisibility(Control.Key!=TEXT("glove"));
  if(Control.Key!=TEXT("glove")){
   const bool Stalk=Control.Key==TEXT("left")||Control.Key==TEXT("right");
   const FName Mesh=Stalk?TEXT("RV80_Stalk"):Control.Key==TEXT("radio")?TEXT("RV80_Radio"):Control.Key==TEXT("ignition")?TEXT("RV80_Ignition"):TEXT("RV80_Rocker");
   C->EmptyOverrideMaterials();C->SetStaticMesh(World->Mesh(Mesh));C->SetRelativeScale3D(FVector(1));C->SetRelativeRotation(FRotator(0,0,Control.Key==TEXT("left")?180:0));
  }
  C->SetCollisionEnabled(ECollisionEnabled::QueryOnly);C->SetCollisionResponseToAllChannels(ECR_Ignore);C->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 }
 // The new integrated glove compartment is the interaction surface. The old lid
 // had a sedan-sized pivot and sat behind the coach fascia.
 if(GloveLid){GloveLid->SetVisibility(false);GloveLid->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
 TickLuxury66(0);TickInstruments42(0);
}
