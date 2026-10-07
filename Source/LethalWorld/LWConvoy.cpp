#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "LWNavigation.h"
#include "Components/CapsuleComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

namespace LWConvoy {
// Called on the game thread from the rider's social tick. No partially vacant
// driver seat or on-foot intermediate state is exposed to the next crew tick.
bool RejoinPlayerVehicle(ALWResident* N,ALWCharacter* P){
 if(!IsValid(N)||!IsValid(P)||N->DownTime>0||P->IsUIOpen())return false;
 auto* From=N->Riding.Get();auto* To=P->Vehicle.Get();
 if(!IsValid(From)||!IsValid(To)||From==To||From->ConvoyOwner!=P||From->ConvoyLeader!=To||To->Driver!=P)return false;
 if(FMath::Abs(From->Speed)>.01f||FMath::Abs(To->Speed)>.01f||From->SpawnPlacementPending||!To->HasFreeCompanionSeat())return false;
 if(!From->Record()||From->Record()->Exploded||From->Record()->FireRemaining>0)return false;
 if(FVector::Dist2D(From->GetActorLocation(),To->GetActorLocation())>From->Spec().HalfLength+To->Spec().HalfLength+450||FMath::Abs(From->GetActorLocation().Z-To->GetActorLocation().Z)>150)return false;

 int32 Seat=INDEX_NONE;
 const bool BecomeDriver=!To->SelfDriving74&&!To->IsHelicopter57()&&!To->IsAircraft84()&&To->PlayerSeat>=0&&!IsValid(To->Chauffeur)&&!To->DriverDismissed&&P->bWaypoint;
 if(!BecomeDriver){for(int32 I=0;I<To->Spec().Seats-1;I++)if(I!=To->PlayerSeat&&(!To->Passengers.IsValidIndex(I)||!IsValid(To->Passengers[I]))){Seat=I;break;}if(Seat==INDEX_NONE)return false;}
 const bool WasDriver=From->Chauffeur==N;const int32 OldSeat=N->SeatIndex;
 if(!WasDriver&&(!From->Passengers.IsValidIndex(OldSeat)||From->Passengers[OldSeat]!=N))return false;
 ALWResident* Replacement=nullptr;int32 ReplacementSeat=INDEX_NONE;bool HasRemainingPassengers=false;
 if(WasDriver)for(int32 I=0;I<From->Passengers.Num();I++)if(IsValid(From->Passengers[I])){
  HasRemainingPassengers=true;auto* Candidate=From->Passengers[I].Get();
  if(!Replacement&&Candidate->Riding==From&&Candidate->DownTime<=0&&P->RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Following&&C.Id==Candidate->ResidentId;})){Replacement=Candidate;ReplacementSeat=I;}
 }
 // Keep the existing driver if the remaining crew cannot safely take the wheel.
 if(WasDriver&&HasRemainingPassengers&&!Replacement)return false;
 const FVector Destination=BecomeDriver?To->DriverEye()-FVector(0,0,83):To->SeatLocation(Seat);
 FCollisionQueryParams Q(NAME_None,false,From);Q.AddIgnoredActor(To);Q.AddIgnoredActor(P);Q.AddIgnoredActor(N);
 if(From->Chauffeur)Q.AddIgnoredActor(From->Chauffeur);if(To->Chauffeur)Q.AddIgnoredActor(To->Chauffeur);
 for(auto R:From->Passengers)if(R)Q.AddIgnoredActor(R);for(auto R:To->Passengers)if(R)Q.AddIgnoredActor(R);
 FHitResult Hit;if(From->GetWorld()->LineTraceSingleByChannel(Hit,N->GetActorLocation()+FVector(0,0,50),To->GetActorTransform().TransformPosition(Destination)+FVector(0,0,50),ECC_Visibility,Q))return false;

 if(WasDriver){
  From->Chauffeur=Replacement;
  if(Replacement){From->Passengers[ReplacementSeat]=nullptr;Replacement->SeatIndex=-1;Replacement->SetActorRelativeLocation(From->DriverEye()-FVector(0,0,83));Replacement->SetActorRelativeRotation(FRotator::ZeroRotator);From->DrivePath.Empty();From->RepathClock=0;}
  else{From->ConvoyOwner=nullptr;From->ConvoyLeader=nullptr;From->EngineOn=From->AutoDriving=From->Boarding=false;From->StopRequested=false;}
  From->Throttle=From->Steer=0;
 }else From->Passengers[OldSeat]=nullptr;
 // Reparent directly, as normal boarding does: no LeaveVehicle terrain teleport,
 // moving-vehicle exit, or frame with an unreserved destination seat.
 N->ResetCompanionNavigation();N->Riding=To;N->SeatIndex=BecomeDriver?-1:Seat;
 if(BecomeDriver){To->Chauffeur=N;To->Boarding=true;To->BoardingClock=0;}
 else{if(To->Passengers.Num()!=To->Spec().Seats-1)To->Passengers.SetNum(To->Spec().Seats-1);To->Passengers[Seat]=N;}
 N->AttachToComponent(To->Chassis,FAttachmentTransformRules::KeepWorldTransform);N->SetActorRelativeLocation(Destination);N->SetActorRelativeRotation(FRotator::ZeroRotator);
 N->GetCharacterMovement()->StopMovementImmediately();N->GetCharacterMovement()->DisableMovement();N->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);N->FollowTimer=N->LeashTime=0;
 return true;
}
}

