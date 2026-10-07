#include "LWCharacter.h"
#include "LWVehicle.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "LWBunker45.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"
#include "Engine/World.h"

bool ALWCharacter::AssignRVBunk66(ALWVehicle* RV,int Bunk,FName Id){
 if(!RV||!RV->IsCamper74()||!RV->Record()||RV->Record()->Exploded||FMath::Abs(RV->Speed)>5||RV->EngineOn||Bunk<0||Bunk>=6)return false;
 auto* Crew=RPG.Crew.FindByPredicate([&](const auto& C){return C.Id==Id;});if(!Id.IsNone()&&!Crew)return false;
 // Clear the prior resident of this berth, without changing their squad membership.
 for(auto& C:RPG.Crew)if(C.HomeVehicle66==RV->RecordId&&C.HomeBunk66==Bunk){C.HomeVehicle66=NAME_None;C.HomeBunk66=-1;}
 if(Crew){Crew->HomeVehicle66=RV->RecordId;Crew->HomeBunk66=Bunk;Crew->Following=false;Crew->Station=NAME_None;Bunker45.Assignments.Remove(Id);
  for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(N->ResidentId==Id){N->SettlementId=NAME_None;N->LeaveVehicle();N->ResetCompanionNavigation();N->TickRVHome66(this);}
 }
 RequestSave40();return true;
}
void ALWCharacter::OpenRVHome66(ALWVehicle* RV,int Bunk){
 if(!RV||Vehicle!=RV||Bunk<0||Bunk>=6||FMath::Abs(RV->Speed)>5)return;
 ClosePanels();Service66=RV;ServiceBunk66=Bunk;RPGPanel=4;
 const auto* Assigned=RPG.Crew.FindByPredicate([&](const auto& C){return C.HomeVehicle66==RV->RecordId&&C.HomeBunk66==Bunk;});
 DialogueText=FString::Printf(TEXT("Bunk %d: %s. Assigned companions return to this motorhome when dismissed. Turn the engine off to change housing."),Bunk+1,Assigned?*Assigned->Name:TEXT("Unassigned"));
 DialogueChoices.Empty();DialogueActions.Empty();for(const auto& C:RPG.Crew){DialogueChoices.Add(C.Name+(C.HomeVehicle66==RV->RecordId?TEXT(" [RV]"):TEXT("")));DialogueActions.Add(FName(*(TEXT("berth:")+C.Id.ToString())));}
 DialogueChoices.Add(TEXT("Clear assignment / return resident to bunker"));DialogueActions.Add(TEXT("berth:"));DialogueChoices.Add(TEXT("Leave"));DialogueActions.Add(TEXT("bye"));CancelReload();SetMenuInput(true);
}
void ALWCharacter::OpenDelivery66(ALWWorldObject* Terminal){
 if(!Terminal||Terminal->UseType!=TEXT("delivery66")||Vehicle||FVector::Dist(GetActorLocation(),Terminal->GetActorLocation())>550)return;
 ClosePanels();Service66=Terminal;ServiceBunk66=-1;RPGPanel=4;
 DialogueText=TEXT("Vehicle delivery costs 150 credits. Choose an owned vehicle. Its keys, cargo, modifications and companion housing stay with it. Vehicles in use, destroyed vehicles or obstructed vehicles cannot be delivered.");
 DialogueChoices.Empty();DialogueActions.Empty();TArray<FName> Ids;for(const auto& Pair:World->Vehicles)if(Pair.Value.Owned45&&!LWTraffic::IsAircraft(Pair.Value.Model))Ids.Add(Pair.Key);Ids.Sort(FNameLexicalLess());
 for(FName Id:Ids){const auto& R=World->Vehicles.FindChecked(Id);DialogueChoices.Add(FString::Printf(TEXT("%s / %s%s [150 credits]"),LWTraffic::Get(R.Model).Name,*R.VIN.ToString(EGuidFormats::Digits).Right(6),R.Exploded?TEXT(" / DESTROYED"):TEXT("")));DialogueActions.Add(FName(*(TEXT("deliver:")+Id.ToString())));}
 if(Ids.IsEmpty())DialogueText=TEXT("You have no owned vehicles. Store a vehicle in your bunker garage, or set a motorhome's master bed as your respawn point.");
 DialogueChoices.Add(TEXT("Leave"));DialogueActions.Add(TEXT("bye"));CancelReload();SetMenuInput(true);
}
bool ALWCharacter::ChooseService66(int I){
 if(!IsValid(Service66)){ClosePanels();return false;}if(!DialogueActions.IsValidIndex(I))return false;
 auto* Source=Service66.Get();const FString Action=DialogueActions[I].ToString();
 if(Action==TEXT("bye")){ClosePanels();return true;}
 if(auto* RV=Cast<ALWVehicle>(Source)){if(Vehicle!=RV||FMath::Abs(RV->Speed)>5){ClosePanels();return true;}
  const int Bunk=ServiceBunk66;if(Action.StartsWith(TEXT("berth:"))){if(!AssignRVBunk66(RV,Bunk,FName(*Action.Mid(6))))Notify(TEXT("PARK AND SWITCH OFF THE ENGINE FIRST"));OpenRVHome66(RV,Bunk);}return true;
 }
 if(Action.StartsWith(TEXT("deliver:"))){FName Id(*Action.Mid(8));if(DeliverVehicle66(Id,Source))ClosePanels();return true;}return false;
}

