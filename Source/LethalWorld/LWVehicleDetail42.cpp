#include "LWVehicle.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/SpotLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"

void ALWVehicle::BuildDetail42(){
 const FName Id(Spec().Id);const bool Camper=Id==TEXT("rv"),Bike=Id==TEXT("dirtbike"),Raised=Id==TEXT("bus")||Id==TEXT("boxtruck");
 Body->EmptyOverrideMaterials();Body->SetStaticMesh(World->Mesh(FName(*(FString(TEXT("Vehicle42_"))+Spec().Id))));PaintWet.Empty();
 for(int i=0;i<Body->GetNumMaterials();++i)if(auto* Base=Body->GetMaterial(i))if(Base->GetName().Contains(TEXT("V42_Paint"))){auto* M=Body->CreateDynamicMaterialInstance(i);PaintWet.Add(M);}
 for(int i=0;i<Body->GetNumMaterials();++i)if(auto* M=Body->GetMaterial(i)){if(M->GetName()==TEXT("M_V42_Lamp"))HeadLens42=Body->CreateDynamicMaterialInstance(i);if(M->GetName()==TEXT("M_V42_RedLamp"))BrakeLens42=Body->CreateDynamicMaterialInstance(i);}
 // Keep interaction hit targets, moving hardware and camper furnishings; remove
 // obsolete cubes and old cabin overlays rather than stacking the new models on them.
 for(auto& C:Details){
  if(!IsValid(C)||Wheels.Contains(C)||Wipers.Contains(C)||C==SteeringWheel||C==GloveLid||C==Windshield||C==FeaturePanel||C==CamperDoor)continue;
  if(Id==TEXT("police")&&FMath::IsNearlyEqual(C->GetRelativeLocation().Z,170.f)&&FMath::IsNearlyEqual(C->GetRelativeLocation().X,-65.f)){continue;}
  if(Camper&&!C->ComponentTags.IsEmpty()){const FName T=C->ComponentTags[0];if(T==TEXT("radio")||T==TEXT("glove")||T==TEXT("ignition")||T==TEXT("lights")||T==TEXT("wipers")||T==TEXT("left")||T==TEXT("right")||T==TEXT("feature"))C->AddLocalOffset(FVector(50,0,45));}
  if(Raised&&!C->ComponentTags.IsEmpty()){const FName T=C->ComponentTags[0];if(T==TEXT("radio")||T==TEXT("glove")||T==TEXT("ignition")||T==TEXT("lights")||T==TEXT("wipers")||T==TEXT("left")||T==TEXT("right"))C->AddLocalOffset(FVector(0,0,27));}
  const FString MeshName=C->GetStaticMesh()?C->GetStaticMesh()->GetName():TEXT("");
  if(Camper&&MeshName.StartsWith(TEXT("SM_Camper"))&&!MeshName.Contains(TEXT("Shell"))&&!MeshName.Contains(TEXT("Glass"))){
   if(MeshName.Contains(TEXT("Seat"))){C->SetStaticMesh(World->Mesh(TEXT("Seat42")));C->EmptyOverrideMaterials();}
   if(C->ComponentTags.Contains(TEXT("wardrobe"))){C->SetStaticMesh(World->Mesh(TEXT("Camper42Wardrobe")));C->EmptyOverrideMaterials();continue;}
   for(const TCHAR* Name:{TEXT("Kitchen"),TEXT("Sink"),TEXT("Fridge"),TEXT("Bed")})if(MeshName.Contains(Name)){C->SetStaticMesh(World->Mesh(FName(*(FString(TEXT("Camper42"))+Name))));C->EmptyOverrideMaterials();}
   continue;
  }
  if(Camper&&!C->ComponentTags.IsEmpty()&&(C->ComponentTags[0].ToString().StartsWith(TEXT("cargo_"))||C->ComponentTags[0].ToString().EndsWith(TEXT("_storage"))))continue;
  C->SetVisibility(false);if(C->ComponentTags.IsEmpty())C->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 }
 if(Id==TEXT("police")){Part(TEXT("Cube"),FVector(-65,0,165),FVector(.24f,1.12f,.04f),FRotator::ZeroRotator,TEXT("V42_Vinyl"));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
 Part(FName(*(FString(TEXT("Cabin42_"))+Spec().Id)),FVector::ZeroVector,FVector(1));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 for(auto& W:Wheels){W->EmptyOverrideMaterials();W->SetStaticMesh(World->Mesh(FName(*(FString(TEXT("Wheel42_"))+Spec().Id))));W->SetRelativeScale3D(FVector(1));}
 SteeringWheel->SetStaticMesh(World->Mesh(TEXT("Steering42")));SteeringWheel->EmptyOverrideMaterials();SteeringWheel->SetRelativeScale3D(FVector(1));
 if(!Bike&&Windshield){Windshield->SetStaticMesh(World->Mesh(FName(*(FString(TEXT("Windshield42_"))+Spec().Id))));Windshield->SetRelativeLocation(FVector::ZeroVector);Windshield->SetRelativeRotation(FRotator::ZeroRotator);Windshield->SetRelativeScale3D(FVector(1));Windshield->SetMaterial(0,World->Material(TEXT("WindshieldV9")));GlassWet=Windshield->CreateDynamicMaterialInstance(0);}
 if(GloveLid){for(int i=0;i<GloveLid->GetNumMaterials();++i)GloveLid->SetMaterial(i,World->Material(TEXT("V42_Leather")));if(Raised)GloveLid->AddLocalOffset(FVector(0,0,27));}
 if(Camper&&GloveLid)GloveLid->AddLocalOffset(FVector(50,0,45));
 if(Camper&&CamperDoor){CamperDoor->SetStaticMesh(World->Mesh(TEXT("Camper42Door")));CamperDoor->EmptyOverrideMaterials();}
 SteeringWheel->ComponentTags.AddUnique(TEXT("horn"));SteeringWheel->SetCollisionEnabled(ECollisionEnabled::QueryOnly);SteeringWheel->SetCollisionResponseToAllChannels(ECR_Ignore);SteeringWheel->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 if(!Bike){SteeringWheel->SetRelativeLocation(Camper?FVector(122,-55,162):FVector(-2,-43,Id==TEXT("supercar")?78:Raised?125:98));}
 const float X=Camper?139.f:Bike?27.f:9.f,Z=Camper?176.f:Bike?118.f:Id==TEXT("supercar")?91.f:Raised?138.f:111.f;
 const TCHAR* Names[]={TEXT("Speed"),TEXT("RPM"),TEXT("Fuel"),TEXT("Temp")};
 for(int i=0;i<(Bike?2:4);++i){
  const FVector At(X,Bike?(-7.f+i*14.f):(i==0?-58.f:i==1?-34.f:i==2?-77.f:-15.f),Z);
  const float Scale=Bike?.55f:i<2?1.f:.62f;
  Part(FName(*(FString(TEXT("Dial42"))+Names[i])),At,FVector(Scale));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  Part(TEXT("Needle42"),At-FVector(.25f,0,0),FVector(Scale));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);GaugeNeedles42.Add(Details.Last());
 }
 GearDisplay42=NewObject<UTextRenderComponent>(this);GearDisplay42->SetupAttachment(Root);GearDisplay42->SetRelativeLocation(FVector(X-.6f,Bike?0:-46.f,Z-12));GearDisplay42->SetRelativeRotation(FRotator(0,180,0));GearDisplay42->SetHorizontalAlignment(EHTA_Center);GearDisplay42->SetWorldSize(Bike?3.f:4.f);GearDisplay42->SetTextRenderColor(FColor(155,215,154));GearDisplay42->SetCastShadow(false);GearDisplay42->RegisterComponent();
 for(int i=0;i<4;++i){Part(TEXT("Cube"),FVector(X-1,-62+i*10,Z-14),FVector(.006f,.025f,.014f),FRotator::ZeroRotator,TEXT("V42_Amber"));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);GaugeLamps42.Add(Details.Last());}
 if(Bike){SteeringWheel->SetVisibility(false);GloveLid->SetVisibility(false);}
 TickInstruments42(0);
}
void ALWVehicle::TickInstruments42(float Dt){
 const auto* R=Record();if(!R)return;HornCooldown42=FMath::Max(0.f,HornCooldown42-Dt);
 if(HeadLens42)HeadLens42->SetScalarParameterValue(TEXT("LampPower"),Headlights?6.f:.08f);
 if(BrakeLens42)BrakeLens42->SetScalarParameterValue(TEXT("LampPower"),WasBrake42?7.f:Headlights?1.5f:.08f);
 const float TempTarget=EngineOn?88.f+(1-FMath::Clamp(R->Health/200.f,0.f,1.f))*35.f:20.f;
 Coolant42=FMath::FInterpTo(Coolant42,TempTarget,Dt,EngineOn?.018f:.009f);
 const float Values[]={FMath::Clamp(FMath::Abs(Speed)*.0223694f/160.f,0.f,1.f),FMath::Clamp(EngineRPM/10000.f,0.f,1.f),FMath::Clamp(FuelLitres()/FMath::Max(1.f,FuelCapacity()),0.f,1.f),FMath::Clamp((Coolant42-20)/110.f,0.f,1.f)};
 for(int i=0;i<GaugeNeedles42.Num();++i)GaugeNeedles42[i]->SetRelativeRotation(FRotator(0,0,(-130.f+260.f*Values[i])));
 if(GearDisplay42){FString Gear=!EngineOn?TEXT("OFF"):Speed<-5?TEXT("R"):FMath::Abs(Speed)<5?TEXT("N"):FString::FromInt(AudioGear);GearDisplay42->SetText(FText::FromString(Gear));}
 if(GaugeLamps42.Num()==4){const bool Blink=FMath::Fmod(SignalClock,1.f)<.5f;GaugeLamps42[0]->SetVisibility(Signal==-1&&Blink);GaugeLamps42[1]->SetVisibility(Headlights);GaugeLamps42[2]->SetVisibility(EngineOn&&(Values[2]<.15f||R->Health<30));GaugeLamps42[3]->SetVisibility(Signal==1&&Blink);}
}
