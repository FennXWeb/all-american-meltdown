#include "LWAircraft84.h"
#include "LWVehicle.h"
#include "LWGarage45.h"
#include "LWFuel.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "ProceduralMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SpotLightComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Crc.h"
#include "InputCoreTypes.h"
namespace {
bool CanOfferRefuel(const ALWVehicle& Vehicle,const ALWCharacter* Player){
 const auto* R=Vehicle.Record();
 if(Vehicle.IsElectric74()||!IsValid(Player)||!Player->CanAct()||Player->World!=Vehicle.World||Vehicle.SpawnPlacementPending||!R||R->Exploded||R->Health<=0||R->FireRemaining>0)return false;
 if(FMath::Abs(Vehicle.Speed)>5||Vehicle.EngineOn||Vehicle.AutoDriving||Vehicle.Boarding||Vehicle.FuelCapacity()-Vehicle.FuelLitres()<1)return false;
 if(FVector::Dist2D(Player->GetActorLocation(),Vehicle.GetActorLocation())>Vehicle.Spec().HalfLength+350)return false;
 return Player->Inventory.ContainsByPredicate([](const auto& I){return I.Definition==TEXT("gas_can")&&I.Slot==TEXT("Tool")&&I.Count==1&&I.Rounds>0;});
}
}
namespace {FString CamperLabel(FName A){const FString N=A.ToString();if(N.StartsWith(TEXT("seat_"))){int I=FCString::Atoi(*N.Mid(5));return I<0?TEXT("DRIVER SEAT"):FString::Printf(TEXT("PASSENGER SEAT %d"),I+1);}if(N.StartsWith(TEXT("cargo_"))){TArray<FString> P;N.ParseIntoArray(P,TEXT("_"));return FString::Printf(TEXT("CARGO HATCH %d"),P.Num()==3?FCString::Atoi(*P[2])+(P[1]==TEXT("-1")?1:8):1);}if(A==TEXT("kitchen_storage"))return TEXT("KITCHEN CUPBOARD");if(A==TEXT("sink_storage"))return TEXT("SINK CABINET");if(A==TEXT("bed_storage"))return TEXT("UNDER-BED STORAGE");return N.Replace(TEXT("_"),TEXT(" ")).ToUpper();}}
ALWVehicle::ALWVehicle(){DamageClock=2;
 Chassis=CreateDefaultSubobject<UBoxComponent>(TEXT("SweptChassis"));SetRootComponent(Chassis);Chassis->SetBoxExtent(FVector(205,87,48));Chassis->SetCollisionProfileName(TEXT("BlockAllDynamic"));Chassis->SetCollisionResponseToChannel(ECC_Pawn,ECR_Ignore);Chassis->SetCollisionResponseToChannel(ECC_PhysicsBody,ECR_Ignore);Root->SetupAttachment(Chassis);Root->SetRelativeLocation(FVector(0,0,-75));
}
FLWVehicleRecord* ALWVehicle::Record()const{return World?World->Vehicles.Find(RecordId):nullptr;}
FName ALWVehicle::GloveId()const{return FName(*(TEXT("glove_")+RecordId.ToString()));}
void ALWVehicle::InitializeVehicle(){
 auto* R=Record();if(!R){FLWVehicleRecord V;V.VIN=FGuid::NewDeterministicGuid(RecordId.ToString(),uint64(uint32(World->Seed)));V.Position=GetActorLocation();V.Position.Z=World->HeightAt(FVector2D(V.Position))+75;V.Rotation=GetActorRotation();uint32 H=FCrc::StrCrc32(*V.VIN.ToString());V.Model=LWTraffic::Choose(H);V.LockTier=H%100<45?0:1+H%4;V.Unlocked=V.LockTier==0;World->Vehicles.Add(RecordId,V);R=Record();}
 SetActorLocationAndRotation(R->Position,R->Rotation);if(IsAircraft84()){BuildAircraft84();return;}Body->SetStaticMesh(World->Mesh(TEXT("SedanShellV5")));Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 Part(TEXT("VehicleCabinV9"),FVector::ZeroVector,FVector(1));Details.Last()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 auto Visual=[&](FName Mesh,FVector At,FRotator Rot=FRotator::ZeroRotator){Part(Mesh,At,FVector(1),Rot);auto* M=Details.Last().Get();M->SetCollisionEnabled(ECollisionEnabled::NoCollision);return M;};
 SteeringWheel=Visual(TEXT("SteeringV5"),FVector(12,-43,108));GloveLid=Visual(TEXT("GloveLidV5"),FVector(32,54,74));
 for(int S:{-1,1}){Wipers.Add(Visual(TEXT("WiperV5"),FVector(70,S*43,113),FRotator(0,0,0)));for(int X:{-132,132})Wheels.Add(Visual(TEXT("SedanWheelV5"),FVector(X,S*95,32)));}
 auto Switch=[&](FName Name,FVector At,FVector Size,FName Mat){Part(TEXT("Cube"),At,Size/100,FRotator::ZeroRotator,Mat);auto* M=Details.Last().Get();M->ComponentTags.Add(Name);M->SetCollisionEnabled(ECollisionEnabled::QueryOnly);M->SetCollisionResponseToAllChannels(ECR_Ignore);M->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);};
 Switch(TEXT("radio"),FVector(32,4,99),FVector(6,27,12),TEXT("Steel"));Switch(TEXT("glove"),FVector(30,53,85),FVector(6,45,14),TEXT("Cloth"));
 Switch(TEXT("ignition"),FVector(17,-23,93),FVector(4,4,4),TEXT("Steel"));Switch(TEXT("lights"),FVector(30,-76,99),FVector(6,10,10),TEXT("Bone"));
 Switch(TEXT("wipers"),FVector(24,25,104),FVector(6,10,8),TEXT("Bone"));Switch(TEXT("left"),FVector(10,-67,104),FVector(17,6,6),TEXT("Steel"));Switch(TEXT("right"),FVector(10,-20,108),FVector(17,6,6),TEXT("Steel"));Switch(TEXT("exit"),FVector(-45,-88,88),FVector(25,8,8),TEXT("Steel"));
 for(int S:{-1,1}){auto* L=NewObject<USpotLightComponent>(this);L->SetupAttachment(Root);L->SetRelativeLocation(FVector(235,S*65,73));L->SetIntensity(900000);L->SetAttenuationRadius(22000);L->SetInnerConeAngle(12);L->SetOuterConeAngle(34);L->SetLightColor(FLinearColor(1,.84f,.58f));L->SetVisibility(false);L->RegisterComponent();Headlamps.Add(L);
 auto* B=NewObject<UPointLightComponent>(this);B->SetupAttachment(Root);B->SetRelativeLocation(FVector(218,S*83,64));B->SetIntensity(3000);B->SetAttenuationRadius(220);B->SetLightColor(FLinearColor(1,.3f,.01f));B->SetVisibility(false);B->RegisterComponent();Indicators.Add(B);}
 BuildVariant();BuildDetail42();BuildLuxury66();BuildElectric74();RepairCoach80();ApplyGarage45();
 if(!IsElectric74()&&!R->FuelLootInitialized){if(auto* Cargo=World->Containers.Find(RecordId)){
  if(R->Model==TEXT("rv")||FCrc::StrCrc32(*R->VIN.ToString())%4==0){auto Can=LWFuel::MakeGasCan();if(Can.Id.IsValid())LWItems::Place(Cargo->Items,Can,Cargo->Width,Cargo->Height);}
  R->FuelLootInitialized=true;
 }}
 SpawnPlacementPending=!ResolveSpawnPlacement();SetActorHiddenInGame(SpawnPlacementPending);SetActorEnableCollision(!SpawnPlacementPending);TickFuel(0);if(!R->Dents.IsEmpty())RebuildDents();
 if(!World->Containers.Contains(GloveId())){FLWContainerRecord G;G.Id=GloveId();G.Context=TEXT("glovebox");G.Width=6;G.Height=4;G.Position=GetActorLocation();auto Pick=LWItems::Make(TEXT("lockpick"),2);LWItems::Place(G.Items,Pick,6,4);
 if(FCrc::StrCrc32(*(R->VIN.ToString()+TEXT(":glovebox-key")))%100<45){auto Key=LWItems::Make(TEXT("car_key"));Key.VehicleId=R->VIN;Key.Id=FGuid::NewDeterministicGuid(TEXT("key:")+R->VIN.ToString());LWItems::Place(G.Items,Key,6,4);}World->Containers.Add(G.Id,G);}
}
void ALWVehicle::SyncRecord(){if(!World||Held49||Airborne49)return;if(Record()&&Record()->Stored45&&!IsActorTickEnabled())return;if(auto* R=Record()){R->Position=GetActorLocation();R->Rotation=GetActorRotation();}if(auto* G=World->Containers.Find(GloveId()))G->Position=GetActorLocation();if(auto* C=World->Containers.Find(RecordId))C->Position=GetActorLocation();if(IsCamper74())for(const TCHAR* N:{TEXT("fridge"),TEXT("pantry"),TEXT("wardrobe")})if(auto* C=World->Containers.Find(FName(*(RecordId.ToString()+TEXT("_")+N))))C->Position=GetActorLocation();}
FString ALWVehicle::VehiclePrompt49()const{if(SpawnPlacementPending)return FString();if(Record()&&Record()->Exploded)return TEXT("DESTROYED VEHICLE");if(auto* User=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0)))if(CanOfferRefuel(*this,User))return FString::Printf(TEXT("[E] REFUEL %s // %.1f / %.0f L (ENGINE OFF)"),Spec().Name,FuelLitres(),FuelCapacity());if(Record()&&Record()->Exploded)return TEXT("DESTROYED VEHICLE");if(IsAircraft84()){auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));return TEXT("[E] ")+AircraftPrompt84(AircraftFocus84(P));}if(IsCamper74()){auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));FName A=CamperFocus(P);if(A.IsNone())return FString();if(A==TEXT("entry"))return Record()&&!Record()->Unlocked?TEXT("[E] LOCKED DOOR"):CamperDoorOpen?TEXT("[E] CLOSE DOOR"):TEXT("[E] OPEN DOOR");return TEXT("[E] ")+(A.ToString().Contains(TEXT("66"))?LuxuryLabel66(A):CamperLabel(A));}const auto* R=Record();auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(R&&R->Unlocked&&P){int Seat=NearestSeat(P);return FString::Printf(TEXT("[E] %s / %s  [G] CARGO"),Spec().Name,Seat<0?TEXT("DRIVER"):*FString::Printf(TEXT("SEAT %d"),Seat+1));}return R?FString::Printf(TEXT("[E] %s // %s  [G] CARGO"),R->Unlocked?Spec().Name:LWSecurity::Tier(R->LockTier),*R->VIN.ToString(EGuidFormats::Digits).Left(12)):TEXT("SEDAN");}
void ALWVehicle::Use(ALWCharacter* P){if(IsAircraft84()){UseAircraft84(P,AircraftFocus84(P));return;}if(Held49||Airborne49)return;if(P)if(auto* PC=Cast<APlayerController>(P->Controller))if(PC->IsInputKeyDown(EKeys::LeftShift)||PC->IsInputKeyDown(EKeys::RightShift)){Flip49(P);return;}if(CanOfferRefuel(*this,P)){Refuel(P);return;}if(IsCamper74()){UseCamper(P,CamperFocus(P));return;}if(!P||P->Vehicle||!P->CanAct())return;auto* R=Record();if(!R)return;if(!R->Unlocked&&P->HasKey(R->VIN)){R->Unlocked=true;P->RequestSave40();}if(!R->Unlocked){P->StartLockpick(this);return;}Enter(P,NearestSeat(P));}
bool ALWVehicle::Enter(ALWCharacter* P,int Seat){if(Grabber49.IsValid()||Airborne49||!P||P->Vehicle||Driver||SpawnPlacementPending||(ConvoyOwner&&ConvoyOwner!=P)||!Record()||Record()->Exploded||!Record()->Unlocked||FVector::Dist(P->GetActorLocation(),GetActorLocation())>Spec().HalfLength+350||Seat < ((IsCamper74()||IsAircraft84())?-2:-1)||Seat>=Spec().Seats-1)return false;
 if(FMath::Abs(Speed)>80)return false;
 if(Seat>=0&&Passengers.IsValidIndex(Seat)&&IsValid(Passengers[Seat]))return false;
 if(ConvoyOwner==P){UnloadPassengers();ConvoyOwner=nullptr;ConvoyLeader=nullptr;AutoDriving=Boarding=StopRequested=false;Throttle=Steer=Speed=0;}
 if(Seat==-1&&IsValid(Chauffeur))return false;
 if(!IsCamper74()&&!IsAircraft84())VehicleSound42(TEXT("CarDoorOpen"),.65f);
 P->ClosePanels();P->CancelReload();if(Seat==-2)CabinEye=IsSolarRV74()?CabinToActor74(FVector(35,140,225)):FVector(LWTraffic::FrontOffset(Spec())+35,140,150);if(IsAircraft84()&&Seat==-2){const auto A=LWAviation84::Spec(Spec().Id);CabinEye=FVector(A.HalfCabin-100,22,A.Floor+164);}CabinVelocity74=FVector::ZeroVector;CabinPitch74=GetActorRotation().Pitch;PlayerSeat=Seat;DriverDismissed=false;Driver=P;P->Vehicle=this;if(Seat==-1)MarkLastDriven();P->SeatYaw=P->SeatPitch=0;P->GetCharacterMovement()->StopMovementImmediately();P->GetCharacterMovement()->DisableMovement();P->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);P->WeaponRoot->SetVisibility(false,true);P->Flashlight->SetVisibility(false);P->bAim=P->bSprint=false;P->AddTickPrerequisiteActor(this);Chassis->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 for(auto& C:World->Chunks)C.Value->Residents.Remove(this);P->TickVehicleSeat();if(Seat!=-2)for(TActorIterator<ALWResident> I(GetWorld());I;++I)if(P->RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Following&&C.Id==I->ResidentId;}))Board(*I);P->Notify(Seat==-2?TEXT("WASD WALK / E INTERACT"):TEXT("[1-8 / WHEEL] SEATS  [R] IGNITION  [E] STOP / EXIT"),8);return true;
}
bool ALWVehicle::Exit(bool Force){if(!Driver)return false;if(!Force&&IsAircraft84()&&!AircraftOnGround84){Driver->Notify(TEXT("LAND BEFORE EXITING THE AIRCRAFT"));return false;}if(!Force&&IsHelicopter57()&&Altitude57>120){Driver->Notify(TEXT("LAND BEFORE EXITING"));return false;}TurretTrigger57=false;auto* P=Driver.Get();if(!Force&&(Chauffeur||SelfDriving74)){RequestDriverStop();return false;}if(!Force&&FMath::Abs(Speed)>80){P->Notify(TEXT("STOP THE CAR BEFORE EXITING"));return false;}
 FVector ExitAt;bool Found=false;FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(P);
 for(FVector Local:{FVector(0,-185,100),FVector(0,185,100),FVector(-310,0,100)}){if(IsCamper74())Local=IsSolarRV74()?CabinToActor74(FVector(35,225,175)):FVector(LWTraffic::FrontOffset(Spec())+35,220,100);if(IsAircraft84())Local=FVector(Spec().HalfLength-490,LWAviation84::Spec(Spec().Id).Width+200,100);FVector At=GetActorTransform().TransformPosition(Local);FHitResult Floor;if(GetWorld()->LineTraceSingleByChannel(Floor,At+FVector(0,0,150),At-FVector(0,0,600),ECC_Visibility,Q)&&Floor.ImpactNormal.Z>.7f){At=Floor.ImpactPoint+FVector(0,0,P->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()+4);if(!GetWorld()->OverlapBlockingTestByChannel(At,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeCapsule(P->GetCapsuleComponent()->GetScaledCapsuleRadius(),P->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()),Q)){ExitAt=At;Found=true;break;}}}
 if(!Found&&!Force){P->Notify(TEXT("EXIT BLOCKED // MOVE TO A CLEAR PARKING SPACE"));return false;}if(!Found)ExitAt=GetActorLocation()+FVector(0,0,200);
 SelfDriving74=false;AutoDriving=Boarding=StopRequested=false;CabinVelocity74=FVector::ZeroVector;CabinMove=FVector2D::ZeroVector;UnloadPassengers();PlayerSeat=-1;P->ClosePanels();P->RemoveTickPrerequisiteActor(this);P->Vehicle=nullptr;Driver=nullptr;Throttle=Steer=Speed=0;P->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);P->GetCharacterMovement()->SetMovementMode(MOVE_Walking);P->SetActorLocation(ExitAt,false,nullptr,ETeleportType::TeleportPhysics);P->SetMenuInput(false);Chassis->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);SyncRecord();if(!Force){VehicleSound42(TEXT("CarDoorClose"));P->RequestSave40();}return true;
}
void ALWVehicle::Ignition(){if(!Driver||PlayerSeat!=-1||Driver->bMenu||Driver->SecurityMode)return;auto* R=Record();if(IsAircraft84()&&R){if(!AircraftOnGround84&&R->Autopilot84){Driver->Notify(TEXT("DISENGAGE AUTOPILOT BEFORE STOPPING ENGINES"));return;}EngineOn=!EngineOn&&HasFuel();R->FlightEngine84=EngineOn;return;}if(!R||R->Health<=0){Driver->Notify(TEXT("ENGINE DESTROYED"));return;}if(PendingIgnition66){PendingIgnition66=false;Driver->Notify(TEXT("START CANCELLED"));return;}if(EngineOn){EngineOn=false;if(SelfDriving74)RequestDriverStop();return;}if(!HasFuel()){Driver->Notify(IsElectric74()?TEXT("BATTERY EMPTY / SOLAR CHARGE OR USE A GARAGE"):TEXT("OUT OF FUEL // EQUIP A GAS CAN AND USE THE PARKED VEHICLE"));return;}if(!R->Hotwired&&!Driver->HasKey(R->VIN)){Driver->StartHotwire();return;}if(!ReadyToDrive66())return;EngineOn=true;}
FName ALWVehicle::FocusControl()const{if(IsAircraft84())return AircraftFocus84(Driver);if(IsCamper74())return CamperFocus(Driver);if(!Driver)return NAME_None;FHitResult H;FCollisionQueryParams Q(NAME_None,false,Driver);Q.AddIgnoredComponent(Chassis.Get());FVector A=Driver->Camera->GetComponentLocation();if(GetWorld()->LineTraceSingleByChannel(H,A,A+Driver->Camera->GetForwardVector()*(IsCamper74()?850:250),ECC_Visibility,Q)&&H.GetActor()==this&&H.GetComponent()&&H.GetComponent()->ComponentTags.Num())return H.GetComponent()->ComponentTags[0];return NAME_None;}
FString ALWVehicle::CabinPrompt()const{if(IsAircraft84()){const FName A=AircraftFocus84(Driver);return A.IsNone()?TEXT("[R] ENGINE [G] GEAR [SHIFT] / [LEFT CTRL] THRUST [SPACE] WALK"):TEXT("[E] ")+AircraftPrompt84(A);}if(HasTurret57()&&PlayerSeat==0)return FString::Printf(TEXT("GUNNER | %d ROUNDS | [LMB] FIRE [R] LOAD FROM CARGO [1-8] SEAT [E] EXIT"),Record()?Record()->TurretRounds57:0);if(IsHelicopter57())return TEXT("[R] ENGINE [W/S] PITCH [A/D] YAW [SPACE] ASCEND [LEFT CTRL] DESCEND [E] LAND / EXIT");if(IsCamper74()){FName A=FocusControl();if(A==TEXT("autopilot74"))return SelfDriving74?TEXT("[E] STOP AUTOPILOT"):TEXT("[E] DRIVE TO WAYPOINT");if(A==TEXT("climate74"))return TEXT("[E] AMBIENT LIGHTING");if(A==TEXT("entry"))return CamperDoorOpen?TEXT("[E] CLOSE DOOR"):TEXT("[E] OPEN DOOR");if(A.IsNone())return PlayerSeat==-2?TEXT("[E] USE / WALK THROUGH OPEN DOOR"):TEXT("[SPACE] STAND UP");return TEXT("[E] ")+(A.ToString().Contains(TEXT("66"))?LuxuryLabel66(A):CamperLabel(A));}FName C=FocusControl();if(IsElectric74()&&(C==TEXT("autopilot74")||C==TEXT("feature")))return SelfDriving74?TEXT("[E] STOP AUTOPILOT"):TEXT("[E] DRIVE TO WAYPOINT");return C.IsNone()?(IsCamper74()?TEXT("[SPACE] WALK CABIN  [1-5] SEATS  [E] EXIT"):TEXT("[1-8 / WHEEL] CHANGE SEAT   [E] STOP / EXIT")):TEXT("[E] ")+(C==TEXT("feature")?FString(Spec().Feature).ToUpper():C.ToString().ToUpper());}
void ALWVehicle::Control(FName A){if(IsAircraft84()&&UseAircraft84(Driver,A))return;if(A==TEXT("turret_reload")){ReloadTurret57();return;}if(!Driver||Driver->bMenu||Driver->SecurityMode)return;
 if(UseElectric74(Driver,A)||UseLuxury66(Driver,A))return;
 if(IsCamper74()&&(A==TEXT("entry")||A.ToString().StartsWith(TEXT("cargo_"))||A.ToString().EndsWith(TEXT("_storage")))){UseCamper(Driver,A);return;}
 if(A.ToString().StartsWith(TEXT("seat_"))){ChangeSeat(FCString::Atoi(*A.ToString().Mid(5)));return;}
 if(IsCamper74()){
 if(A==TEXT("bed")||A==TEXT("cooker")||A==TEXT("sink")||A==TEXT("fridge")||A==TEXT("pantry")||A==TEXT("wardrobe")){
 if((FMath::Abs(Speed)>5||AutoDriving||Boarding)&&!(IsSolarRV74()&&SelfDriving74&&A!=TEXT("bed"))){Driver->Notify(TEXT("PARK FIRST"));return;}
 if(A==TEXT("bed")){Driver->OpenBedMenu(this);return;}
 if(A==TEXT("cooker")){if(Driver->ConsumeSupply(TEXT("food"),1)){Driver->Hunger=100;Driver->Notify(TEXT("HOT MEAL"));Driver->RequestSave40();}else Driver->Notify(TEXT("REQUIRES FOOD"));return;}
 if(A==TEXT("sink")){Driver->Thirst=100;Driver->Notify(TEXT("FILTERED WATER"));Driver->RequestSave40();return;}
 OpenCamperStorage(Driver,A);return;
 }}
 if(A==TEXT("feature")||A==TEXT("siren")){
 if(A==TEXT("siren")){SirenOn=!SirenOn;FeatureOn=SirenOn||FeatureOn;return;}
 if(IsCamper74()){if(FMath::Abs(Speed)>1){Driver->Notify(TEXT("PARK BEFORE RESTING"));return;}if(Driver->ConsumeSupply(TEXT("food"),1)){Driver->Health=FMath::Min(Driver->MaxHealth(),Driver->Health+25);Driver->Stamina=Driver->MaxStamina();Driver->RequestSave40();Driver->Notify(TEXT("RESTED"));}else Driver->Notify(TEXT("NEEDS ONE FOOD RATION"));return;}
 if(FMath::Abs(Speed)>80&&(Spec().Feature==FName(TEXT("door"))||Spec().Feature==FName(TEXT("ramp"))||Spec().Feature==FName(TEXT("tailgate")))){Driver->Notify(TEXT("STOP FIRST"));return;}
 FeatureOn=!FeatureOn;VehicleSound42(FeatureOn?TEXT("CarCargoOpen"):TEXT("CarCargoClose"));Driver->Notify(FString(Spec().Feature).ToUpper()+(FeatureOn?TEXT(" ON"):TEXT(" OFF")));return;}
 if(A==TEXT("horn")){if(HornCooldown42<=0){VehicleSound42(TEXT("CarHorn"),.85f);World->Noise(GetActorLocation(),5000);HornCooldown42=.8f;}return;}
 if(A==TEXT("exit")){Exit();return;}if(A==TEXT("ignition")){Ignition();return;}
 if(A==TEXT("glove")||A==TEXT("cargo")){if(FMath::Abs(Speed)>80){Driver->Notify(TEXT("STOP TO ACCESS STORAGE"));return;}if(A==TEXT("glove")&&GloveOpen){GloveOpen=false;VehicleSound42(TEXT("CarGloveClose"));Driver->ClosePanels();return;}GloveOpen=A==TEXT("glove");VehicleSound42(GloveOpen?TEXT("CarGloveOpen"):TEXT("CarCargoOpen"));auto& O=GloveOpen?GloveObject:CargoObject;if(O&&O->RecordId!=(GloveOpen?GloveId():RecordId)){O->Destroy();O=nullptr;}if(!O){O=World->SpawnObject(ELWObjectKind::Container,GloveOpen?GloveId():RecordId,GetActorLocation());O->SetActorHiddenInGame(true);O->SetActorEnableCollision(false);O->SetActorTickEnabled(false);}O->SetActorLocation(Driver->GetActorLocation());Driver->OpenContainer(O);return;}
 if(A==TEXT("lights"))Headlights=!Headlights;if(A==TEXT("wipers"))WipersOn=!WipersOn;
 if(A==TEXT("left"))Signal=Signal==-1?0:-1;if(A==TEXT("right"))Signal=Signal==1?0:1;
 if(A==TEXT("radio")){RadioChannel=(RadioChannel+1)%3;if(RadioAudio){RadioAudio->Stop();RadioAudio=nullptr;}if(RadioChannel){RadioAudio=World->Sound(TEXT("CarRadio"),GetActorLocation(),.35f,RadioChannel==1?1.f:.78f);if(RadioAudio)RadioAudio->AttachToComponent(Root,FAttachmentTransformRules::KeepWorldTransform);}Driver->Notify(RadioChannel?FString::Printf(TEXT("CIVIL RADIO // CHANNEL %d"),RadioChannel):TEXT("RADIO OFF"));}
 VehicleSound42(A==TEXT("radio")?TEXT("CarRadioSwitch"):TEXT("CarSwitch"),.5f);
}
void ALWVehicle::Tick(float Dt){
 AActor::Tick(Dt);if(!World)return;auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));if(P&&(!P->bStarted||P->bMenu))return;if(IsAircraft84()){TickAircraft84(Dt);return;}Dt=FMath::Min(Dt,.05f);
 if(SpawnPlacementPending){SpawnPlacementRetry-=Dt;if(SpawnPlacementRetry<=0){SpawnPlacementRetry=2;SpawnPlacementPending=!ResolveSpawnPlacement();SetActorHiddenInGame(SpawnPlacementPending);SetActorEnableCollision(!SpawnPlacementPending);}if(SpawnPlacementPending)return;}
 if(TickThrown49(Dt))return;TickDamage(Dt);if(Record()&&Record()->Exploded){TickLuxury66(0);SyncRecord();if(P&&FVector::Dist2D(P->GetActorLocation(),GetActorLocation())>(World->RenderRadius+1)*LWGen::ChunkSize)Destroy();return;}TickTurret57(Dt);if(TickFlight57(Dt))return;TickConvoy(Dt);RecoverTerrain(Dt);TickCamperEntry(Dt);TickCabinWalk(Dt);TickAutopilot(Dt);TickLuxury66(Dt);TickFuel(Dt);
 bool Input=Driver&&PlayerSeat==-1&&!SelfDriving74&&!Driver->bInventory&&!Driver->bMap&&!Driver->RPGPanel&&!Driver->SecurityMode&&Driver->Health>0;
 auto* PC=Driver?Cast<APlayerController>(Driver->Controller):nullptr;bool Brake=PendingIgnition66||SlideAlpha66>.001f||AwningAlpha66>.001f||(!Input&&!AutoDriving)||StopRequested||Boarding||(PC&&PlayerSeat==-1&&!SelfDriving74&&PC->IsInputKeyDown(EKeys::SpaceBar));
 float Gas=(Input||AutoDriving)&&EngineOn?Throttle:0;bool Opposing=Gas*Speed<-30;
 const auto& S=Spec();auto Mod=[&](FName Id){return Record()?LWGarage45::Stat(Record(),Id):0.f;};const float TopSpeed=S.MaxSpeed*(1+Mod(TEXT("speed")));const bool Sport=FeatureOn&&(FName(S.Id)==TEXT("muscle")||FName(S.Id)==TEXT("supercar"));
 const bool Traction=FeatureOn&&FName(S.Id)==TEXT("suv");
 const bool OpenGate=FeatureOn&&(FName(S.Feature)==TEXT("ramp")||FName(S.Feature)==TEXT("door")||FName(S.Feature)==TEXT("stand"));
 if(OpenGate)Brake=true;
 float Limit=FMath::Lerp(S.Steering,10.f,FMath::Clamp(FMath::Abs(Speed)/S.MaxSpeed,0.f,1.f));
 SteeringAngle=FMath::FInterpConstantTo(SteeringAngle,Steer*Limit,Dt*(1+Mod(TEXT("grip"))),FMath::Abs(Steer)>.01?75.f:110.f);
 if(Brake||Opposing)Speed=FMath::FInterpConstantTo(Speed,0,Dt,S.Brake*(1+Mod(TEXT("brake"))));else if(FMath::Abs(Gas)>.05){float Power=S.Acceleration*(1+Mod(TEXT("power")))*(Record()?FMath::Clamp(Record()->Health/200.f,.25f,1.f):1.f)*(Sport?1.2f:1.f)*(1-.65f*FMath::Clamp(FMath::Abs(Speed)/S.MaxSpeed,0.f,1.f));Speed=FMath::Clamp(Speed+Gas*Dt*Power,-S.Reverse,TopSpeed);}else Speed=FMath::FInterpConstantTo(Speed,0,Dt,45+Speed*Speed*.000018f);
 auto GripSpec=S;GripSpec.Grip*=1+Mod(TEXT("grip"));if(FMath::Abs(Speed)>1){FRotator R=GetActorRotation();R.Yaw+=FMath::RadiansToDegrees(LWTraffic::YawRate(GripSpec,Speed,SteeringAngle,Traction)*Dt);FVector At=GetActorLocation()+FRotator(0,R.Yaw,0).Vector()*Speed*Dt;FCollisionQueryParams Q(NAME_None,false,this);if(Driver)Q.AddIgnoredActor(Driver);
 FCollisionObjectQueryParams GroundTypes;GroundTypes.AddObjectTypesToQuery(ECC_WorldStatic);GroundTypes.AddObjectTypesToQuery(ECC_WorldDynamic);
 float Z[4];bool Ground=true;for(int I=0;I<4;I++){FVector Off=FRotator(0,R.Yaw,0).RotateVector(FVector(LWTraffic::FrontOffset(S)+132-(I<2?0:S.Wheelbase),I%2?S.HalfWidth:-S.HalfWidth,0));FHitResult H;if(GetWorld()->LineTraceSingleByObjectType(H,At+Off+FVector(0,0,600),At+Off-FVector(0,0,FMath::Max(900.f,S.Wheelbase)),GroundTypes,Q)&&H.ImpactNormal.Z>.65f)Z[I]=H.ImpactPoint.Z;else Ground=false;}
 if(Ground){float Height=(Z[0]+Z[1]+Z[2]+Z[3])*.25f;R.Pitch=FMath::FInterpTo(R.Pitch,FMath::RadiansToDegrees(FMath::Atan2((Z[0]+Z[1]-Z[2]-Z[3])*.5,S.Wheelbase)),Dt,7);R.Roll=FMath::FInterpTo(R.Roll,FMath::RadiansToDegrees(FMath::Atan2((Z[0]+Z[2]-Z[1]-Z[3])*.5,S.HalfWidth*2)),Dt,7);
 // The wheel midpoint is ahead of the chassis origin on long coaches.
 // Fit height at the chassis origin, then keep the low overhang clear of terrain.
 const float AxleMid=LWTraffic::FrontOffset(S)+132-S.Wheelbase*.5f;
 At.Z=Height+75.f+Mod(TEXT("clearance"))-R.RotateVector(FVector(AxleMid,0,0)).Z;
 for(float X:{-S.HalfLength,0.f,S.HalfLength})for(float Y:{-S.HalfWidth,S.HalfWidth}){
  const FVector Bottom=R.RotateVector(FVector(X,Y,-Chassis->GetUnscaledBoxExtent().Z));
  const FVector Sample=At+Bottom;FHitResult Support;
  if(GetWorld()->LineTraceSingleByChannel(Support,Sample+FVector(0,0,240),Sample-FVector(0,0,350),ECC_Visibility,Q)){
   const auto* Chunk=Cast<ALWChunk>(Support.GetActor());
   // Prop tops must not lift the car over walls, wrecks or barricades.
   if(Chunk&&(Support.GetComponent()==Chunk->Terrain||Support.GetComponent()->ComponentHasTag(TEXT("RoadSurface")))&&Support.ImpactNormal.Z>.65f)
    At.Z=FMath::Max(At.Z,Support.ImpactPoint.Z-Bottom.Z+5.f);
  }
 }
 HitPedestrians(At,R);FHitResult Hit;SetActorLocationAndRotation(At,R,true,&Hit);if(Hit.bBlockingHit&&!(Cast<ALWChunk>(Hit.GetActor())&&Hit.GetComponent()&&(Hit.GetComponent()==Cast<ALWChunk>(Hit.GetActor())->Terrain||Hit.GetComponent()->ComponentHasTag(TEXT("RoadSurface")))&&TryIncline45(At,R))){if(FMath::Abs(Speed)>800&&Driver)UGameplayStatics::ApplyDamage(Driver,FMath::Abs(Speed)*.012f,nullptr,this,nullptr);if(FMath::Abs(Speed)>500)UGameplayStatics::ApplyPointDamage(this,FMath::Abs(Speed)*.035f,Hit.ImpactNormal,Hit,nullptr,this,nullptr);Speed=0;World->Sound(TEXT("CarImpact"),Hit.ImpactPoint,.7f);}}else {Speed=0;}
 }
 WheelSpin+=Speed*Dt/32*57.2958f;for(int I=0;I<Wheels.Num();I++)Wheels[I]->SetRelativeRotation(FRotator(WheelSpin,(I<4&&I%2)?SteeringAngle:0,0));SteeringWheel->SetRelativeRotation(FRotator(0,0,-Steer*90));
 GloveAngle=FMath::FInterpTo(GloveAngle,GloveOpen?65:0,Dt,6);GloveLid->SetRelativeRotation(FRotator(GloveAngle,0,0));
 for(auto& L:Headlamps)if(IsValid(L))L->SetVisibility(Headlights);for(int I=0;I<Wipers.Num();I++)Wipers[I]->SetRelativeRotation(FRotator(0,0,WipersOn?FMath::Sin(GetWorld()->GetTimeSeconds()*4)*40:0));
 float Old=SignalClock;SignalClock+=Dt;bool Blink=FMath::Fmod(SignalClock,1.f)<.5f;for(int I=0;I<Indicators.Num();I++)Indicators[I]->SetVisibility(Signal==(I==0?-1:1)&&Blink);if(Signal&&int(Old*2)!=int(SignalClock*2))VehicleSound42(TEXT("CarSignal"),.3f);
 TickFeatures(Dt);TickGlass(Dt);
 TickDriveAudio(Dt,Brake||Opposing,Gas);TickInstruments42(Dt);TickElectric74(Dt);
 SyncRecord();if(!Driver&&!ConvoyOwner&&!(Record()&&Record()->Stored45)&&P&&FVector::Dist2D(P->GetActorLocation(),GetActorLocation())>(World->RenderRadius+1)*LWGen::ChunkSize)Destroy();
}
void ALWVehicle::EndPlay(const EEndPlayReason::Type R){if(TurbineAudio84)TurbineAudio84->Stop();if(TVAudio66){TVAudio66->Stop();TVAudio66->DestroyComponent();}if(ServoAudio57){ServoAudio57->Stop();ServoAudio57->DestroyComponent();}StopDriveAudio();if(Driver)Exit(true);UnloadPassengers();SyncRecord();if(EngineAudio)EngineAudio->Stop();if(RadioAudio)RadioAudio->Stop();if(SirenAudio)SirenAudio->Stop();if(GloveObject)GloveObject->Destroy();if(CargoObject)CargoObject->Destroy();Super::EndPlay(R);}

