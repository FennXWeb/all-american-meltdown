#include "LWAircraft84.h"
#include "Engine/StaticMesh.h"
#include "Components/SpotLightComponent.h"
#include "Components/BoxComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"
#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
void ALWVehicle::BuildCamper(){
 Body->EmptyOverrideMaterials();Body->SetStaticMesh(World->Mesh(TEXT("CamperShellV17")));
 // Retain functional switch components, but place them on the coach dashboard.
 for(auto& C:Details){if(C->GetStaticMesh()&&C->GetStaticMesh()->GetName()==TEXT("SM_VehicleCabinV9")){C->SetVisibility(false);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);}if(!C->ComponentTags.IsEmpty())C->AddLocalOffset(FVector(80,0,20));}
 SteeringWheel->SetRelativeLocation(FVector(92,-55,135));GloveLid->SetRelativeLocation(FVector(112,54,94));
 for(int I=0;I<Headlamps.Num();I++)Headlamps[I]->SetRelativeLocation(FVector(206,I?97:-97,110));
 for(int I=0;I<Indicators.Num();I++)Indicators[I]->SetRelativeLocation(FVector(206,I?99:-99,130));
 for(int I=0;I<Wheels.Num();I++){Wheels[I]->SetRelativeLocation(FVector(I%2?132:-605,I<2?-125:125,43));Wheels[I]->SetRelativeScale3D(FVector(1.45,1.2,1.45));}
 for(int Side:{-1,1}){Part(TEXT("SedanWheelV5"),FVector(-755,Side*125,43),FVector(1.45,1.2,1.45));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);Wheels.Add(Details.Last());}
 PaintWet.Empty();for(int I=0;I<Body->GetNumMaterials();I++)if(Body->GetMaterial(I)&&Body->GetMaterial(I)->GetName().Contains(TEXT("Pearl"))){auto* M=Body->CreateDynamicMaterialInstance(I);PaintWet.Add(M);}
 if(Windshield){Windshield->SetRelativeLocation(FVector(186,0,217));Windshield->SetRelativeScale3D(FVector(.003,2.26,FVector(-24,0,142).Size()/100));Windshield->SetRelativeRotation(FQuat::FindBetweenNormals(FVector::UpVector,FVector(-24,0,142).GetSafeNormal()));}
 for(int I=0;I<Wipers.Num();I++)Wipers[I]->SetRelativeLocation(FVector(198,I?58:-58,146));
 for(int Side:{-1,1})for(FVector2D Window:{FVector2D(-900,174),FVector2D(-710,174),FVector2D(-510,174),FVector2D(-310,174),FVector2D(-110,174),FVector2D(90,174)}){if(Side==1&&Window.X==90)continue;Part(TEXT("Cube"),FVector(Window.X,Side*123.5,238),FVector((Window.Y-8)/100,.002,.80),FRotator::ZeroRotator,TEXT("CamperGlassV14"));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);}

 auto Add=[&](FName Mesh,FVector V,FName Action,FRotator Rot=FRotator::ZeroRotator){Part(Mesh,V,FVector(1),Rot);auto* C=Details.Last().Get();C->SetCollisionEnabled(ECollisionEnabled::QueryOnly);C->SetCollisionResponseToAllChannels(ECR_Ignore);C->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);if(!Action.IsNone())C->ComponentTags.Add(Action);};
 Add(TEXT("CamperDoorV17"),FVector(85,126,61),TEXT("entry"));CamperDoor=Details.Last();auto* DoorHit=NewObject<UBoxComponent>(this);DoorHit->SetupAttachment(CamperDoor);DoorHit->SetRelativeLocation(FVector(-50,0,106));DoorHit->SetBoxExtent(FVector(49,4,105));DoorHit->SetCollisionEnabled(ECollisionEnabled::QueryOnly);DoorHit->SetCollisionResponseToAllChannels(ECR_Ignore);DoorHit->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);DoorHit->SetCollisionResponseToChannel(ECC_Pawn,ECR_Block);DoorHit->ComponentTags.Add(TEXT("entry"));DoorHit->RegisterComponent();
 Part(TEXT("Cube"),FVector(35,126,277),FVector(1.06,.12,.10),FRotator::ZeroRotator,TEXT("Steel"));
 Part(TEXT("Cube"),FVector(133,123.5,238),FVector(.90,.002,.80),FRotator::ZeroRotator,TEXT("CamperGlassV14"));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 // Frame and step treads surround a clear metre-wide doorway.
 for(float X:{-18.f,88.f}){Part(TEXT("Cube"),FVector(X,126,167),FVector(.06,.12,2.12),FRotator::ZeroRotator,TEXT("Steel"));}
 for(int I=0;I<3;I++){Part(TEXT("Cube"),FVector(35,145+I*23,51-I*17),FVector(.94,.25,.08),FRotator::ZeroRotator,TEXT("Steel"));}
 for(int Side:{-1,1})for(int I=0;I<7;I++){FName Action(*FString::Printf(TEXT("cargo_%d_%d"),Side,I));Add(TEXT("Cube"),FVector(-870+I*125,Side*128,93),Action);Details.Last()->SetRelativeScale3D(FVector(1.18,.035,.48));Details.Last()->SetMaterial(0,World->Material(TEXT("Steel")));}
 Add(TEXT("CamperSeatV14"),FVector(60,-55,61),TEXT("seat_-1"));
 for(int I=0;I<4;I++)Add(TEXT("CamperSeatV14"),SeatLocation(I)-FVector(LWTraffic::FrontOffset(Spec()),0,-26),FName(*FString::Printf(TEXT("seat_%d"),I)));
 Add(TEXT("CamperBedV14"),FVector(-892,0,61),TEXT("bed"));
 Add(TEXT("CamperKitchenV14"),FVector(-535,-95,61),TEXT("cooker"),FRotator(0,180,0));
 Add(TEXT("CamperSinkV14"),FVector(-640,-95,61),TEXT("sink"),FRotator(0,180,0));
 Add(TEXT("CamperFridgeV14"),FVector(-430,94,61),TEXT("fridge"));
 // Accessible wardrobe, external cargo and pantry keep independent persistent records.
 for(FName Name:{FName(TEXT("fridge")),FName(TEXT("pantry")),FName(TEXT("wardrobe"))}){FName Key(*(RecordId.ToString()+TEXT("_")+Name.ToString()));if(!World->Containers.Contains(Key)){FLWContainerRecord C;C.Id=Key;C.Context=Name;C.Width=Name==TEXT("wardrobe")?10:8;C.Height=Name==TEXT("wardrobe")?12:6;C.Position=GetActorLocation();World->Containers.Add(Key,C);}}
 Add(TEXT("CamperFridgeV14"),FVector(-774,94,61),TEXT("wardrobe"));Details.Last()->SetRelativeScale3D(FVector(1,1,1));
 Add(TEXT("CamperKitchenV14"),FVector(-535,-100,266),TEXT("pantry"),FRotator(0,180,0));Details.Last()->SetRelativeScale3D(FVector(1,.65,.4));
 for(auto Pair:{TPair<FName,FVector>(TEXT("kitchen_storage"),FVector(-535,-65,101)),TPair<FName,FVector>(TEXT("sink_storage"),FVector(-640,-65,101)),TPair<FName,FVector>(TEXT("bed_storage"),FVector(-818,0,89))}){Add(TEXT("Cube"),Pair.Value,Pair.Key);Details.Last()->SetRelativeScale3D(FVector(.72,.04,.60));if(Pair.Key==TEXT("bed_storage"))Details.Last()->SetRelativeScale3D(FVector(.04,1.65,.45));Details.Last()->SetMaterial(0,World->Material(TEXT("Wood")));}
 for(float X:{-810.f,-410.f,-30.f}){auto* L=NewObject<UPointLightComponent>(this);L->ComponentTags.Add(TEXT("LegacyCamperCabinLight"));L->SetupAttachment(Root);L->SetRelativeLocation(FVector(X,0,295));L->SetIntensity(11000);L->SetAttenuationRadius(650);L->SetLightColor(FLinearColor(1,.83,.6));L->SetCastShadows(true);L->RegisterComponent();}
}

