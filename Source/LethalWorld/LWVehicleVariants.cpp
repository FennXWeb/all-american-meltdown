#include "LWAircraft84.h"
#include "LWVehicle.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"
FVector ALWVehicle::SeatLocation(int Seat)const{if(IsAircraft84())return LWAviation84::Seat(Spec().Id,Seat);if(IsSolarRV74()){const FVector Seats[]={FVector(60,65,110),FVector(-165,-85,110),FVector(-345,-85,110),FVector(-212,86,110),FVector(-300,86,110)};auto V=Seats[FMath::Clamp(Seat,0,4)];if(Seat>0)V.Y+=(Seat<3?-80:80)*SlideAlpha66;return CabinToActor74(V); }if(FName(Spec().Id)==TEXT("apex_ev"))return FVector(LWTraffic::FrontOffset(Spec())-48,43,-44);if(IsExpansion57()){float F=LWTraffic::FrontOffset(Spec());if(HasTurret57()&&Seat==0)return FVector(F-170,0,FName(Spec().Id)==TEXT("apc")?160:62);if(IsHelicopter57())return FVector(F+(Seat==0?10:-140),Seat==0?43:Seat==1?-60:60,-25);if(FName(Spec().Id)==TEXT("apc"))return FVector(F-160-((Seat-1)/2)*100,Seat%2?-63:63,-30);return FVector(F-43,43,-30);}const auto& S=Spec();if(FName(S.Id)==TEXT("bus"))return FVector(LWTraffic::FrontOffset(S)-112-(Seat/2)*100,Seat%2?43:-43,-30);if(FName(S.Id)==TEXT("boxtruck"))return FVector(LWTraffic::FrontOffset(S)-43,43,-3);if(FName(S.Id)==TEXT("supercar"))return FVector(LWTraffic::FrontOffset(S)-43,43,-50);if(FName(S.Id)==TEXT("rv")){const FVector Seats[]={FVector(60,65,61),FVector(-165,-85,61),FVector(-345,-85,61),FVector(-212,86,61),FVector(-300,86,61)};FVector At=Seats[FMath::Clamp(Seat,0,4)]+FVector(LWTraffic::FrontOffset(S),0,-26);if(Seat>0)At.Y+=(Seat<3?-80:80)*SlideAlpha66;return At;}if(FName(S.Id)==TEXT("suv")&&Seat>0)return FVector(LWTraffic::FrontOffset(S)+(Seat<4?-112:-205),Seat<4?(Seat-2)*55:(Seat==4?-43:43),-30);return FVector(LWTraffic::FrontOffset(S)+(Seat==0?-43:-112-((Seat-1)/2)*100),Seat==0?43:((Seat-1)%2?-43:43),-30);}
void ALWVehicle::BuildVariant(){
 const auto& S=Spec();FName Id(S.Id);float F=LWTraffic::FrontOffset(S);Root->SetRelativeLocation(FVector(F,0,-75));Chassis->SetBoxExtent(FVector(S.HalfLength,S.HalfWidth,48));
 const FName ArtId=IsSolarRV74()?FName(TEXT("rv")):IsElectric74()?FName(TEXT("supercar")):Id;
 Body->SetStaticMesh(World->Mesh(IsExpansion57()?FName(*(FString(TEXT("Vehicle42_"))+S.Id)):FName(*(FString(TEXT("Vehicle_"))+ArtId.ToString()+TEXT("_V9")))));
 if(Id==TEXT("supercar"))Body->SetStaticMesh(World->Mesh(TEXT("SupercarV21")));
 for(int I=0;I<Body->GetNumMaterials();I++){auto* Base=Body->GetMaterial(I);if(!Base)continue;FString Name=Base->GetName();if(Name.Contains(TEXT("Steel"))||Name.Contains(TEXT("Red"))||Name.Contains(TEXT("Bone"))||Name.Contains(TEXT("Lane"))){auto* M=UMaterialInstanceDynamic::Create(World->Material(TEXT("VehiclePaintV9")),this);FLinearColor Tint=Name.Contains(TEXT("Red"))?FLinearColor(.6f,.12f,.07f):Name.Contains(TEXT("Lane"))?FLinearColor(.8f,.55f,.08f):Name.Contains(TEXT("Bone"))?FLinearColor(.8f,.8f,.7f):FLinearColor(.35f,.42f,.4f);M->SetVectorParameterValue(TEXT("PaintTint"),Tint);Body->SetMaterial(I,M);PaintWet.Add(M);}}
 auto B=[&](FVector V,FVector Size,FName Mat){Part(TEXT("Cube"),V,Size/100,FRotator::ZeroRotator,Mat);auto* C=Details.Last().Get();C->SetCollisionEnabled(ECollisionEnabled::NoCollision);return C;};
 auto Tag=[&](FVector V,FName Action){auto* C=B(V,FVector(3,4,2),TEXT("Steel"));C->ComponentTags.Add(Action);C->SetCollisionEnabled(ECollisionEnabled::QueryOnly);C->SetCollisionResponseToAllChannels(ECR_Ignore);C->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);};
 if(auto* C=World->Containers.Find(RecordId)){if(C->Items.IsEmpty()){C->Width=S.CargoW;C->Height=S.CargoH;}else{C->Width=FMath::Max(C->Width,S.CargoW);C->Height=FMath::Max(C->Height,S.CargoH);}}
 else{FLWContainerRecord NewCargo;NewCargo.Id=RecordId;NewCargo.Width=S.CargoW;NewCargo.Height=S.CargoH;NewCargo.Position=GetActorLocation();World->Containers.Add(RecordId,NewCargo);}
 if(Id==TEXT("dirtbike")){
 for(auto& C:Details){C->SetVisibility(false);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);}Wheels.Empty();
 for(int Sign:{-1,1}){Part(TEXT("SedanWheelV5"),FVector(Sign*72,0,32),FVector(1,.35,1));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);Wheels.Add(Details.Last());}
 Tag(FVector(40,0,121),TEXT("ignition"));Tag(FVector(40,-15,121),TEXT("lights"));Tag(FVector(-30,0,95),TEXT("glove"));Tag(FVector(-20,-20,90),TEXT("feature"));FeaturePanel=B(FVector(-25,-24,32),FVector(8,5,62),TEXT("Steel"));return;}
 for(int I=-1;I<S.Seats-1;I++){if(IsCamper74())continue;FVector V=I<0?FVector(-43,-43,0):SeatLocation(I)-FVector(F,0,-30);if(Id==TEXT("supercar"))V.Z=0;B(V+FVector(0,0,44),FVector(48,45,8),TEXT("Steel"));B(V+FVector(0,0,57),FVector(52,55,18),TEXT("Cloth"));B(V+FVector(-27,0,88),FVector(12,55,60),TEXT("Cloth"));B(V+FVector(-27,0,125),FVector(12,31,18),TEXT("Rubber"));}
 for(int I=0;I<Wheels.Num();I++){if(Id==TEXT("supercar"))Wheels[I]->SetStaticMesh(World->Mesh(TEXT("SuperWheelV21")));FVector V=Wheels[I]->GetRelativeLocation();if(Id==TEXT("supercar"))V.Z=35;V.X=I%2?132:132-S.Wheelbase;V.Y=FMath::Sign(V.Y)*(S.HalfWidth-8);Wheels[I]->SetRelativeLocation(V);}
 for(int I=0;I<Wipers.Num();I++){Wipers[I]->SetRelativeLocation(FVector(70,I?50:-50,113));Wipers[I]->SetRelativeScale3D(FVector(.8,I?-.8:.8,.8));}
 float Top=Id==TEXT("rv")?268:Id==TEXT("boxtruck")||Id==TEXT("rv")||Id==TEXT("bus")||Id==TEXT("van")?208:157;
 Windshield=B(FVector(36,0,(113+Top)*.5f),FVector(.3f,(S.HalfWidth-15)*2,FVector(-68,0,Top-113).Size()),TEXT("WindshieldV9"));Windshield->SetRelativeRotation(FQuat::FindBetweenNormals(FVector::UpVector,FVector(-68,0,Top-113).GetSafeNormal()));
 GlassWet=Windshield->CreateDynamicMaterialInstance(0);Tag(FVector(30,-7,115),TEXT("feature"));
 float Rear=220-S.HalfLength*2;FName Feature(S.Feature);
 if(Feature==TEXT("ramp")||Feature==TEXT("door")||Feature==TEXT("tailgate"))FeaturePanel=B(FVector(Rear,0,Feature==TEXT("tailgate")?94:(S.Height+45)*.5f),FVector(6,S.HalfWidth*2,Feature==TEXT("tailgate")?90:S.Height-45),TEXT("Steel"));
 if(Id==TEXT("supercar")){
  for(auto& C:Details){if(Wheels.Contains(C))continue;if(C->GetStaticMesh()==World->Mesh(TEXT("VehicleCabinV9")))C->SetRelativeScale3D(FVector(1,1,.82));else C->AddLocalOffset(FVector(0,0,-20));}
  for(auto& L:Headlamps)L->AddLocalOffset(FVector(0,0,-16));for(auto& L:Indicators)L->AddLocalOffset(FVector(0,0,-16));
 }
 if(IsCamper74())BuildCamper();
 if(Id==TEXT("police")){Tag(FVector(30,12,115),TEXT("siren"));for(int Side:{-1,1}){B(FVector(-65,Side*35,170),FVector(20,50,12),Side<0?TEXT("Red"):TEXT("Glow"));auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(Root);L->SetRelativeLocation(FVector(-65,Side*35,178));L->SetLightColor(Side<0?FLinearColor::Red:FLinearColor::Blue);L->SetIntensity(45000);L->SetAttenuationRadius(2200);L->SetCastShadows(false);L->SetVisibility(false);L->RegisterComponent();EmergencyLights.Add(L);}}
}
void ALWVehicle::TickFeatures(float Dt){
 for(int I=0;I<EmergencyLights.Num();I++)EmergencyLights[I]->SetVisibility(FeatureOn&&(int(GetWorld()->GetTimeSeconds()*7)%2==I));
 if(SirenOn){if(!IsValid(SirenAudio)){SirenAudio=World->Sound(TEXT("PoliceSiren"),GetActorLocation(),.8f,1,true);if(SirenAudio)SirenAudio->AttachToComponent(Root,FAttachmentTransformRules::KeepWorldTransform);}World->Noise(GetActorLocation(),9000);}else if(SirenAudio){SirenAudio->Stop();SirenAudio=nullptr;}
 if(FeaturePanel){const auto& S=Spec();FName Type(S.Feature);if(Type==TEXT("door")||Type==TEXT("ramp")||Type==TEXT("tailgate")||Type==TEXT("stand")){
 float Open=Type==TEXT("ramp")?100.f:90.f;FeatureAngle=FMath::FInterpTo(FeatureAngle,FeatureOn?Open:0,Dt,3);
 float Rear=220-S.HalfLength*2;FVector Pivot,Offset;FRotator Rotation;
 if(Type==TEXT("door")){Pivot=FVector(Rear,-S.HalfWidth,(S.Height+45)*.5f);Offset=FVector(0,S.HalfWidth,0);Rotation=FRotator(0,FeatureAngle,0);}
 else if(Type==TEXT("stand")){Pivot=FVector(-25,-24,63);Offset=FVector(0,0,-31);Rotation=FRotator(0,0,FeatureAngle*.4f);}
 else{Pivot=FVector(Rear,0,Type==TEXT("tailgate")?49:45);Offset=FVector(0,0,Type==TEXT("tailgate")?45:(S.Height-45)*.5f);Rotation=FRotator(FeatureAngle,0,0);}
 FeaturePanel->SetRelativeRotation(Rotation);FeaturePanel->SetRelativeLocation(Pivot+Rotation.RotateVector(Offset));
 }}
}
