#include "LWAircraft84.h"
#include "Engine/World.h"
#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWWorld.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/PlayerController.h"
#include "InputCoreTypes.h"
#include "Misc/Crc.h"
bool ALWCharacter::HasKey(FGuid VIN)const{return VIN.IsValid()&&Inventory.ContainsByPredicate([&](const auto& I){return I.Definition==TEXT("car_key")&&I.Count>0&&I.VehicleId==VIN;});}
bool ALWCharacter::IsLocked(ALWWorldObject* O)const{if(!IsValid(O))return false;if(auto* C=Cast<ALWVehicle>(O)){auto* R=C->Record();return R&&!R->Unlocked&&R->LockTier>0;}const auto* R=World->Containers.Find(O->RecordId);return R&&R->LockTier>0&&!R->Unlocked;}
bool ALWCharacter::StartLockpick(ALWWorldObject* O){
 if(!CanAct()||!IsLocked(O)||FVector::Dist(GetActorLocation(),O->GetActorLocation())>550)return false;
 auto* Pick=Inventory.FindByPredicate([](const auto& I){return I.Definition==TEXT("lockpick")&&I.Slot.IsNone()&&I.Count>0;});if(!Pick){Notify(TEXT("LOCKPICKS REQUIRED // FIND SUPPLIES OR VISIT A MERCHANT"));return false;}
 const FGuid Id=Pick->Id;float Durability=Pick->Durability;ClosePanels();CancelReload();SecurityPick=Id;SecurityTarget=O;SecurityMode=1;PickHealth=Durability;PickAngle=LockTurn=0;
 int Attempts=0;if(auto* C=Cast<ALWVehicle>(O)){SecurityTier=C->Record()->LockTier;Attempts=C->Record()->Attempts;}else{const auto& R=World->Containers.FindChecked(O->RecordId);SecurityTier=R.LockTier;Attempts=R.LockAttempts;}
 FRandomStream RNG(int32(FCrc::StrCrc32(*O->RecordId.ToString())^uint32(World->Seed)^uint32(Attempts*7919)));LockSecret=RNG.FRandRange(-78.f,78.f);SetMenuInput(true);return true;
}
bool ALWCharacter::StartHotwire(){if(!Vehicle||Vehicle->EngineOn||FMath::Abs(Vehicle->Speed)>5||Health<=0||bMenu)return false;ClosePanels();SecurityMode=2;SecurityTarget=Vehicle;WireStage=0;WireHeat=WireProgress=WireClock=0;WireOrder={0,1,2,3,4,5};FRandomStream R(int32(FCrc::StrCrc32(*Vehicle->Record()->VIN.ToString())));for(int I=5;I>0;I--)WireOrder.Swap(I,R.RandRange(0,I));SetMenuInput(true);return true;}
void ALWCharacter::CancelSecurity(){if(SecurityMode==1)if(auto* I=FindItem(SecurityPick))I->Durability=FMath::Clamp(FMath::FloorToInt(PickHealth),1,100);bool Had=SecurityMode!=0;SecurityMode=0;SecurityTarget=nullptr;SecurityPick.Invalidate();LockTurn=0;if(Had)RequestSave40();}
void ALWCharacter::FinishSecurity(){auto* O=SecurityTarget.Get();if(!O)return;
 if(SecurityMode==1){if(auto* C=Cast<ALWVehicle>(O)){C->Record()->Unlocked=true;}else if(auto* R=World->Containers.Find(O->RecordId))R->Unlocked=true;GainXP(15*SecurityTier);World->Sound(TEXT("LockOpen"),O->GetActorLocation());ClosePanels();Notify(TEXT("LOCK OPEN // ACCESS GRANTED"));if(!Cast<ALWVehicle>(O))OpenContainer(O);}
 else if(SecurityMode==2&&Vehicle==O){Vehicle->Record()->Hotwired=true;ClosePanels();Vehicle->Ignition();GainXP(50);Notify(TEXT("IGNITION BYPASSED // ENGINE READY"));}RequestSave40();
}
void ALWCharacter::ChooseWire(int32 I){if(SecurityMode!=2||!Vehicle||WireStage>=3||!WireOrder.IsValidIndex(WireStage)||I<0||I>5)return;
 if(I==WireOrder[WireStage]){WireStage++;World->Sound(TEXT("Click"),GetActorLocation());}else{WireStage=0;WireProgress=0;WireHeat=FMath::Min(100.f,WireHeat+35);World->Sound(TEXT("WireSpark"),GetActorLocation());World->Noise(GetActorLocation(),6000);Notify(TEXT("SHORT CIRCUIT // CONNECTIONS RESET"));}
}
void ALWCharacter::TickSecurity(float Dt){if(!SecurityMode)return;if(!IsValid(SecurityTarget)||Health<=0||FVector::Dist(GetActorLocation(),SecurityTarget->GetActorLocation())>550){ClosePanels();return;}
 auto* PC=Cast<APlayerController>(Controller);if(!PC)return;Dt=FMath::Min(Dt,.05f);bool Torque=PC->IsInputKeyDown(EKeys::LeftMouseButton)||PC->IsInputKeyDown(EKeys::SpaceBar);
 if(SecurityMode==1){
  float Axis=(PC->IsInputKeyDown(EKeys::D)?1.f:0)-(PC->IsInputKeyDown(EKeys::A)?1.f:0);PickAngle=FMath::Clamp(PickAngle+Axis*Dt*65,-90.f,90.f);
  auto* Pick=FindItem(SecurityPick);if(!Pick||Pick->Definition!=TEXT("lockpick")||Pick->Count<1){ClosePanels();return;}
  float Limit=LWSecurity::TurnLimit(PickAngle,LockSecret,SecurityTier);LockTurn=FMath::FInterpConstantTo(LockTurn,Torque?Limit:0,Dt,110);
  if(Torque&&Limit<89&&LockTurn>=Limit-1){PickHealth-=Dt*(17+SecurityTier*6);if(PickHealth<=0){Pick->Count--;if(Pick->Count<=0)Inventory.RemoveAll([&](const auto& I){return I.Id==SecurityPick;});else Pick->Durability=100;
   if(auto* C=Cast<ALWVehicle>(SecurityTarget))C->Record()->Attempts++;else World->Containers.FindChecked(SecurityTarget->RecordId).LockAttempts++;
   World->Sound(TEXT("PickBreak"),GetActorLocation());SecurityPick.Invalidate();ClosePanels();Notify(TEXT("PICK BROKE // TRY ANOTHER"));RequestSave40();return;}}
  if(LockTurn>=89.9f)FinishSecurity();
 }else{
  WireClock+=Dt;if(WireStage==3){float Phase=FMath::Fmod(WireClock,2.4f);bool Good=Phase>=.7f&&Phase<=1.45f;
   if(Torque){WireProgress=FMath::Max(0.f,WireProgress+Dt*(Good?1.f:-.35f));WireHeat+=Dt*(Good?-8.f:45.f);}else WireHeat-=Dt*12;
   if(WireProgress>=1.3f){FinishSecurity();return;}}
  WireHeat=FMath::Clamp(WireHeat,0.f,100.f);if(WireHeat>=100){WireStage=0;WireProgress=0;WireHeat=25;World->Sound(TEXT("WireSpark"),GetActorLocation());World->Noise(GetActorLocation(),7500);Notify(TEXT("STARTER OVERHEATED // RECONNECT THE HARNESS"));}
 }
}
void ALWCharacter::TickVehicleSeat(){if(!Vehicle)return;
 if(Vehicle->PlayerSeat==-1&&!IsUIOpen())if(auto* PC=Cast<APlayerController>(Controller);PC&&(PC->IsInputKeyDown(EKeys::LeftAlt)||PC->IsInputKeyDown(EKeys::RightAlt))){
 Vehicle->AdjustSeat50(float(PC->IsInputKeyDown(EKeys::Right))-float(PC->IsInputKeyDown(EKeys::Left)),float(PC->IsInputKeyDown(EKeys::Up))-float(PC->IsInputKeyDown(EKeys::Down)),GetWorld()->GetDeltaSeconds());}
 FVector Eye=Vehicle->GetActorTransform().TransformPosition(Vehicle->PlayerEye()+(Vehicle->PlayerSeat==-1?Vehicle->SeatOffset50:FVector::ZeroVector));SetActorLocation(Eye-FVector(0,0,Camera->GetRelativeLocation().Z),false,nullptr,ETeleportType::TeleportPhysics);if(Vehicle->IsAircraft84())Controller->SetControlRotation((Vehicle->GetActorQuat()*FRotator(SeatPitch,SeatYaw,0).Quaternion()).Rotator());else Controller->SetControlRotation(FRotator((Vehicle->IsSolarRV74()?Vehicle->CabinPitch74:Vehicle->GetActorRotation().Pitch)+SeatPitch,Vehicle->GetActorRotation().Yaw+SeatYaw,0));WeaponRoot->SetVisibility(false,true);bIndoors=Vehicle->Spec().Seats>1;}
