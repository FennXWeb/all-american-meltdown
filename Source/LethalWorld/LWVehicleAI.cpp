#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "LWNavigation.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
namespace { struct FAvoidance {float Offset=0,Hold=0;};
FAvoidance& AvoidState(ALWVehicle* C){static TMap<TWeakObjectPtr<ALWVehicle>,FAvoidance> States;for(auto I=States.CreateIterator();I;++I)if(!I.Key().IsValid())I.RemoveCurrent();return States.FindOrAdd(C);}}
FVector ALWVehicle::PlayerEye()const{return PlayerSeat==-2?CabinEye:PlayerSeat<0?DriverEye():SeatLocation(PlayerSeat)+FVector(0,0,83);}
int ALWVehicle::NearestSeat(const ALWCharacter* P)const{int Seat=-1;double Best=DBL_MAX;for(int I=-1;I<Spec().Seats-1;I++){if(I>=0&&Passengers.IsValidIndex(I)&&IsValid(Passengers[I]))continue;FVector At=GetActorTransform().TransformPosition(I<0?DriverEye():SeatLocation(I));double D=FVector::DistSquared2D(P->GetActorLocation(),At);if(D<Best){Seat=I;Best=D;}}return Seat;}
bool ALWVehicle::ChangeSeat(int Seat){if(!Driver||Driver->bMenu||Driver->bInventory||Driver->SecurityMode||Seat < -1||Seat>=Spec().Seats-1)return false;if(FMath::Abs(Speed)>5||Chauffeur){Driver->Notify(TEXT("STOP AND DISMISS THE DRIVER BEFORE CHANGING SEATS"));return false;}if(Seat>=0&&Passengers.IsValidIndex(Seat)&&IsValid(Passengers[Seat])){Driver->Notify(TEXT("SEAT OCCUPIED"));return false;}PlayerSeat=Seat;VehicleSound42(TEXT("CarSeat"),.5f);if(Seat==-1)MarkLastDriven();DriverDismissed=false;Throttle=Steer=0;Driver->TickVehicleSeat();Driver->Notify(Seat<0?TEXT("DRIVER SEAT"):FString::Printf(TEXT("PASSENGER SEAT %d"),Seat+1));return true;}
void ALWVehicle::RequestDriverStop(){StopRequested=true;Throttle=0;if(Driver)Driver->Notify(TEXT("PULLING TO A STOP"));}
void ALWVehicle::TickAutopilot(float Dt){
 if(!Driver)return;
 if(PlayerSeat<0){AutoDriving=Boarding=false;return;}
 if(!IsValid(Chauffeur)){
 Chauffeur=nullptr;
 AutoDriving=Boarding=false;Throttle=Steer=0;
 if(!DriverDismissed&&Driver->bWaypoint)for(TActorIterator<ALWResident> I(GetWorld());I;++I)if(Driver->RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Following&&C.Id==I->ResidentId;})){Board(*I);if(Chauffeur)break;}
 if(!Chauffeur)return;
 }
 if(Chauffeur->DownTime>0||!Driver->bWaypoint||PlayerSeat<0||Record()->Health<=0||!HasFuel())StopRequested=true;
 if(StopRequested){Throttle=0;Steer=0;if(FMath::Abs(Speed)<5){Speed=0;DriverDismissed=true;AutoDriving=Boarding=false;StopRequested=false;Chauffeur->LeaveVehicle();Driver->Notify(TEXT("DRIVER EXITED // F TO EXIT"));}return;}
 if(Boarding){BoardingClock+=Dt;bool Waiting=false;int Free=0;for(int I=0;I<Spec().Seats-1;I++)if(I!=PlayerSeat&&(!Passengers.IsValidIndex(I)||!IsValid(Passengers[I])))Free++;
 for(TActorIterator<ALWResident> I(GetWorld());I;++I){if(I->Riding||I->DownTime>0||!Driver->RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Following&&C.Id==I->ResidentId;}))continue;if(Board(*I)){Free--;continue;}if(Free>0){Waiting=true;Free--;}}
 if(Waiting){if(BoardingClock>25){Driver->Notify(TEXT("WAITING FOR THE CREW // F TO CANCEL"));BoardingClock=0;}return;}
 if(!Record()->Hotwired&&!Driver->HasKey(Record()->VIN)){Driver->Notify(TEXT("KEY OR HOTWIRE REQUIRED // TAKE DRIVER SEAT TO HOTWIRE"));DriverDismissed=true;Chauffeur->LeaveVehicle();Boarding=false;return;}
 Boarding=false;AutoDriving=true;EngineOn=true;DrivePath.Empty();RepathClock=0;RecoveryAttempts=0;Driver->Notify(TEXT("CREW ABOARD // HEADING TO WAYPOINT"));
 }
 if(!AutoDriving)return;
 if(Driver->bMenu||Driver->SecurityMode){Throttle=0;return;}
 const FVector2D Here(GetActorLocation());RepathClock-=Dt;
 if(DrivePath.Num()<2||!DriveGoal.Equals(Driver->Waypoint,1)||DriveStep>=DrivePath.Num()){
 DriveGoal=Driver->Waypoint;DrivePath=LWNavigation::FindPath(Here,DriveGoal,World->Seed,&DriveComplete);DriveStep=1;RepathClock=3;
 if(DrivePath.Num()<2){RequestDriverStop();Driver->Notify(TEXT("NO DRIVABLE ROUTE"));return;}}
 const float Arrival=FVector2D::Distance(Here,DriveGoal);if(Arrival<250){RequestDriverStop();Driver->Notify(TEXT("ARRIVED AT WAYPOINT"));return;}
 while(DriveStep<DrivePath.Num()-1&&FVector2D::Distance(Here,DrivePath[DriveStep])<FMath::Max(180.f,FMath::Abs(Speed)*.3f))DriveStep++;
 if(DriveStep==DrivePath.Num()-1&&FVector2D::Distance(Here,DrivePath.Last())<230&&!DriveComplete){DrivePath.Empty();Throttle=0;return;}
 FVector2D A=DrivePath[DriveStep-1],B=DrivePath[DriveStep],D=(B-A).GetSafeNormal();double Along=FVector2D::DotProduct(Here-A,D);double Length=FVector2D::Distance(A,B);double Look=FMath::Clamp(350.+FMath::Abs(Speed)*.6,350.,1100.);
 FVector2D Target=A+D*FMath::Clamp(Along+Look,0.,Length);FVector2D Right(-D.Y,D.X);
 // Lane offset fades at destination and junctions; never cuts across the next block.
 auto& Avoid=AvoidState(this);Avoid.Hold=FMath::Max(0.f,Avoid.Hold-Dt);if(Avoid.Hold<=0)Avoid.Offset=FMath::FInterpTo(Avoid.Offset,0,Dt,1.2f);
 if(Length>1100&&FVector2D::Distance(Here,B)>600)Target+=Right*(140+Avoid.Offset);
 FVector Local=GetActorTransform().InverseTransformPosition(FVector(Target,GetActorLocation().Z));float Angle=FMath::Atan2(Local.Y,Local.X);float Wheel=FMath::RadiansToDegrees(FMath::Atan2(2*Spec().Wheelbase*FMath::Sin(Angle),FMath::Max(200.,Local.Size2D())));
 float Limit=FMath::Lerp(Spec().Steering,10.f,FMath::Clamp(FMath::Abs(Speed)/Spec().MaxSpeed,0.f,1.f));Steer=FMath::Abs(Angle)>2.f?FMath::Sign(Angle):FMath::Clamp(Wheel/Limit,-1.f,1.f);
 float Desired=FMath::Min(Spec().MaxSpeed*.58f,1500.f);Desired*=FMath::Clamp(1.f-FMath::Abs(Angle)*.8f,.18f,1.f);
 if(DriveStep+1<DrivePath.Num()){FVector2D Next=(DrivePath[DriveStep+1]-B).GetSafeNormal();float Turn=FMath::Acos(FMath::Clamp(FVector2D::DotProduct(D,Next),-1.,1.));float CornerSpeed=FMath::Lerp(Desired,260.f,FMath::Clamp(Turn/1.4f,0.f,1.f));Desired=FMath::Min(Desired,FMath::Sqrt(CornerSpeed*CornerSpeed+2*Spec().Brake*.55f*FMath::Max(0.,FVector2D::Distance(Here,B)-300)));}
 Desired=FMath::Min(Desired,FMath::Sqrt(2*Spec().Brake*.55f*FMath::Max(0.f,Arrival-210)));
 FCollisionQueryParams Q(NAME_None,false,this);Q.AddIgnoredActor(Driver);Q.AddIgnoredActor(Chauffeur);for(auto& N:Passengers)if(N)Q.AddIgnoredActor(N);
 float StopDistance=FMath::Max(280.f,Speed*Speed/(2*Spec().Brake)+FMath::Abs(Speed)*.55f);
 FVector Origin=GetActorLocation()+GetActorForwardVector()*(Spec().HalfLength+20);FHitResult H;
 bool Blocked=GetWorld()->SweepSingleByChannel(H,Origin,Origin+GetActorForwardVector()*StopDistance,GetActorQuat(),ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(20,Spec().HalfWidth+16,35)),Q);
 if(Blocked&&!Cast<APawn>(H.GetActor())&&H.Distance>80&&FMath::Abs(Angle)<.55f){
 // Only pass stationary obstructions where the full vehicle remains on a wide road.
 TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(Here,World->Seed,Roads,Sites);
 for(float Offset:{280.f,-280.f,450.f,-450.f}){
 FVector2D Candidate=Here+D*FMath::Max(700.,Look)+Right*Offset;bool OnRoad=false;
 for(const auto& Road:Roads)if(Road.Width>=780&&LWGen::DistanceToSegment(Candidate,Road)<Road.Width*.5-Spec().HalfWidth-50){OnRoad=true;break;}if(!OnRoad)continue;
 FHitResult SideHit;FVector End(Candidate,Origin.Z);
 if(GetWorld()->SweepSingleByChannel(SideHit,Origin,End,GetActorQuat(),ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(45,Spec().HalfWidth+24,35)),Q))continue;
 // Check beyond the passing point so a short clear pocket cannot lure the car into another wreck.
 if(GetWorld()->SweepSingleByChannel(SideHit,End,End+FVector(D,0)*(Spec().HalfLength*2+300),GetActorQuat(),ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(45,Spec().HalfWidth+24,35)),Q))continue;
 Avoid.Offset=Offset;Avoid.Hold=3;FVector To=GetActorTransform().InverseTransformPosition(End);float Steering=FMath::RadiansToDegrees(FMath::Atan2(2*Spec().Wheelbase*FMath::Sin(FMath::Atan2(To.Y,To.X)),FMath::Max(200.,To.Size2D())));Steer=FMath::Clamp(Steering/Limit,-1.f,1.f);Desired=FMath::Min(Desired,350.f);Blocked=false;break;
 }
 }
 Desired=LWRoads33::TrafficSpeed(this,Desired,Dt);
 if(Blocked){Desired=FMath::Min(Desired,FMath::Sqrt(2*Spec().Brake*.5f*FMath::Max(0.f,H.Distance-180)));if(H.Distance<220)Desired=0;}
 StuckClock=Blocked&&FMath::Abs(Speed)<25?StuckClock+Dt:0;
 if(StuckClock>4){StuckClock=0;if(++RecoveryAttempts>3){RequestDriverStop();Driver->Notify(TEXT("ROAD BLOCKED // DRIVER STOPPING"));return;}ReverseClock=1.6f;}
 if(ReverseClock>0){ReverseClock-=Dt;FVector Back=GetActorLocation()-GetActorForwardVector()*(Spec().HalfLength+20);if(!GetWorld()->SweepSingleByChannel(H,Back,Back-GetActorForwardVector()*280,GetActorQuat(),ECC_WorldDynamic,FCollisionShape::MakeBox(FVector(20,Spec().HalfWidth+16,35)),Q)){Throttle=-.5f;Steer=-Steer;}else{ReverseClock=0;Throttle=0;}if(ReverseClock<=0)DrivePath.Empty();return;}
 if(FMath::Abs(Along)>Length+2500&&RepathClock<=0){DrivePath.Empty();Throttle=0;return;}
 Throttle=Speed>Desired+35?-1.f:Speed<Desired-35?FMath::Clamp((Desired-Speed)/300.f,.15f,1.f):0.f;
 Signal=FMath::Abs(Angle)>.3f?(Angle<0?-1:1):0;
}
