#include "LWVehicle.h"
#include "LWGarage45.h"
#include "LWFuel.h"
#include "LWCharacter.h"
#include "LWWorld.h"

float ALWVehicle::FuelCapacity()const{
 const FName Model(Spec().Id);const float Base= Model==TEXT("dirtbike")?12.f:Model==TEXT("rv")||Model==TEXT("bus")?120.f:Model==TEXT("boxtruck")?100.f:Model==TEXT("van")||Model==TEXT("pickup")||Model==TEXT("suv")?75.f:55.f;return Base*(1+(Record()?LWGarage45::Stat(Record(),TEXT("tank")):0));
}
float ALWVehicle::FuelLitres()const{
 const auto* R=Record();if(!R)return 0;
 return R->FuelLitres<0?FuelCapacity()*.65f:FMath::IsFinite(R->FuelLitres)?FMath::Clamp(R->FuelLitres,0.f,FuelCapacity()):0.f;
}
bool ALWVehicle::HasFuel()const{return FuelLitres()>0;}
void ALWVehicle::TickFuel(float Dt){
 auto* R=Record();if(!R)return;R->FuelLitres=FuelLitres();
 if(EngineOn){
  // Gameplay scale: idle burns slowly; larger tanks offset heavier road consumption.
  const float Load=FuelCapacity()/55.f;
  R->FuelLitres=FMath::Max(0.f,R->FuelLitres-FMath::Max(0.f,Dt)*Load/FMath::Max(1.f,1+LWGarage45::Stat(R,TEXT("tank")))*FMath::Clamp(1-LWGarage45::Stat(R,TEXT("economy")),.2f,1.f)*(.002f+FMath::Abs(Speed)*.000015f+FMath::Clamp(FMath::Abs(Throttle),0.f,1.f)*.008f));
  if(R->FuelLitres<=0){EngineOn=false;Throttle=0;StopRequested=AutoDriving||Boarding;if(Driver)Driver->Notify(TEXT("OUT OF FUEL // STOP AND REFILL WITH A GAS CAN"));else if(ConvoyOwner)ConvoyOwner->Notify(TEXT("CONVOY VEHICLE OUT OF FUEL"));}
 }
}
bool ALWVehicle::Refuel(ALWCharacter* P){
 if(!IsValid(P)||!P->CanAct()||P->World!=World||SpawnPlacementPending||!Record()||Record()->Exploded||Record()->Health<=0||Record()->FireRemaining>0)return false;
 if((P->Vehicle&&P->Vehicle!=this)||FVector::Dist2D(P->GetActorLocation(),GetActorLocation())>Spec().HalfLength+350)return false;
 if(FMath::Abs(Speed)>5||EngineOn||AutoDriving||Boarding){P->Notify(TEXT("PARK AND SWITCH OFF THE ENGINE TO REFUEL"));return false;}
 const float Before=FuelLitres();
 // Whole-litre transfer conserves fuel without requiring a new inventory instance field.
 int32 Needed=FMath::FloorToInt(FuelCapacity()-Before),Added=0;
 for(auto& I:P->Inventory)if(I.Definition==TEXT("gas_can")&&I.Count==1&&I.Rounds>0&&Needed>0){const int32 Take=FMath::Min(Needed,FMath::Clamp(I.Rounds,0,LWFuel::CanCapacity));I.Rounds-=Take;Needed-=Take;Added+=Take;}
 if(!Added){P->Notify(FuelCapacity()-Before<1?TEXT("FUEL TANK FULL"):TEXT("REQUIRES A FILLED GAS CAN"));return false;}
 VehicleSound42(TEXT("CarFuelCap"),.6f);Record()->FuelLitres=Before+Added;P->SyncAmmoHUD();P->PersistWorldChange();
 P->Notify(FString::Printf(TEXT("REFUELLED +%d L // %.1f / %.0f L"),Added,FuelLitres(),FuelCapacity()));return true;
}
void ALWVehicle::MarkLastDriven(){
 if(!World||!Record())return;
 for(auto& Pair:World->Vehicles)Pair.Value.LastDriven=Pair.Key==RecordId;
 SyncRecord();
}
FName ALWVehicle::LastDrivenId(const ALWWorld* W){
 if(W)for(const auto& Pair:W->Vehicles)if(Pair.Value.LastDriven)return Pair.Key;
 return NAME_None;
}
FGuid ALWVehicle::LastDrivenVIN(const ALWWorld* W){
 const auto* R=W?W->Vehicles.Find(LastDrivenId(W)):nullptr;return R?R->VIN:FGuid();
}
