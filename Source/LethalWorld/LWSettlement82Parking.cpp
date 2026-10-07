#include "LWResident.h"
#include "LWSettlement82.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWVehicle.h"
#include "LWGarage45.h"
#include "EngineUtils.h"
#include "Engine/World.h"
FVector ALWSettlement82::Bay(const FLWConstruction82& Lot,int Index,int Size)const{return Lot.Transform.TransformPosition({-1600+(Index%5)*800.f+(Size-1)*400.f,Index<5?-800.f:800.f,100});}
bool ALWSettlement82::Park(ALWVehicle* Car,FName LotId){
 auto* C=State();const auto* Lot=C?C->Pieces.FindByPredicate([&](const auto& P){return P.Id==LotId&&P.Catalog==TEXT("parking");}):nullptr;
 if(!Lot||!IsValid(Car)||Car->IsAircraft84()||!Car->Record())return false;auto* R=Car->Record();
 if(Car->Driver||Car->ConvoyOwner||Car->EngineOn||FMath::Abs(Car->Speed)>5||Car->Held49||Car->Airborne49||R->Exploded||R->Health<=0||Car->Passengers.ContainsByPredicate([](const auto& N){return IsValid(N);})||Car->Chauffeur){Message=TEXT("Park, switch off the engine, and have everyone step out first.");return false;}
 FVector Local=Lot->Transform.InverseTransformPosition(Car->GetActorLocation());if(FMath::Abs(Local.X)>2000||FMath::Abs(Local.Y)>1300||FMath::Abs(Local.Z)>260||FVector::Dist2D(Player->GetActorLocation(),Car->GetActorLocation())>6500){Message=TEXT("Drive the vehicle onto this lot first.");return false;}
 const int Size=LWGarage45::Bays(R->Model);bool Used[10]={};for(const auto& V:World->Vehicles)if(V.Key!=Car->RecordId&&V.Value.HomeParking82==LotId&&V.Value.ParkingBay82>=0)for(int K=0;K<LWGarage45::Bays(V.Value.Model);K++)if(V.Value.ParkingBay82+K<10)Used[V.Value.ParkingBay82+K]=true;
 int Free=-1;for(int I=0;I<10;I++){if(I%5+Size>5)continue;bool Fits=true;for(int J=0;J<Size;J++)Fits&=!Used[I+J];if(Fits){Free=I;break;}}if(Free<0){Message=TEXT("No suitable bays free. RVs and buses require three adjoining bays.");return false;}
 // Ownership never depends on teleporting the car: this action insures the parked vehicle in place.
 R->Owned45=true;R->Stored45=false;R->GarageBay45=-1;R->HomeSettlement82=C->Id;R->HomeParking82=LotId;R->ParkingBay82=Free;R->Unlocked=R->Hotwired=true;Car->SyncRecord();Player->RequestSave40();Message=TEXT("Vehicle insured. It stays marked on the map and returns here if destroyed.");return true;
}
bool ALWSettlement82::ReleaseCar(FName Id){auto* C=State();auto* R=World->Vehicles.Find(Id);if(!C||!R||R->HomeSettlement82!=C->Id)return false;if(R->Exploded){Message=TEXT("Wait for the replacement vehicle before releasing it.");return false;}R->HomeSettlement82=R->HomeParking82=NAME_None;R->ParkingBay82=-1;R->Stored45=false;R->Owned45=false;Message=TEXT("Vehicle released. Its cargo remains, but this lot no longer insures it.");Player->RequestSave40();return true;}
void ALWSettlement82::TickParking(float Dt){
 TMap<FName,ALWVehicle*> Cars;for(TActorIterator<ALWVehicle> I(GetWorld());I;++I)Cars.Add(I->RecordId,*I);
 for(auto& Pair:World->Vehicles){auto& R=Pair.Value;if(!R.Owned45||R.HomeParking82.IsNone())continue;auto* C=Player->RPG.Claims82.Find(R.HomeSettlement82);if(!C)continue;const auto* Lot=C->Pieces.FindByPredicate([&](const auto& P){return P.Id==R.HomeParking82;});if(!Lot)continue;
  if(!R.Exploded)continue;if(R.Replacement45<=0)R.Replacement45=30;R.Replacement45=FMath::Max(.01f,R.Replacement45-Dt);if(R.Replacement45>.01f)continue;
  const int Size=LWGarage45::Bays(R.Model);FVector At=Bay(*Lot,R.ParkingBay82,Size);FRotator Rot=Lot->Transform.Rotator();Rot.Yaw+=Size==1?(R.ParkingBay82<5?90:-90):0;
  FCollisionQueryParams Q(NAME_None,false,this);if(auto* Old=Cars.FindRef(Pair.Key))Q.AddIgnoredActor(Old);if(auto* Surface=Actors.FindRef(Lot->Id).Get())Q.AddIgnoredActor(Surface);
  // A replacement waits instead of crushing a player, resident, or another parked car.
  bool Blocked=false;for(const auto& Other:Cars)if(Other.Key!=Pair.Key&&IsValid(Other.Value)&&LWBuilding82::Overlap(FTransform(Rot,At),{Size==1?260.:1100.,140,150},Other.Value->GetActorTransform(),{Other.Value->Spec().HalfLength,Other.Value->Spec().HalfWidth,150},0)){Blocked=true;break;}
  if(FVector::Dist2D(Player->GetActorLocation(),At)<(Size==1?350:1200))Blocked=true;
  const auto& Spec=LWTraffic::Get(R.Model);
  Blocked|=GetWorld()->OverlapBlockingTestByChannel(At+FVector(0,0,Spec.Height*.35f),Rot.Quaternion(),ECC_Pawn,FCollisionShape::MakeBox(FVector(Spec.HalfLength+20,Spec.HalfWidth+20,Spec.Height*.45f)),Q);
  if(Blocked)continue;
  if(auto* Old=Cars.FindRef(Pair.Key)){if(Old->Driver)Old->Exit(true);Old->Destroy();}
  R.Health=200;R.FireRemaining=0;R.Exploded=false;R.Dents.Empty();R.Replacement45=0;R.Position=At;R.Rotation=Rot;R.Stored45=false;R.Slides66=R.Awning66=false;
  if(Loaded.Contains(C->Id))World->SpawnObject(ELWObjectKind::Car,Pair.Key,R.Position,R.Rotation);Player->Notify(TEXT("Your replacement vehicle is ready at ")+C->Name+TEXT(". Cargo secured."),6);Player->RequestSave40();
 }
}