bool ALWVehicle::HasFreeCompanionSeat()const{
 if(SpawnPlacementPending||!Record()||Record()->Exploded||Record()->Health<=0||Record()->FireRemaining>0)return false;
 if(!IsAircraft84()&&!IsHelicopter57()&&!SelfDriving74&&Driver&&PlayerSeat>=0&&!IsValid(Chauffeur)&&!DriverDismissed&&Driver->bWaypoint)return true;
 for(int I=0;I<Spec().Seats-1;I++)if(I!=PlayerSeat&&(!Passengers.IsValidIndex(I)||!IsValid(Passengers[I])))return true;
 return false;
}

bool ALWVehicle::BoardConvoy(ALWResident* N,ALWCharacter* P){
 if(IsAircraft84()||IsHelicopter57()||!N||!P||!P->Vehicle||P->Vehicle->IsAircraft84()||SpawnPlacementPending||!HasFuel()||P->Vehicle==this||Driver||N->Riding||N->DownTime>0||!Record()||Record()->Exploded||Record()->FireRemaining>0||Record()->Health<=0||FMath::Abs(Speed)>100||FVector::Dist2D(N->GetActorLocation(),GetActorLocation())>Spec().HalfLength+350)return false;
 if(P->Vehicle->HasFreeCompanionSeat()){P->Vehicle->Board(N);return N->Riding==P->Vehicle;}
 if(ConvoyOwner&&ConvoyOwner!=P)return false;ConvoyOwner=P;ConvoyLeader=P->Vehicle;DriverDismissed=false;
 if(Chauffeur)return Board(N);
 Chauffeur=N;PlayerSeat=-2;N->Riding=this;N->SeatIndex=-1;N->GetCharacterMovement()->StopMovementImmediately();N->GetCharacterMovement()->DisableMovement();N->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);N->AttachToComponent(Chassis,FAttachmentTransformRules::KeepWorldTransform);N->SetActorRelativeLocation(DriverEye()-FVector(0,0,83));N->SetActorRelativeRotation(FRotator::ZeroRotator);if(N->Gun)N->Gun->SetVisibility(false);for(int I:{5,6})N->Parts[I]->SetRelativeRotation(FRotator(65,0,0));ConvoyStartDelay=0;Record()->Unlocked=Record()->Hotwired=true;EngineOn=true;AutoDriving=true;StopRequested=Boarding=false;BoardingClock=0;RepathClock=0;DrivePath.Empty();for(auto& C:World->Chunks)C.Value->Residents.Remove(this);return true;
}
void ALWVehicle::TickConvoy(float Dt){
 if(!IsValid(ConvoyOwner)){ConvoyOwner=nullptr;return;}
 if(!IsValid(ConvoyLeader)||ConvoyOwner->Vehicle!=ConvoyLeader||!IsValid(Chauffeur)||Chauffeur->DownTime>0||!Record()||Record()->Health<=0||!HasFuel()){Throttle=Steer=0;AutoDriving=false;if(FMath::Abs(Speed)<5){UnloadPassengers();ConvoyOwner=nullptr;ConvoyLeader=nullptr;EngineOn=false;}return;}
 StuckClock=FVector::Dist2D(GetActorLocation(),ConvoyLeader->GetActorLocation())>12000?StuckClock+Dt:0;
 if(StuckClock>=8){auto* SquadPlayer=ConvoyOwner.Get();TArray<FName> Crew;if(IsValid(Chauffeur))Crew.Add(Chauffeur->ResidentId);for(auto& N:Passengers)if(IsValid(N))Crew.Add(N->ResidentId);UnloadPassengers();Throttle=Steer=Speed=0;EngineOn=AutoDriving=false;ConvoyOwner=nullptr;ConvoyLeader=nullptr;for(FName Id:Crew)SquadPlayer->SetCompanion(Id,false);SquadPlayer->Notify(TEXT("CONVOY LOST CONTACT / CREW RETURNING HOME"));SyncRecord();return;}
 AutoDriving=true;BoardingClock+=Dt;if(BoardingClock<ConvoyStartDelay||ConvoyOwner->IsUIOpen()){Throttle=0;return;}
 if(!EngineOn){Record()->Hotwired=true;EngineOn=true;SyncRecord();}
 const FVector Goal=ConvoyLeader->GetActorLocation()-ConvoyLeader->GetActorForwardVector()*(ConvoyLeader->Spec().HalfLength+Spec().HalfLength+320);
 float Distance=FVector::Dist2D(GetActorLocation(),Goal);if(Distance<250){Throttle=0;Steer=0;Speed=FMath::FInterpConstantTo(Speed,0,Dt,Spec().Brake);return;}
 RepathClock-=Dt;if(RepathClock<=0){FCollisionQueryParams DirectQ(NAME_None,false,this);DirectQ.AddIgnoredActor(ConvoyLeader);DirectQ.AddIgnoredActor(ConvoyOwner);DirectQ.AddIgnoredActor(Chauffeur);for(auto& N:Passengers)if(N)DirectQ.AddIgnoredActor(N);FHitResult DirectHit;
 if(Distance<3500&&!GetWorld()->SweepSingleByChannel(DirectHit,GetActorLocation(),Goal,GetActorQuat(),ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(25,Spec().HalfWidth+15,35)),DirectQ)){DrivePath={FVector2D(GetActorLocation()),FVector2D(Goal)};DriveComplete=true;}else DrivePath=LWNavigation::FindPath(FVector2D(GetActorLocation()),FVector2D(Goal),World->Seed,&DriveComplete);DriveStep=1;RepathClock=2;}
 if(DrivePath.Num()<2){Throttle=0;return;}
 while(DriveStep<DrivePath.Num()-1&&FVector2D::Distance(FVector2D(GetActorLocation()),DrivePath[DriveStep])<FMath::Max(250.f,FMath::Abs(Speed)*.5f))DriveStep++;
 FVector Target(DrivePath[FMath::Clamp(DriveStep,0,DrivePath.Num()-1)],GetActorLocation().Z);FVector Local=GetActorTransform().InverseTransformPosition(Target);float Angle=FMath::Atan2(Local.Y,Local.X);float Limit=FMath::Lerp(Spec().Steering,10.f,FMath::Clamp(FMath::Abs(Speed)/Spec().MaxSpeed,0.f,1.f));Steer=FMath::Clamp(FMath::RadiansToDegrees(FMath::Atan2(2*Spec().Wheelbase*FMath::Sin(Angle),FMath::Max(250.,Local.Size2D())))/Limit,-1.f,1.f);
 float Desired=FMath::Min(Spec().MaxSpeed*.62f,FMath::Max(350.f,FMath::Abs(ConvoyLeader->Speed)+Distance*.12f));Desired*=FMath::Clamp(1.f-FMath::Abs(Angle),.15f,1.f);Desired=FMath::Min(Desired,FMath::Sqrt(2*Spec().Brake*FMath::Max(0.f,Distance-200)));
 FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(Chauffeur);for(auto& N:Passengers)if(N)Q.AddIgnoredActor(N);FHitResult H;FVector Start=GetActorLocation()+GetActorForwardVector()*(Spec().HalfLength+15);
 if(GetWorld()->SweepSingleByChannel(H,Start,Start+GetActorForwardVector()*FMath::Max(350.f,Speed*Speed/(2*Spec().Brake)+250),GetActorQuat(),ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(20,Spec().HalfWidth+12,40)),Q))Desired=FMath::Min(Desired,FMath::Sqrt(2*Spec().Brake*FMath::Max(0.f,H.Distance-130)));
 Desired=LWRoads33::TrafficSpeed(this,Desired,Dt);
 Throttle=Speed<Desired-40?1:0;if(Speed>Desired)Speed=FMath::FInterpConstantTo(Speed,Desired,Dt,Spec().Brake);
}

