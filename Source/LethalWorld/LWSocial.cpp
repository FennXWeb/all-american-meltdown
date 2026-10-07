#include "LWResident.h"
#include "LWBunker45.h"
#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "EngineUtils.h"
namespace LWConvoy {bool RejoinPlayerVehicle(ALWResident* N,ALWCharacter* P);}
bool ALWVehicle::Board(ALWResident* N){if(IsAircraft84()&&(!AircraftOnGround84||Speed>50))return false;
 if(IsHelicopter57()&&Altitude57>120)return false;
 if(SpawnPlacementPending||(!Driver&&!ConvoyOwner)||!N||N->Riding||N->DownTime>0||FMath::Abs(Speed)>250||FVector::Dist2D(N->GetActorLocation(),GetActorLocation())>Spec().HalfLength+220)return false;
 if(!IsHelicopter57()&&!IsAircraft84()&&!DriverDismissed&&!SelfDriving74&&Driver&&PlayerSeat>=0&&!Chauffeur&&Driver->bWaypoint){Chauffeur=N;N->Riding=this;N->SeatIndex=-1;N->GetCharacterMovement()->StopMovementImmediately();N->GetCharacterMovement()->DisableMovement();N->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);N->AttachToComponent(Chassis,FAttachmentTransformRules::KeepWorldTransform);N->SetActorRelativeLocation(DriverEye()-FVector(0,0,83));N->SetActorRelativeRotation(FRotator::ZeroRotator);if(N->Gun)N->Gun->SetVisibility(false);N->Parts[5]->SetRelativeRotation(FRotator(65,0,0));N->Parts[6]->SetRelativeRotation(FRotator(65,0,0));Boarding=true;BoardingClock=0;return true;}
 const int Seats=Spec().Seats-1;if(Passengers.Num()!=Seats)Passengers.SetNum(Seats);for(int I=0;I<Seats;I++)if(I!=PlayerSeat&&!IsValid(Passengers[I])){
 Passengers[I]=N;N->Riding=this;N->SeatIndex=I;N->GetCharacterMovement()->StopMovementImmediately();N->GetCharacterMovement()->DisableMovement();N->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);N->AttachToComponent(Chassis,FAttachmentTransformRules::KeepWorldTransform);N->SetActorRelativeLocation(SeatLocation(I));N->SetActorRelativeRotation(FRotator::ZeroRotator);if(N->Gun)N->Gun->SetVisibility(false);N->Parts[5]->SetRelativeRotation(FRotator(65,0,0));N->Parts[6]->SetRelativeRotation(FRotator(65,0,0));return true;}return false;
}
void ALWVehicle::UnloadPassengers(){if(IsValid(Chauffeur))Chauffeur->LeaveVehicle();auto Copy=Passengers;for(auto& N:Copy)if(IsValid(N))N->LeaveVehicle();}
void ALWResident::LeaveVehicle(){ResetCompanionNavigation();if(!Riding)return;auto* Car=Riding.Get();const int Seat=SeatIndex;if(Car->Chauffeur==this){Car->Chauffeur=nullptr;Car->AutoDriving=Car->Boarding=false;Car->Throttle=Car->Steer=0;}DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);if(Car->Passengers.IsValidIndex(Seat)&&Car->Passengers[Seat]==this)Car->Passengers[Seat]=nullptr;Riding=nullptr;SeatIndex=-1;FVector At=Car->GetActorLocation()+Car->GetActorRightVector()*(Seat==1?-220:220)+Car->GetActorForwardVector()*(-100-Seat*85);if(Car->IsAircraft84())At=Car->GetActorTransform().TransformPosition(FVector(Car->Spec().HalfLength-490,Car->Spec().HalfWidth+180,0));At.Z=World->HeightAt(FVector2D(At))+100;FRotator Rot=Car->GetActorRotation();if(!GetWorld()->FindTeleportSpot(this,At,Rot))At=Car->GetActorLocation()-Car->GetActorForwardVector()*(400+Seat*100)+FVector(0,0,120);SetActorLocation(At,false,nullptr,ETeleportType::TeleportPhysics);GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);GetCharacterMovement()->SetMovementMode(MOVE_Walking);for(int I=3;I<7;I++)Parts[I]->SetRelativeRotation(FRotator::ZeroRotator);if(Gun)Gun->SetVisibility(NpcRole==TEXT("recruit"));FollowTimer=0;}
void ALWResident::TickSocial(float Dt,ALWCharacter* P,bool Following,int32 Bedroom){
 if(Riding){if(!Following||(P->Vehicle!=Riding&&Riding->ConvoyLeader!=P->Vehicle)||!P->Vehicle)LeaveVehicle();else{if(Riding!=P->Vehicle)LWConvoy::RejoinPlayerVehicle(this,P);return;}}
 FVector Target=Home;float Speed=190;
 if(Bedroom>=0){
 if(!Following){const auto* Crew=P->RPG.Crew.FindByPredicate([&](const auto& C){return C.Id==ResidentId;});const auto* Town=Crew&&!Crew->Station.IsNone()?P->RPG.Settlements.Find(Crew->Station):nullptr;Target=Town?Town->Center+FVector(Bedroom*85,700,0):World->BedroomPosition(Bedroom);if(!Town&&P->BunkerManager45)if(const auto* Job=P->Bunker45.Assignments.Find(ResidentId))if(P->Bunker45.Utilities.Contains(*Job))Target=P->BunkerManager45->UtilityPosition(*Job);if(Town?FVector::Dist2D(GetActorLocation(),Target)>1200:(!ALWWorld::IsSafePosition(GetActorLocation())||FMath::Abs(GetActorLocation().Z-Target.Z)>350))SetActorLocation(Target,false,nullptr,ETeleportType::TeleportPhysics);}
 else{
 float Distance=FVector::Dist2D(P->GetActorLocation(),GetActorLocation());
 if(P->Vehicle){if(P->Vehicle->Board(this))return;for(TActorIterator<ALWVehicle> V(GetWorld());V;++V)if(*V!=P->Vehicle&&V->BoardConvoy(this,P))return;Target=P->Vehicle->GetActorLocation()-P->Vehicle->GetActorForwardVector()*350;Speed=620;
 ALWVehicle* Candidate=nullptr;float Best=3000;for(TActorIterator<ALWVehicle> V(GetWorld());V;++V){if(V->IsAircraft84()||V->IsHelicopter57()||P->Vehicle->HasFreeCompanionSeat()||*V==P->Vehicle||V->SpawnPlacementPending||!V->HasFuel()||V->Driver||!V->Record()||V->Record()->Exploded||V->Record()->Health<=45||(V->ConvoyOwner&&V->ConvoyOwner!=P)||FMath::Abs(V->Speed)>80)continue;int Free=V->Chauffeur?0:1;for(int I=0;I<V->Spec().Seats-1;I++)Free+=!V->Passengers.IsValidIndex(I)||!IsValid(V->Passengers[I]);float D=FVector::Dist2D(GetActorLocation(),V->GetActorLocation());if(Free>0&&D<Best){Best=D;Candidate=*V;}}
 if(Candidate)Target=Candidate->GetActorLocation()+Candidate->GetActorRightVector()*(Candidate->Spec().HalfWidth+110);
}
 else{LeashTime=0;FollowTimer-=Dt;if(FollowTimer<=0||FVector::Dist2D(FollowTarget,P->GetActorLocation())>600){FollowTimer=FMath::FRandRange(2.5f,5.f);float Angle=FMath::FRandRange(-PI,PI),Radius=FMath::FRandRange(220.f,420.f);FollowTarget=P->GetActorLocation()+FVector(FMath::Cos(Angle)*Radius,FMath::Sin(Angle)*Radius,0);}Target=FollowTarget;Speed=Distance>800?530:330;
 if(!P->bSafehouse&&ALWWorld::IsSafePosition(GetActorLocation())){FVector Spot=Target+FVector(0,0,20);FRotator R=GetActorRotation();if(GetWorld()->FindTeleportSpot(this,Spot,R))SetActorLocation(Spot,false,nullptr,ETeleportType::TeleportPhysics);}}
 }}else if(ActivitySpots.Num()){
 ActivityTime-=Dt;if(ActivityTime<=0){Activity=CommunityRoutine84?(World->TimeOfDay<6||World->TimeOfDay>=23?0:World->TimeOfDay>=18?3: FMath::RandRange(0,3)?1:2):FMath::RandRange(0,ActivitySpots.Num()-1);ActivityTime=FMath::FRandRange(18.f,42.f);}Target=ActivitySpots[Activity];
 }
 FVector Delta=Target-GetActorLocation();Delta.Z=0;
 if(NavigateCompanion(Target,Speed,Dt)){
 float G=GetWorld()->GetTimeSeconds()*8;Parts[0]->SetRelativeRotation(FRotator::ZeroRotator);for(int I=3;I<7;I++)Parts[I]->SetRelativeRotation(FRotator(FMath::Sin(G+(I%2)*PI)*(I<5?12:25),0,0));
 }else{GetCharacterMovement()->StopMovementImmediately();for(int I=3;I<7;I++)Parts[I]->SetRelativeRotation(FRotator::ZeroRotator);if(Bedroom<0&&Activity>1){SetActorRotation(FRotator(0,Activity==3?90:-90,0));float T=GetWorld()->GetTimeSeconds();Parts[0]->SetRelativeRotation(FRotator(Activity==2?12:0,0,0));Parts[3]->SetRelativeRotation(FRotator(-35+FMath::Sin(T*3)*8,0,0));Parts[4]->SetRelativeRotation(FRotator(-40,0,0));}else Parts[0]->SetRelativeRotation(FRotator::ZeroRotator);}
}