void ALWVehicle::StandInCamper(){if(IsAircraft84()){StandAircraft84();return;}
 if(!Driver||!IsCamper74()||Driver->bMenu||Driver->bInventory||Driver->SecurityMode)return;
 if((FMath::Abs(Speed)>5||Chauffeur||AutoDriving||Boarding)&&!(IsSolarRV74()&&SelfDriving74&&!Chauffeur&&!StopRequested)){Driver->Notify(TEXT("PARK OR ENGAGE AUTOPILOT FIRST"));return;}
 if(PlayerSeat==-2)return;
 CabinEye=FVector(FMath::Clamp(PlayerEye().X,LWTraffic::FrontOffset(Spec())-760.f,LWTraffic::FrontOffset(Spec())+30.f),0,150);
 if(IsSolarRV74()){FVector Local=ActorToCabin74(PlayerEye());Local.Y=0;Local.Z=225;Local.X=FMath::Clamp(Local.X,-805.,30.);CabinEye=CabinToActor74(Local);}
 CabinMove=FVector2D::ZeroVector;CabinVelocity74=FVector::ZeroVector;PlayerSeat=-2;if(!SelfDriving74)Throttle=Steer=0;Driver->TickVehicleSeat();Driver->Notify(TEXT("WASD MOVE / INTERACT WITH SEATS TO SIT"));
}
void ALWVehicle::TickCabinWalk(float Dt){
 if(IsSolarRV74()){
  if(!Driver||PlayerSeat!=-2)return;
  if(Driver->IsUIOpen()||Held49||Airborne49){CabinMove=FVector2D::ZeroVector;CabinVelocity74=FVector::ZeroVector;return;}
  // Integrate exclusively in cabin coordinates. The actor receives the vehicle's
  // final transform after its tick, so road motion is never added to walking twice.
  const FVector Intent=FRotator(0,Driver->SeatYaw,0).RotateVector(FVector(CabinMove.GetClampedToMaxSize(1),0))*165;
  const int Steps=FMath::Clamp(FMath::CeilToInt(Dt*90),1,12);const float Step=FMath::Clamp(Dt,0.f,.15f)/Steps;
  FVector Eye=ActorToCabin74(CabinEye);
  FCollisionQueryParams Q(SCENE_QUERY_STAT(ElectricCabin74),false,Driver);Q.AddIgnoredComponent(Chassis.Get());
  for(const auto& N:Passengers)if(N)Q.AddIgnoredActor(N);if(Chauffeur)Q.AddIgnoredActor(Chauffeur);
  for(int I=0;I<Steps;I++){
   CabinVelocity74=FMath::VInterpConstantTo(CabinVelocity74,Intent,Step,900);
   FVector Next=Eye+CabinVelocity74*Step;Next.X=FMath::Clamp(Next.X,-813.,75.);Next.Z=225;
   const bool Door=CamperDoorOpen&&!SelfDriving74&&FMath::Abs(Speed)<5&&Next.X>-10&&Next.X<80;
   const float Width=85+((Next.X> -365&&Next.X< -145)?SlideAlpha66*80:0);
   Next.Y=FMath::Clamp(Next.Y,-double(Width),Door?220.:double(Width));
   if(Door&&Next.Y>185){CabinEye=CabinToActor74(Next);Exit();return;}
   const FTransform X=Root->GetComponentTransform();const FVector Start=X.TransformPosition(Eye-FVector(0,0,50)),End=X.TransformPosition(Next-FVector(0,0,50));FHitResult Hit;
   if(!GetWorld()->SweepSingleByChannel(Hit,Start,End,GetActorQuat(),ECC_Visibility,FCollisionShape::MakeCapsule(22,65),Q))Eye=Next;
   else if(!Hit.bStartPenetrating){
    const FVector Advance=(End-Start)*FMath::Max(0.f,Hit.Time-.01f);const FVector Contact=Start+Advance;
    const FVector Slide=FVector::VectorPlaneProject(End-Contact,Hit.Normal);FHitResult Side;
    const bool Block=GetWorld()->SweepSingleByChannel(Side,Contact,Contact+Slide,GetActorQuat(),ECC_Visibility,FCollisionShape::MakeCapsule(22,65),Q);
    Eye=X.InverseTransformPosition(Contact+Slide*(Block?FMath::Max(0.f,Side.Time-.01f):1.f))+FVector(0,0,50);
   }else CabinVelocity74=FVector::ZeroVector;
  }
  Eye.Z=225;CabinEye=CabinToActor74(Eye);return;
 }
 if(!Driver||PlayerSeat!=-2||Driver->bMenu||Driver->bInventory||Driver->bMap||Driver->RPGPanel||Driver->SecurityMode)return;
 FVector Delta=FRotator(0,Driver->SeatYaw,0).RotateVector(FVector(CabinMove.GetSafeNormal()*FMath::Min(1.,CabinMove.Size()),0))*150*Dt;
 const float F=LWTraffic::FrontOffset(Spec());FVector Next=CabinEye+Delta;Next.X=FMath::Clamp(Next.X,F-813.,F+75.);const bool Doorway=CamperDoorOpen&&Next.X>F-10&&Next.X<F+80;const bool SlideRoom=Next.X>F-365&&Next.X<F-145;const double Width=85.+(SlideRoom?SlideAlpha66*80:0);Next.Y=FMath::Clamp(Next.Y,-Width,Doorway?220.:Width);Next.Z=150;
 if(Doorway&&Next.Y>185){Exit();return;}
 FCollisionQueryParams Q(NAME_None,false,Driver);Q.AddIgnoredComponent(Chassis.Get());FHitResult Hit;
 FVector Start=GetActorTransform().TransformPosition(CabinEye-FVector(0,0,50)),End=GetActorTransform().TransformPosition(Next-FVector(0,0,50));
 if(!GetWorld()->SweepSingleByChannel(Hit,Start,End,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeCapsule(22,65),Q))CabinEye=Next;
 else{FVector Slide=FVector::VectorPlaneProject(End-Start,Hit.Normal);if(!GetWorld()->SweepSingleByChannel(Hit,Start,Start+Slide,FQuat::Identity,ECC_Visibility,FCollisionShape::MakeCapsule(22,65),Q))CabinEye=GetActorTransform().InverseTransformPosition(Start+Slide)+FVector(0,0,50);}
}