bool ALWResident::TickRVHome66(ALWCharacter* P){
 auto* C=P->RPG.Crew.FindByPredicate([&](const auto& X){return X.Id==ResidentId;});
 auto* R=C&&!C->HomeVehicle66.IsNone()?World->Vehicles.Find(C->HomeVehicle66):nullptr;
 const bool AtHome=C&&!C->Following&&C->Station.IsNone()&&R&&!R->Exploded&&R->Health>0&&C->HomeBunk66>=0&&C->HomeBunk66<6;
 if(!AtHome){if(HomeRV66){auto* Old=HomeRV66.Get();DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);HomeRV66=nullptr;SetActorRotation(FRotator(0,Old->GetActorRotation().Yaw,0));
   FVector At=Old->GetActorTransform().TransformPosition(FVector(LWTraffic::FrontOffset(Old->Spec())+35,240,100));FRotator Rot=GetActorRotation();if(GetWorld()->FindTeleportSpot(this,At,Rot))SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);
   GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);GetCharacterMovement()->SetMovementMode(MOVE_Walking);if(Gun)Gun->SetVisibility(true);ResetCompanionNavigation();}
  return false;
 }
 ALWVehicle* RV=nullptr;for(TActorIterator<ALWVehicle> V(GetWorld());V;++V)if(V->RecordId==C->HomeVehicle66){RV=*V;break;}if(!RV)return false;
 if(Riding)LeaveVehicle();GetCharacterMovement()->StopMovementImmediately();GetCharacterMovement()->DisableMovement();GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);if(Gun)Gun->SetVisibility(false);
 if(HomeRV66!=RV){DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);HomeRV66=RV;AttachToComponent(RV->Chassis,FAttachmentTransformRules::KeepWorldTransform);ResetCompanionNavigation();}
 SetActorLocation(RV->BunkPosition66(C->HomeBunk66)+RV->GetActorUpVector()*30,false,nullptr,ETeleportType::TeleportPhysics);SetActorRotation(RV->GetActorRotation()+FRotator(90,C->HomeBunk66<3?0:180,0));
 return true;
}

