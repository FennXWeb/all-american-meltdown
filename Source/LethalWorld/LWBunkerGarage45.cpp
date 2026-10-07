#include "LWElectric74.h"
#include "LWBunker45.h"
#include "LWGarage45.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "LWResident.h"
#include "LWVehicle.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
void ALWBunker45::GoFloor(int I){
 if(!Lift||MovingLift||BuildQueue.Num())return;
 if(I!=0&&!(I>0&&I<=Player->Bunker45.BedroomFloors)&&!(I==10&&Player->Bunker45.Utilities.Num())&&!(I==11&&Player->Bunker45.Garage))return;
 Destination=I;MovingLift=I!=Floor;
 if(MovingLift){LiftGate->SetVisibility(true);LiftGate->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);World->Sound(TEXT("CarSwitch"),Lift->GetComponentLocation());}Close();
}
void ALWBunker45::TickLift(float Dt){
 if(!Lift)return;float Old=LiftZ;LiftZ=FMath::FInterpConstantTo(LiftZ,FloorZ(Destination)+20,Dt,240);float Delta=LiftZ-Old;
 if(MovingLift){
  for(TActorIterator<APawn> It(GetWorld());It;++It){FVector P=It->GetActorLocation();if(FMath::Abs(P.X-(Center().X+1700))<153&&FMath::Abs(P.Y-Center().Y)<143&&P.Z>Old&&P.Z<Old+370){if(auto* C=Cast<ACharacter>(*It))C->GetCharacterMovement()->StopMovementImmediately();It->SetActorLocation(P+FVector(0,0,Delta),false,nullptr,ETeleportType::TeleportPhysics);}}
  Lift->SetWorldLocation(FVector(Center().X+1700,Center().Y,LiftZ));LiftGate->SetWorldLocation(FVector(Center().X+1535,Center().Y,LiftZ+135));
  if(CabButton)CabButton->SetActorLocation(FVector(Center().X+1800,Center().Y-120,LiftZ+85));
  if(FMath::Abs(LiftZ-(FloorZ(Destination)+20))<.1f){Floor=Destination;MovingLift=false;LiftGate->SetVisibility(false);LiftGate->SetCollisionEnabled(ECollisionEnabled::NoCollision);World->Sound(TEXT("LockOpen"),Lift->GetComponentLocation());}
 }
 for(int I=0;I<Gates.Num();++I){bool Closed=MovingLift||Floor!=GateFloors[I];Gates[I]->SetVisibility(Closed);Gates[I]->SetCollisionEnabled(Closed?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);}
}
bool ALWBunker45::StoreVehicle(ALWVehicle* C){
 if(!Player->Bunker45.Garage||BuildQueue.Num()||TransferCar||!IsValid(C)||C->IsAircraft84()||!C->Record()||C->Record()->Exploded||C->Driver||C->EngineOn||FMath::Abs(C->Speed)>1){Message=TEXT("Park on the platform, stop the engine, and exit.");return false;}
 const FVector D=C->GetActorLocation()-Surface;
 if(FMath::Abs(D.X)+FMath::Abs(C->GetActorForwardVector().X)*C->Spec().HalfLength+FMath::Abs(C->GetActorRightVector().X)*C->Spec().HalfWidth>760||FMath::Abs(D.Y)+FMath::Abs(C->GetActorForwardVector().Y)*C->Spec().HalfLength+FMath::Abs(C->GetActorRightVector().Y)*C->Spec().HalfWidth>360||FMath::Abs(D.Z)>250){Message=TEXT("Center the vehicle on the platform.");return false;}
 int Bay=C->Record()->Owned45&&C->Record()->GarageBay45>=0?C->Record()->GarageBay45:LWGarage45::FreeBay(World->Vehicles,C->Record()->Model);
 if(Bay<0){Message=TEXT("No suitable bays. RVs and buses need three adjacent bays.");return false;}
 C->UnloadPassengers();C->AutoDriving=C->Boarding=false;C->ConvoyLeader=nullptr;C->ConvoyOwner=nullptr;C->Record()->Owned45=true;C->Record()->GarageBay45=Bay;C->Record()->Unlocked=C->Record()->Hotwired=true;
 TransferCar=C;TransferClock=2;C->SetActorTickEnabled(false);C->SetActorEnableCollision(false);C->Record()->HomeSettlement82=C->Record()->HomeParking82=NAME_None;C->Record()->ParkingBay82=-1;C->Record()->Stored45=true;C->Record()->Position=LWGarage45::BayPosition(Bay,LWGarage45::Bays(C->Record()->Model));C->Record()->Rotation=FRotator(0,LWGarage45::Bays(C->Record()->Model)>1?0:Bay<5?90:-90,0);Player->RequestSave40();Message=TEXT("Vehicle lift descending...");return true;
}
bool ALWBunker45::Retrieve(FName Id){
 auto* R=World->Vehicles.Find(Id);if(!R||!R->Owned45||!R->Stored45||R->Exploded||TransferCar)return false;
 for(TActorIterator<ALWVehicle> C(GetWorld());C;++C)if(C->RecordId!=Id&&FVector::Dist2D(C->GetActorLocation(),Surface)<1100&&FMath::Abs(C->GetActorLocation().Z-Surface.Z)<500){Message=TEXT("Clear the surface platform first.");return false;}
 ALWVehicle* Car=nullptr;for(TActorIterator<ALWVehicle> C(GetWorld());C;++C)if(C->RecordId==Id){Car=*C;break;}
 if(Car&&Car->Driver){Message=TEXT("Exit the vehicle before using the lift.");return false;}
 const auto& Spec=LWTraffic::Get(R->Model);FCollisionQueryParams Clearance(NAME_None,false,Car);Clearance.AddIgnoredActor(Player);if(SurfacePlatform)Clearance.AddIgnoredActor(SurfacePlatform->GetOwner());
 if(GetWorld()->OverlapBlockingTestByChannel(Surface+FVector(0,0,15+Spec.Height*.5f),FQuat::Identity,ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(Spec.HalfLength+20,Spec.HalfWidth+20,Spec.Height*.5f-2)),Clearance)){Message=TEXT("The surface platform is obstructed.");return false;}
 R->Position=Surface+FVector(0,0,90);R->Rotation=FRotator::ZeroRotator;R->Stored45=false;
 if(Car){Car->SetActorLocationAndRotation(R->Position,R->Rotation,false,nullptr,ETeleportType::TeleportPhysics);Car->SyncRecord();}else World->SpawnObject(ELWObjectKind::Car,Id,R->Position,R->Rotation);
 Close();Player->LeaveSafehouse();Player->SetActorLocation(Surface+FVector(0,650,110),false,nullptr,ETeleportType::TeleportPhysics);Player->RequestSave40();Message=TEXT("Vehicle ready on the surface platform.");return true;
}
void ALWBunker45::TickGarage(float Dt){
 if(TransferCar){
  TransferClock-=Dt;FVector At=TransferCar->GetActorLocation();At.Z-=Dt*90;TransferCar->SetActorLocation(At);
  if(SurfacePlatform)SurfacePlatform->SetWorldLocation(Surface-FVector(0,0,(2-FMath::Max(0.f,TransferClock))*90));
  if(TransferClock<=0){
   auto* R=TransferCar->Record();if(R){R->Stored45=true;R->Position=LWGarage45::BayPosition(R->GarageBay45,LWGarage45::Bays(R->Model));R->Rotation=FRotator(0,LWGarage45::Bays(R->Model)>1?0:R->GarageBay45<5?90:-90,0);TransferCar->SetActorLocationAndRotation(R->Position,R->Rotation,false,nullptr,ETeleportType::TeleportPhysics);TransferCar->SpawnPlacementPending=false;TransferCar->SyncRecord();}
   TransferCar->SetActorEnableCollision(true);TransferCar->SetActorTickEnabled(true);TransferCar=nullptr;if(SurfacePlatform)SurfacePlatform->SetWorldLocation(Surface);Player->RequestSave40();Message=TEXT("Vehicle secured. Ownership registered.");
  }
 }
 Poll+=Dt;if(Poll<1||BuildQueue.Num())return;float Elapsed=Poll;Poll=0;
 for(auto& Pair:World->Vehicles){
  auto& R=Pair.Value;if(!R.Owned45||LWTraffic::IsAircraft(R.Model)||!R.HomeParking82.IsNone())continue;
  if(R.Exploded){
   if(!Player->Bunker45.Garage||R.GarageBay45<0)continue;
   if(R.Replacement45<=0)R.Replacement45=30;R.Replacement45-=Elapsed;if(R.Replacement45>0)continue;
   for(TActorIterator<ALWVehicle> C(GetWorld());C;++C)if(C->RecordId==Pair.Key){if(C->Driver)C->Exit(true);C->Destroy();}
   R.Health=200;R.FireRemaining=0;R.Exploded=false;R.Dents.Empty();R.Stored45=true;R.Replacement45=0;R.Position=LWGarage45::BayPosition(R.GarageBay45,LWGarage45::Bays(R.Model));R.Rotation=FRotator(0,LWGarage45::Bays(R.Model)>1?0:R.GarageBay45<5?90:-90,0);Player->Notify(TEXT("Vehicle restored in your garage. Cargo secured."),6);Player->RequestSave40();
  }
  if(R.Stored45){bool Found=false;for(TActorIterator<ALWVehicle> C(GetWorld());C;++C)if(C->RecordId==Pair.Key){Found=true;break;}if(!Found)World->SpawnObject(ELWObjectKind::Car,Pair.Key,R.Position,R.Rotation);}
 }
}
bool ALWBunker45::Install(FName Id){
 auto* R=World->Vehicles.Find(GarageCar);const auto* M=LWGarage45::Mods().FindByPredicate([&](const auto& X){return X.Id==Id;});
 if(!R||!R->Owned45||!R->Stored45||R->Exploded||!M||!LWGarage45::Compatible(*M,R->Model)||R->Mods45.Contains(Id))return false;
 if(Player->Money<M->Cost){Message=TEXT("Not enough credits.");return false;}
 for(const auto& Old:LWGarage45::Mods())if(Old.Slot==M->Slot)R->Mods45.Remove(Old.Id);R->Mods45.Add(Id);Player->Money-=M->Cost;
 for(TActorIterator<ALWVehicle> C(GetWorld());C;++C)if(C->RecordId==GarageCar)C->ApplyGarage45();Player->RequestSave40();Message=M->Name+TEXT(" installed");return true;
}
bool ALWBunker45::Paint(int Index){
 auto* R=World->Vehicles.Find(GarageCar);if(!R||!R->Stored45||!R->Owned45||Index<0||Index>10||Player->Money<250)return false;
 Player->Money-=250;R->Paint45=Index;for(TActorIterator<ALWVehicle> C(GetWorld());C;++C)if(C->RecordId==GarageCar)C->ApplyGarage45();Player->RequestSave40();Message=TEXT("Paint applied");return true;
}
bool ALWBunker45::Repair(){
 auto* R=World->Vehicles.Find(GarageCar);if(!R||!R->Owned45||!R->Stored45||R->Exploded)return false;
 int Cost=FMath::CeilToInt((200-R->Health)*3+R->Dents.Num()*5+(LWTraffic::IsElectric(R->Model)?FMath::Max(0.f,LWElectric74::Capacity(R->Model==TEXT("solstice_rv"))-FMath::Max(0.f,R->BatteryKWh74))*.5f:0.f));if(Cost<=0)return true;if(Player->Money<Cost){Message=TEXT("Not enough credits.");return false;}
 Player->Money-=Cost;R->Health=200;if(LWTraffic::IsElectric(R->Model))R->BatteryKWh74=LWElectric74::Capacity(R->Model==TEXT("solstice_rv"));R->FireRemaining=0;R->Dents.Empty();for(TActorIterator<ALWVehicle> C(GetWorld());C;++C)if(C->RecordId==GarageCar){C->bDentsDirty=true;C->RebuildDents();}Player->RequestSave40();Message=TEXT("Repaired");return true;
}