void ALWCharacter::VehicleGlove(){if(auto* RV=Vehicle?Vehicle.Get():Cast<ALWVehicle>(ObjectFocus)){if(RV->IsCamper74()){RV->UseCamper(this,RV->CamperFocus(this));return;}}if(Vehicle){Vehicle->Control(TEXT("glove"));return;}auto* C=Cast<ALWVehicle>(ObjectFocus);if(!C||!CanAct())return;if(!C->Record()->Unlocked){if(HasKey(C->Record()->VIN)){C->Record()->Unlocked=true;RequestSave40();}else{StartLockpick(C);return;}}if(!C->CargoObject){C->CargoObject=World->SpawnObject(ELWObjectKind::Container,C->RecordId,C->GetActorLocation());C->CargoObject->SetActorHiddenInGame(true);C->CargoObject->SetActorEnableCollision(false);C->CargoObject->SetActorTickEnabled(false);}C->CargoObject->SetActorLocation(C->GetActorLocation());OpenContainer(C->CargoObject);}
void ALWCharacter::VehicleLights(){if(Vehicle)Vehicle->Control(TEXT("lights"));}void ALWCharacter::VehicleRadio(){if(Vehicle)Vehicle->Control(TEXT("radio"));}void ALWCharacter::VehicleWipers(){if(Vehicle)Vehicle->Control(TEXT("wipers"));}void ALWCharacter::VehicleLeft(){if(Vehicle)Vehicle->Control(TEXT("left"));}void ALWCharacter::VehicleRight(){if(Vehicle)Vehicle->Control(TEXT("right"));}