bool ALWCharacter::DeliverVehicle66(FName Id,ALWWorldObject* Terminal){
 auto Fail=[&](const TCHAR* Why){Notify(Why);return false;};
 if(!Terminal||Terminal->UseType!=TEXT("delivery66")||FVector::Dist(GetActorLocation(),Terminal->GetActorLocation())>550||Vehicle)return Fail(TEXT("USE THE PARKING LOT TERMINAL ON FOOT"));
 auto* R=World->Vehicles.Find(Id);if(!R||LWTraffic::IsAircraft(R->Model)||!R->Owned45||R->Exploded||R->Health<=0||R->FireRemaining>0)return Fail(TEXT("VEHICLE UNAVAILABLE"));
 if(Money<150)return Fail(TEXT("NOT ENOUGH CREDITS"));
 if(BunkerManager45&&BunkerManager45->TransferCar&&BunkerManager45->TransferCar->RecordId==Id)return Fail(TEXT("GARAGE LIFT IS BUSY"));
 ALWVehicle* Existing=nullptr;for(TActorIterator<ALWVehicle> V(GetWorld());V;++V)if(V->RecordId==Id){Existing=*V;break;}
 if(Existing&&(Existing->Driver||Existing->Chauffeur||Existing->EngineOn||Existing->AutoDriving||Existing->Boarding||Existing->Held49||Existing->Airborne49||FMath::Abs(Existing->Speed)>5||Existing->Passengers.ContainsByPredicate([](const auto& N){return IsValid(N);})))return Fail(TEXT("VEHICLE IS IN USE"));
 const auto& S=LWTraffic::Get(R->Model);const FRotator Rotation(0,Terminal->GetActorRotation().Yaw+90,0);
 const FVector Base=Terminal->Route.IsEmpty()?Terminal->GetActorLocation()+Terminal->GetActorForwardVector()*850:FVector(Terminal->Route[0],Terminal->GetActorLocation().Z);
 FCollisionQueryParams Q(NAME_None,true,Terminal);if(Existing)Q.AddIgnoredActor(Existing);
 FCollisionQueryParams SimpleQ=Q;SimpleQ.bTraceComplex=false;
 FVector Destination=FVector::ZeroVector;bool Found=false;
 // Sample footprint support and full-height clearance, not only the centre of a bay.
 for(int Attempt=0;Attempt<5&&!Found;Attempt++){
  FVector At=Base+Terminal->GetActorForwardVector()*(Attempt*420);float Low=MAX_flt,High=-MAX_flt;bool Supported=true;
  for(float X:{-S.HalfLength+10,0.f,S.HalfLength-10})for(float Y:{-S.HalfWidth+10,S.HalfWidth-10}){FVector V=At+Rotation.RotateVector(FVector(X,Y,0));FHitResult H;
   // Start above the coach, not inside a parked obstacle. Mesh overlaps alone can
   // miss a volume wholly enclosed by triangles; never deliver onto its roof.
   if(!GetWorld()->LineTraceSingleByChannel(H,V+FVector(0,0,FMath::Max(600.f,S.Height+300)),V-FVector(0,0,340),ECC_Visibility,Q)||H.ImpactNormal.Z<.94f||FMath::Abs(H.ImpactPoint.Z-Base.Z)>90||!Cast<ALWChunk>(H.GetActor())){Supported=false;break;}Low=FMath::Min(Low,float(H.ImpactPoint.Z));High=FMath::Max(High,float(H.ImpactPoint.Z));}
  if(!Supported||High-Low>25)continue;At.Z=High+75;
  const FVector Center=At+FVector(0,0,S.Height*.5f-68);const auto Bounds=FCollisionShape::MakeBox(FVector(S.HalfLength+22,S.HalfWidth+24,S.Height*.5f-8));
  if(GetWorld()->OverlapBlockingTestByChannel(Center,Rotation.Quaternion(),ECC_Visibility,Bounds,Q)||GetWorld()->OverlapBlockingTestByChannel(Center,Rotation.Quaternion(),ECC_Visibility,Bounds,SimpleQ))continue;
  bool NearCar=false;for(TActorIterator<ALWVehicle> V(GetWorld());V;++V)if(*V!=Existing&&FVector::Dist2D(V->GetActorLocation(),At)<S.HalfLength+V->Spec().HalfLength+40){NearCar=true;break;}if(NearCar)continue;
  Destination=At;Found=true;
 }
 if(!Found)return Fail(TEXT("DELIVERY BAY BLOCKED. CLEAR THE PARKING AREA"));
 const auto Before=*R;R->Position=Destination;R->Rotation=Rotation;R->Stored45=false;R->Slides66=R->Awning66=false;
 ALWVehicle* Car=Existing;if(!Car)Car=Cast<ALWVehicle>(World->SpawnObject(ELWObjectKind::Car,Id,Destination,Rotation));
 if(!Car||(!Existing&&Car->SpawnPlacementPending)){if(!Existing&&Car)Car->Destroy();*R=Before;return Fail(TEXT("NO SAFE DELIVERY POSITION"));}
 for(auto& Chunk:World->Chunks)Chunk.Value->Residents.Remove(Car);
 Car->SetActorLocationAndRotation(Destination,Rotation,false,nullptr,ETeleportType::TeleportPhysics);Car->SpawnPlacementPending=false;Car->Speed=Car->Throttle=Car->Steer=0;Car->EngineOn=false;Car->PendingIgnition66=false;Car->SlideAlpha66=Car->AwningAlpha66=0;Car->TickLuxury66(0);Car->SetActorTickEnabled(true);Car->SetActorHiddenInGame(false);Car->SetActorEnableCollision(true);Car->SyncRecord();
 Money-=150;SetWaypoint(FVector2D(Destination));RequestSave40();Notify(TEXT("VEHICLE DELIVERED / 150 CREDITS"));return true;
}
