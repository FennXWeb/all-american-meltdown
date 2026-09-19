#include "LWFuel.h"

// Run with BOTH -LWV17Smoke (isolated save slot) and -LWVehicle30Smoke.
void ALWGameMode::BuildVehicle30Smoke(ALWCharacter& Initial){
 auto Add=[this](FString Name,FLWV2Action Begin){V2->Steps.Add({Name,0,45,MoveTemp(Begin),FLWV2Action(),FLWV2Ready()});};
 struct FState {
  TWeakObjectPtr<ALWChunk> Floor;
  TWeakObjectPtr<ALWVehicle> RV,Sedan,Pending;
  TArray<TWeakObjectPtr<ALWResident>> Crew;
  TArray<FName> Reservations;
 };auto S=MakeShared<FState>();
 auto Spawn=[this](ALWCharacter& P,FName Id,FName Model,FVector At,float Yaw){
  FLWVehicleRecord R;R.VIN=FGuid::NewGuid();R.Model=Model;R.Position=At;R.Rotation=FRotator(0,Yaw,0);
  R.Unlocked=R.Hotwired=true;R.FuelLitres=30;R.FuelLootInitialized=true;P.World->Vehicles.Add(Id,R);
  auto* C=Cast<ALWVehicle>(P.World->SpawnObject(ELWObjectKind::Car,Id,At,R.Rotation));
  if(C)C->SetActorTickEnabled(false);return C;
 };
 Add(TEXT("vehicle30 isolated fixtures"),[this,S](ALWCharacter& P){
  P.NewGame();P.LeaveSafehouse();P.ClosePanels();P.RPG.Crew.Empty();P.bWaypoint=false;P.Health=10000;
  P.World->EnableEncounters=false;LWV2Teleport(P,FVector(18000,20000,5100));P.World->Stream(P.GetActorLocation(),true);
  // Freeze streaming/ambient movement; each case explicitly advances the vehicle under test.
  P.World->SetActorTickEnabled(false);
  for(TActorIterator<ALWVehicle> I(GetWorld());I;++I)I->Destroy();P.World->Vehicles.Empty();
  for(TActorIterator<ALWResident> I(GetWorld());I;++I){I->SetActorTickEnabled(false);I->GetCharacterMovement()->DisableMovement();}
  auto* Floor=GetWorld()->SpawnActor<ALWChunk>();S->Floor=Floor;
  if(!RequireV2(Floor!=nullptr,TEXT("vehicle30 floor spawned")))return;
  Floor->Box(P.World,TEXT("Concrete"),FVector(20000,20000,4990),FVector(40000,40000,20));
 });
 Add(TEXT("RV versus rotated sedan spawn overlap"),[this,S,Spawn](ALWCharacter& P){
  const FVector Origin(20000,20000,5085);
  auto* RV=Spawn(P,TEXT("vehicle30_rv"),TEXT("rv"),Origin,0);S->RV=RV;
  if(!RequireV2(RV&&!RV->SpawnPlacementPending,TEXT("RV publishes in clear space")))return;
  auto* Sedan=Spawn(P,TEXT("vehicle30_sedan"),TEXT("sedan"),RV->GetActorLocation(),67);S->Sedan=Sedan;
  if(!RequireV2(Sedan&&!Sedan->SpawnPlacementPending,TEXT("overlapping rotated sedan finds a clear spawn")))return;
  Check(!Sedan->IsHidden()&&Sedan->GetActorEnableCollision(),TEXT("resolved spawn is visible and colliding"));
  Check(FMath::Abs(Sedan->GetActorRotation().Yaw-67.)<.01,TEXT("relocation preserves sedan rotation"));
  // Independent oracle: project transformed footprint corners, with no production placement helper.
  TArray<FVector> A,B;
  for(int X:{-1,1})for(int Y:{-1,1}){
   A.Add(RV->GetActorTransform().TransformPosition(FVector(X*RV->Spec().HalfLength,Y*RV->Spec().HalfWidth,0)));
   B.Add(Sedan->GetActorTransform().TransformPosition(FVector(X*Sedan->Spec().HalfLength,Y*Sedan->Spec().HalfWidth,0)));
  }
  bool Separated=false;
  for(FVector Axis:{RV->GetActorForwardVector(),RV->GetActorRightVector(),Sedan->GetActorForwardVector(),Sedan->GetActorRightVector()}){
   double AMin=DBL_MAX,AMax=-DBL_MAX,BMin=DBL_MAX,BMax=-DBL_MAX;
   for(FVector V:A){const double D=FVector::DotProduct(V,Axis);AMin=FMath::Min(AMin,D);AMax=FMath::Max(AMax,D);}
   for(FVector V:B){const double D=FVector::DotProduct(V,Axis);BMin=FMath::Min(BMin,D);BMax=FMath::Max(BMax,D);}
   Separated|=AMax<BMin||BMax<AMin;
  }
  Check(Separated,TEXT("full RV and rotated sedan footprints do not overlap"));
  Check(Sedan->Record()->Position.Equals(Sedan->GetActorLocation(),.01),TEXT("relocated position replaces saved reservation"));
  // Subsequent driving tests use a flat, elevated fixture rather than procedural terrain.
  RV->SetActorLocationAndRotation(FVector(20000,20000,5075),FRotator::ZeroRotator);RV->SyncRecord();
  Sedan->SetActorLocationAndRotation(FVector(22000,20000,5075),FRotator::ZeroRotator);Sedan->SyncRecord();
 });
 Add(TEXT("blocked saved reservations and self-reservation retry"),[this,S,Spawn](ALWCharacter& P){
  const FVector Origin(26000,20000,5085);
  auto Reserve=[&](FVector At){FName Id(*FString::Printf(TEXT("vehicle30_block_%d"),S->Reservations.Num()));FLWVehicleRecord R;R.Model=TEXT("rv");R.Position=At;P.World->Vehicles.Add(Id,R);S->Reservations.Add(Id);};
  Reserve(Origin);
  for(int Ring=1;Ring<=12;Ring++)for(int Direction=0;Direction<16;Direction++){
   const float Angle=Direction*2*PI/16;Reserve(Origin+FVector(FMath::Cos(Angle),FMath::Sin(Angle),0)*float(Ring*350));
  }
  auto* C=Spawn(P,TEXT("vehicle30_pending"),TEXT("sedan"),Origin,0);S->Pending=C;
  if(!RequireV2(C!=nullptr,TEXT("crowded vehicle retains a retry actor")))return;
  Check(C->SpawnPlacementPending&&C->IsHidden()&&!C->GetActorEnableCollision(),TEXT("fully reserved search publishes no overlapping geometry"));
  C->SpawnPlacementRetry=0;C->Tick(.05f);
  Check(C->SpawnPlacementPending&&C->IsHidden(),TEXT("retry cannot bypass other saved reservations"));
  for(FName Id:S->Reservations)P.World->Vehicles.Remove(Id);S->Reservations.Empty();
  Check(C->Record()&&C->Record()->Position.Equals(Origin),TEXT("own saved position remains reserved during retry"));
  C->SpawnPlacementRetry=0;C->Tick(.05f);
  Check(!C->SpawnPlacementPending&&!C->IsHidden()&&C->GetActorEnableCollision(),TEXT("own record does not prevent retry from unhiding"));
  Check(C->GetActorLocation().Equals(Origin,.01),TEXT("retry reuses cleared origin without relocation"));
 });
 Add(TEXT("manual fuel burn and empty ignition"),[this,S](ALWCharacter& P){
  auto* C=S->Sedan.Get();if(!RequireV2(C!=nullptr,TEXT("manual fuel vehicle available")))return;
  P.ClosePanels();LWV2Teleport(P,C->GetActorLocation()+FVector(0,-250,30));
  if(!RequireV2(C->Enter(&P,-1),TEXT("player enters manual driver seat")))return;
  C->Record()->FuelLitres=10;C->Ignition();Check(C->EngineOn,TEXT("fuelled hotwired ignition starts"));
  const FVector Before=C->GetActorLocation();const float Fuel=C->FuelLitres();
  for(int I=0;I<20;I++){C->Throttle=1;C->Tick(.05f);}
  Check(C->FuelLitres()<Fuel&&C->FuelLitres()>0,TEXT("manual vehicle tick consumes fuel"));
  Check(FVector::Dist2D(Before,C->GetActorLocation())>1,TEXT("manual throttle moves vehicle on fixture floor"));
  C->Record()->FuelLitres=.00001f;C->Throttle=1;C->Tick(.05f);
  Check(C->FuelLitres()==0&&!C->EngineOn&&C->Throttle==0,TEXT("fuel exhaustion shuts off engine and propulsion"));
  C->Ignition();Check(!C->EngineOn&&C->FuelLitres()==0,TEXT("ignition cannot restart an empty vehicle"));
  C->Speed=0;C->Exit(true);
 });
 Add(TEXT("gas can vehicle-use refill conserves litres"),[this,S](ALWCharacter& P){
  auto* C=S->Sedan.Get();if(!RequireV2(C!=nullptr,TEXT("refill vehicle available")))return;
  P.ClosePanels();P.Inventory.Empty();LWV2Teleport(P,C->GetActorLocation()+FVector(0,-250,30));
  auto Can=LWFuel::MakeGasCan(20);Can.Slot=TEXT("Tool");P.Inventory.Add(Can);
  Check(Can.Id.IsValid()&&LWItems::Def(TEXT("gas_can")).Capacity==20,TEXT("registered gas can has valid identity and capacity"));
  C->Record()->FuelLitres=C->FuelCapacity()-5;C->EngineOn=true;
  Check(!C->Refuel(&P)&&P.Inventory[0].Rounds==20,TEXT("running engine rejects refill without spending fuel"));
  C->EngineOn=false;C->Use(&P);
  Check(C->FuelLitres()==C->FuelCapacity()&&P.Inventory[0].Rounds==15,TEXT("equipped-can use transfers only tank headroom"));
  Check(!C->Refuel(&P)&&P.Inventory[0].Rounds==15,TEXT("full tank rejects refill without spending fuel"));
  C->Record()->FuelLitres=0;C->Use(&P);
  Check(C->FuelLitres()==15&&P.Inventory.Num()==1&&P.Inventory[0].Id==Can.Id&&P.Inventory[0].Rounds==0,TEXT("empty can retains identity and no litres are lost"));
  auto* Pump=P.World->SpawnObject(ELWObjectKind::FuelPump,TEXT("vehicle30_pump"),P.GetActorLocation()+FVector(150,0,-90));
  if(RequireV2(Pump!=nullptr,TEXT("gas refill pump spawned"))){
   Pump->bChanged=true;Pump->Use(&P);Check(P.Inventory[0].Rounds==0,TEXT("burnt pump interaction cannot refill gas can"));
   Pump->bChanged=false;Pump->Use(&P);Check(P.Inventory[0].Rounds==20,TEXT("intact pump interaction refills reusable can"));
   Pump->Destroy();
  }
  P.Inventory.Empty();
 });
 Add(TEXT("last driven VIN and fuel record archive"),[this,S](ALWCharacter& P){
  auto* C=S->Sedan.Get();auto* RV=S->RV.Get();if(!RequireV2(C&&RV,TEXT("persistence vehicles available")))return;
  const FGuid VIN=C->Record()->VIN;
  Check(ALWVehicle::LastDrivenVIN(P.World)==VIN,TEXT("manual driver remains last driven after exit"));
  LWV2Teleport(P,RV->GetActorLocation()+FVector(0,-250,30));
  if(!RequireV2(RV->Enter(&P,0),TEXT("player enters RV passenger seat")))return;
  Check(ALWVehicle::LastDrivenVIN(P.World)==VIN,TEXT("passenger entry preserves previous driven VIN"));RV->Exit(true);
  auto* Save=NewObject<ULWSaveGame>();Save->Vehicles=P.World->Vehicles;TArray<uint8> Bytes;
  if(!RequireV2(UGameplayStatics::SaveGameToMemory(Save,Bytes),TEXT("vehicle records serialize")))return;
  auto* Loaded=Cast<ULWSaveGame>(UGameplayStatics::LoadGameFromMemory(Bytes));
  if(!RequireV2(Loaded!=nullptr,TEXT("vehicle record archive restores")))return;
  const auto* R=Loaded->Vehicles.Find(C->RecordId);
  Check(R&&R->LastDriven&&R->VIN==VIN&&R->Position.Equals(C->GetActorLocation(),.01)&&R->FuelLitres==15,TEXT("last driven identity position and remaining fuel persist"));
  auto Original=MoveTemp(P.World->Vehicles);P.World->Vehicles=Loaded->Vehicles;
  Check(ALWVehicle::LastDrivenVIN(P.World)==VIN&&ALWVehicle::LastDrivenId(P.World)==C->RecordId,TEXT("map accessors resolve deserialized record data"));
  P.World->Vehicles=MoveTemp(Original);
  LWV2Teleport(P,RV->GetActorLocation()+FVector(0,-250,30));
  if(!RequireV2(RV->Enter(&P,-1),TEXT("player takes another driver seat")))return;
  int32 Marked=0;for(const auto& Pair:P.World->Vehicles)Marked+=Pair.Value.LastDriven?1:0;
  Check(Marked==1&&!C->Record()->LastDriven&&ALWVehicle::LastDrivenVIN(P.World)==RV->Record()->VIN,TEXT("new driver seat replaces previous marker uniquely"));RV->Exit(true);
 });
 Add(TEXT("full-car overflow starts engine in boarding call"),[this,S](ALWCharacter& P){
  auto* C=S->Sedan.Get();auto* RV=S->RV.Get();if(!RequireV2(C&&RV,TEXT("convoy vehicles available")))return;
  P.ClosePanels();P.RPG.Crew.Empty();LWV2Teleport(P,C->GetActorLocation()+FVector(0,-250,30));
  if(!RequireV2(C->Enter(&P,-1),TEXT("player takes convoy leader")))return;
  auto Recruit=[&](int32 Index,FVector At){auto* N=GetWorld()->SpawnActor<ALWResident>(At,FRotator::ZeroRotator);if(!N)return N;
   N->ConfigureResident(FName(*FString::Printf(TEXT("vehicle30_crew_%d"),Index)),TEXT("recruit"),TEXT("Smoke Crew"),0);
   N->SetActorTickEnabled(false);N->GetCharacterMovement()->DisableMovement();S->Crew.Add(N);
   FLWCrewRecord Crew;Crew.Id=N->ResidentId;Crew.Following=true;P.RPG.Crew.Add(Crew);return N;
  };
  auto* First=Recruit(0,RV->GetActorLocation()+FVector(0,-200,30));
  if(!RequireV2(First!=nullptr,TEXT("priority companion spawned")))return;
  Check(C->HasFreeCompanionSeat()&&!RV->BoardConvoy(First,&P)&&!First->Riding,TEXT("distant free player seat prevents overflow boarding"));
  First->SetActorLocation(C->GetActorLocation()+FVector(0,-200,30));Check(C->Board(First),TEXT("companion uses player car first"));
  for(int32 I=1;I<C->Spec().Seats-1;I++){auto* N=Recruit(I,C->GetActorLocation()+FVector(I*35,-200,30));Check(N&&C->Board(N),TEXT("remaining player passenger seat filled"));}
  Check(!C->HasFreeCompanionSeat(),TEXT("leader is full before overflow allocation"));
  auto* Overflow=Recruit(20,RV->GetActorLocation()+FVector(0,-200,30));
  if(!RequireV2(Overflow!=nullptr,TEXT("overflow companion spawned")))return;
  RV->Record()->Unlocked=RV->Record()->Hotwired=false;RV->Record()->FuelLitres=20;RV->EngineOn=false;
  Check(RV->BoardConvoy(Overflow,&P),TEXT("overflow companion boards locked spare vehicle"));
  // Deliberately before TickConvoy: a future timer cannot satisfy this assertion.
  Check(RV->Chauffeur==Overflow&&RV->EngineOn&&RV->AutoDriving&&RV->ConvoyStartDelay==0&&RV->Record()->Unlocked&&RV->Record()->Hotwired,TEXT("overflow boarding immediately enables driving without key delay"));
  auto* Replacement=Recruit(21,RV->GetActorLocation()+FVector(0,-200,30));
  auto* Rider=Recruit(22,RV->GetActorLocation()+FVector(30,-200,30));
  if(!RequireV2(Replacement&&Rider&&RV->BoardConvoy(Replacement,&P)&&RV->BoardConvoy(Rider,&P),TEXT("overflow passenger fixtures board full-car convoy")))return;
  RV->SetActorLocationAndRotation(C->GetActorLocation()-FVector(C->Spec().HalfLength+RV->Spec().HalfLength+320,0,0),FRotator::ZeroRotator);RV->SyncRecord();
  // Free exactly one leader seat; existing player-car occupants stand well clear.
  auto FreeLeaderSeat=[&](ALWResident* Keep){for(auto N:C->Passengers)if(IsValid(N)&&N!=Keep&&N!=Overflow&&N!=Replacement&&N!=Rider){
   N->LeaveVehicle();N->SetActorLocation(C->GetActorLocation()+FVector(0,3000,100));return true;
  }return false;};
  Check(FreeLeaderSeat(nullptr),TEXT("one player-car seat becomes available"));
  C->Speed=40;RV->Speed=0;Rider->TickSocial(.05f,&P,true,0);
  Check(Rider->Riding==RV,TEXT("overflow rider stays aboard while player car moves"));
  C->Speed=0;RV->Speed=40;Rider->TickSocial(.05f,&P,true,0);
  Check(Rider->Riding==RV,TEXT("overflow rider stays aboard while convoy car moves"));
  RV->Speed=0;Rider->TickSocial(.05f,&P,true,0);
  Check(Rider->Riding==C&&RV->Chauffeur==Overflow&&!C->HasFreeCompanionSeat(),TEXT("stopped passenger transfer fills only the available player seat"));
  Replacement->TickSocial(.05f,&P,true,0);
  Check(Replacement->Riding==RV,TEXT("remaining convoy passengers stay when player car is full"));
  Check(FreeLeaderSeat(nullptr),TEXT("second player-car seat becomes available"));
  Replacement->DownTime=10;Overflow->TickSocial(.05f,&P,true,0);
  Check(Overflow->Riding==RV&&RV->Chauffeur==Overflow,TEXT("driver cannot strand an incapacitated remaining passenger"));
  Replacement->DownTime=0;C->Speed=40;Overflow->TickSocial(.05f,&P,true,0);
  Check(Overflow->Riding==RV&&RV->Chauffeur==Overflow,TEXT("driver handoff also refuses a moving player car"));
  C->Speed=0;Overflow->TickSocial(.05f,&P,true,0);
  Check(Overflow->Riding==C&&RV->Chauffeur==Replacement&&Replacement->SeatIndex==-1&&Replacement->Riding==RV&&RV->EngineOn&&RV->AutoDriving,TEXT("stopped driver transfer promotes remaining passenger before departure"));
  Check(!C->HasFreeCompanionSeat(),TEXT("driver handoff reserves exactly one destination seat"));
  Check(FreeLeaderSeat(nullptr),TEXT("last player-car seat becomes available"));
  Replacement->TickSocial(.05f,&P,true,0);
  Check(Replacement->Riding==C&&!RV->Chauffeur&&!RV->ConvoyOwner&&!RV->ConvoyLeader&&!RV->EngineOn&&!RV->AutoDriving,TEXT("last stopped convoy driver rejoins and parks empty convoy vehicle"));
  Check(C->Passengers.Contains(Rider)&&C->Passengers.Contains(Overflow)&&C->Passengers.Contains(Replacement),TEXT("all transferred riders occupy distinct player-car passenger seats"));
  C->Exit(true);RV->UnloadPassengers();RV->ConvoyOwner=nullptr;RV->ConvoyLeader=nullptr;RV->EngineOn=false;
 });
 Add(TEXT("vehicle30 cleanup"),[S](ALWCharacter& P){
  if(P.Vehicle)P.Vehicle->Exit(true);
  for(auto N:S->Crew)if(N.IsValid())N->Destroy();P.RPG.Crew.Empty();
  for(auto C:{S->RV,S->Sedan,S->Pending})if(C.IsValid()){const FName Id=C->RecordId;C->Destroy();P.World->Vehicles.Remove(Id);}
  for(FName Id:S->Reservations)P.World->Vehicles.Remove(Id);
  if(S->Floor.IsValid())S->Floor->Destroy();
 });
}
