#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
FName ALWVehicle::CamperFocus(const ALWCharacter* P)const{
 if(!P||!P->Camera)return NAME_None;
 FHitResult H;FCollisionQueryParams Q(NAME_None,false,P);Q.AddIgnoredComponent(Chassis.Get());
 const FVector A=P->Camera->GetComponentLocation();
 if(GetWorld()->LineTraceSingleByChannel(H,A,A+P->Camera->GetForwardVector()*330,ECC_Visibility,Q)&&H.GetActor()==this&&H.GetComponent()&&!H.GetComponent()->ComponentTags.IsEmpty())return H.GetComponent()->ComponentTags[0];
 return NAME_None;
}
void ALWVehicle::OpenCamperStorage(ALWCharacter* P,FName Action){
 if(!P||(FMath::Abs(Speed)>5&&!(IsSolarRV74()&&SelfDriving74&&P==Driver&&!Action.ToString().StartsWith(TEXT("cargo_")))))return;VehicleSound42(TEXT("CarCargoOpen"));
 // The first cargo hatch preserves the former shared cargo inventory.
 const FName Key=Action==TEXT("cargo_-1_0")?RecordId:FName(*(RecordId.ToString()+TEXT("_")+Action.ToString()));
 if(!World->Containers.Contains(Key)){FLWContainerRecord R;R.Id=Key;R.Context=TEXT("bunker");R.Width=8;R.Height=Action.ToString().StartsWith(TEXT("cargo_"))?10:6;R.Position=GetActorLocation();World->Containers.Add(Key,R);}
 if(CargoObject){CargoObject->Destroy();CargoObject=nullptr;}
 CargoObject=World->SpawnObject(ELWObjectKind::Container,Key,P->GetActorLocation());CargoObject->SetActorHiddenInGame(true);CargoObject->SetActorEnableCollision(false);CargoObject->SetActorTickEnabled(false);P->OpenContainer(CargoObject);
}
void ALWVehicle::UseCamper(ALWCharacter* P,FName Action){
 if(!P||Action.IsNone()||!Record()||Record()->Exploded||P->bMenu||P->SecurityMode)return;
 if(!Record()->Unlocked){if(P->HasKey(Record()->VIN)){Record()->Unlocked=true;P->RequestSave40();}else{P->StartLockpick(this);return;}}
 if(UseElectric74(P,Action)||UseLuxury66(P,Action))return;
 if(Action==TEXT("entry")){
  if(FMath::Abs(Speed)>5||AutoDriving||Boarding){P->Notify(TEXT("PARK FIRST"));return;}
  if(CamperDoorOpen&&Driver&&PlayerSeat==-2&&CabinEye.Y>85){P->Notify(TEXT("CLEAR THE DOORWAY"));return;}
  CamperDoorOpen=!CamperDoorOpen;VehicleSound42(CamperDoorOpen?TEXT("CarDoorOpen"):TEXT("CarDoorClose"));return;
 }
 if(Action.ToString().StartsWith(TEXT("cargo_"))||Action.ToString().EndsWith(TEXT("_storage"))){OpenCamperStorage(P,Action);return;}
 if(Action.ToString().StartsWith(TEXT("seat_"))){int Seat=FCString::Atoi(*Action.ToString().Mid(5));if(P->Vehicle==this)ChangeSeat(Seat);else P->Notify(TEXT("ENTER THROUGH THE DOOR"));return;}
 if(P->Vehicle==this)Control(Action);
}
void ALWVehicle::TickCamperEntry(float Dt){
 if(!IsCamper74()||!CamperDoor)return;
 if(FMath::Abs(Speed)>5)CamperDoorOpen=false;
 CamperDoorAngle=FMath::FInterpConstantTo(CamperDoorAngle,CamperDoorOpen?-100.f:0.f,Dt,120);
 CamperDoor->SetRelativeRotation(FRotator(0,CamperDoorAngle,0));
 if(Driver||!CamperDoorOpen||CamperDoorAngle>-80||FMath::Abs(Speed)>5)return;
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(!P||P->Vehicle||!P->CanAct())return;
 FVector Local=GetActorTransform().InverseTransformPosition(P->GetActorLocation());if(IsSolarRV74())Local=ActorToCabin74(Local)+FVector(LWTraffic::FrontOffset(Spec()),0,-75);
 FVector Motion=GetActorTransform().InverseTransformVectorNoScale(P->GetVelocity());
 if(FMath::Abs(Local.X-(LWTraffic::FrontOffset(Spec())+35))<32&&Local.Y>110&&Local.Y<210&&FMath::Abs(Local.Z-50)<160&&Motion.Y<-5)Enter(P,-2);
}
