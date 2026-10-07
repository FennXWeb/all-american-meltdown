#include "LWAircraft84.h"
#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "LWCanada68.h"
#include "LWGeography84.h"
#include "LWPlayerInput51.h"
#include "Components/BoxComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/DamageEvents.h"
#include "InputCoreTypes.h"
using LWAviation84::EPhase;
namespace {
// PlayerInput stores translated logical keys for keyboard and controller input.
// Translating back to the physical binding here breaks remapped thrust and rudder.
bool Held(ALWCharacter* P,FKey Key){auto* PC=P?Cast<APlayerController>(P->Controller):nullptr;return PC&&PC->IsInputKeyDown(Key);}
float HeadingError(float Current,FVector Delta){return FMath::FindDeltaAngleDegrees(Current,Delta.Rotation().Yaw);}
bool OnStrip(FVector At,const LWAviation84::FRunway& R,float Margin=0){FVector D=R.Direction(),Q=At-R.A;const float Along=FVector::DotProduct(Q,D);return Along>-Margin&&Along<R.Length()+Margin&&FMath::Abs(Q.X*D.Y-Q.Y*D.X)<R.Width*.5+Margin;}
}
void ALWVehicle::ToggleAutopilot84(){auto* R=Record();if(!Driver||!R||R->Exploded)return;
 if(R->Autopilot84){if(PlayerSeat!=-1){Driver->Notify(TEXT("RETURN TO THE PILOT SEAT TO DISENGAGE AUTOPILOT"));return;}R->Autopilot84=false;Driver->Notify(TEXT("MANUAL FLIGHT"));return;}
 if(!HasFuel()){Driver->Notify(TEXT("REFUEL BEFORE ENGAGING AUTOPILOT"));return;}
 const auto* Origin=LWAviation84::Closest(FVector2D(GetActorLocation()),90000,true);if(!Origin)return;
 if(AircraftOnGround84&&FVector::Dist2D(GetActorLocation(),Origin->Apron)>25000&&!OnStrip(GetActorLocation(),*Origin,2000)){Driver->Notify(TEXT("TAXI TO AN AIRPORT BEFORE AUTOMATIC TAKEOFF"));return;}
 const auto* Target=Driver->bWaypoint?LWAviation84::Closest(Driver->Waypoint,90000,LWCanada68::Authorized(Driver)):nullptr;
 R->Departure84=Origin->Id;R->Destination84=Target?Target->Id:NAME_None;R->HasFlightGoal84=Target!=nullptr;R->FlightGoal84=Target?FVector2D(Target->A):FVector2D(GetActorLocation());if(!Target&&!LWCanada68::Authorized(Driver)){for(int Attempt=0;Attempt<8;Attempt++){bool Clear=true;for(int I=0;I<24;I++)Clear&=!LWGeography84::Canada(R->FlightGoal84+FVector2D(240000,0).GetRotated(I*15));if(Clear)break;R->FlightGoal84.X-=260000;}}R->Autopilot84=true;EngineOn=R->FlightEngine84=true;R->Hotwired=true;R->Thrust84=AircraftOnGround84?.3f:.75f;R->FlightPhase84=uint8(AircraftOnGround84?EPhase::Taxi:EPhase::Cruise);R->LoiterAngle84=0;AircraftDoorOpen84=false;Driver->Notify(Target?TEXT("AUTOPILOT / ")+Target->Name:TEXT("AUTOPILOT / TAKEOFF AND ROAM"),7);
}
void ALWVehicle::TickAircraft84(float Dt){auto* R=Record();if(!R)return;TickDamage(Dt);if(R->Exploded){if(TurbineAudio84)TurbineAudio84->Stop();SyncRecord();return;}
 const auto S=LWAviation84::Spec(R->Model);Dt=FMath::Clamp(Dt,0.f,.5f);R->FuelLitres=FuelLitres();const bool Auto=R->Autopilot84;
 if(EngineOn){R->FuelLitres=FMath::Max(0.f,R->FuelLitres-LWAviation84::Burn(S.Capacity,Auto,Dt));if(R->FuelLitres<=0)EngineOn=false;}
 if(Auto&&R->FuelLitres<S.Capacity*.035f&&!R->HasFlightGoal84){if(const auto* Emergency=LWAviation84::Closest(FVector2D(GetActorLocation()),90000,Driver&&LWCanada68::Authorized(Driver))){R->Destination84=Emergency->Id;R->FlightGoal84=FVector2D(Emergency->A);R->HasFlightGoal84=true;R->FlightPhase84=uint8(EPhase::Cruise);if(Driver)Driver->Notify(TEXT("LOW FUEL / DIVERTING TO ")+Emergency->Name,8);}}
 AlarmClock84-=Dt;if((R->FuelLitres<S.Capacity*.08f||R->Health<199.9f)&&Driver&&AlarmClock84<=0){AlarmClock84=3.5f;World->Sound(TEXT("AircraftWarning84"),GetActorLocation(),1);if(Driver)Driver->Notify(R->Health<199.9f?TEXT("AIRFRAME DAMAGE / LAND IMMEDIATELY"):TEXT("LOW FUEL / LAND SOON"),3);}
 if(!Auto&&Driver&&PlayerSeat==-1&&!Driver->IsUIOpen()){float Adjust=float(Held(Driver,EKeys::LeftShift))-float(Held(Driver,EKeys::LeftControl));if(auto* Input=ULWPlayerInput51::Get51(Driver);Input&&Input->Controller58)Adjust+=Input->Axes58.FindRef(EKeys::Gamepad_RightTriggerAxis)-Input->Axes58.FindRef(EKeys::Gamepad_LeftTriggerAxis);R->Thrust84=FMath::Clamp(R->Thrust84+Adjust*Dt*.25f,0.f,1.f);}
 const float Ground84=World->HeightAt(FVector2D(GetActorLocation()))+12;
 // A parked aircraft on the destination strip requires another circuit, rather
 // than blindly continuing the descent into it. Check once per outer tick.
 if(Auto&&EPhase(R->FlightPhase84)==EPhase::Final)if(const auto* Field=LWAviation84::Runway(R->Destination84))for(TActorIterator<ALWVehicle> Other(GetWorld());Other;++Other)if(*Other!=this&&FMath::Abs(Other->GetActorLocation().Z-Field->A.Z)<700&&OnStrip(Other->GetActorLocation(),*Field,100)){R->FlightPhase84=uint8(EPhase::Climb);R->Thrust84=1;R->LoiterAngle84=0;if(Driver)Driver->Notify(TEXT("RUNWAY OCCUPIED / GOING AROUND"),5);break;}
 float Remaining=Dt;while(Remaining>.0001f){const float Step=FMath::Min(Remaining,1.f/90);Remaining-=Step;FVector At=GetActorLocation();FRotator Rot=GetActorRotation();float Ground=Ground84;const auto* Departure=LWAviation84::Runway(R->Departure84);const auto* Dest=LWAviation84::Runway(R->Destination84);EPhase Phase=EPhase(R->FlightPhase84);FVector Target=At+GetActorForwardVector()*50000;float Desired=S.Cruise,Pitch=0,Yaw=0,Roll=0;bool Brakes=false;
  if(Auto){
   if(Phase==EPhase::Parked){R->Autopilot84=false;EngineOn=false;R->Thrust84=0;Desired=0;Brakes=true;}
   else if(Phase==EPhase::Taxi&&Departure){
    const FVector Right(-Departure->Direction().Y,Departure->Direction().X,0);FVector Corner=Departure->A+Right*9500;
    const bool AtCorner=R->LoiterAngle84>0;Target=AtCorner?Departure->A:Corner;Desired=FMath::Min(480.f, float(FVector::Dist2D(At,Target)*.65));R->Thrust84=.2f;
    if(FVector::Dist2D(At,Target)<220){if(!AtCorner)R->LoiterAngle84=1;else{Target=Departure->B;Desired=0;Brakes=true;if(FMath::Abs(HeadingError(Rot.Yaw,Target-At))<3){R->FlightPhase84=uint8(EPhase::Takeoff);R->Gear84=true;R->LoiterAngle84=0;}}}
   }else if(Phase==EPhase::Takeoff&&Departure){Target=Departure->B+Departure->Direction()*20000;Target.Z=At.Z+(Speed>S.Takeoff?14000:0);Desired=S.Takeoff*1.4f;R->Thrust84=1;if(!AircraftOnGround84&&At.Z>Ground+1500){R->Gear84=false;R->FlightPhase84=uint8(EPhase::Climb);}}
   else if(Phase==EPhase::Climb||Phase==EPhase::Cruise||Phase==EPhase::Loiter){R->Gear84=false;R->Thrust84=.8f;
    if(Dest){const FVector Pre=Dest->A-Dest->Direction()*180000;Target=Pre;const float D=FVector::Dist2D(At,Pre);Target.Z=Dest->A.Z+FMath::Clamp(D*.08f,10000.f,650000.f);if(D<55000&&FMath::Abs(At.Z-Target.Z)<20000){R->FlightPhase84=uint8(EPhase::Approach);R->LoiterAngle84=0;}}
    else{R->FlightPhase84=uint8(EPhase::Loiter);R->LoiterAngle84+=Step*S.Cruise/220000;Target=FVector(R->FlightGoal84,1200000)+FVector(FMath::Cos(R->LoiterAngle84)*220000,FMath::Sin(R->LoiterAngle84)*220000,0);}
    if(Phase==EPhase::Climb&&At.Z>Ground+7000)R->FlightPhase84=uint8(EPhase::Cruise);
   }else if(Phase==EPhase::Approach&&Dest){Target=Dest->A-Dest->Direction()*180000;Target.Z=Dest->A.Z+10000;Desired=S.Takeoff*1.3f;R->Gear84=true;R->Thrust84=.45f;if(FVector::Dist2D(At,Target)<15000&&FMath::Abs(HeadingError(Rot.Yaw,Dest->B-Dest->A))<25)R->FlightPhase84=uint8(EPhase::Final);else if(FVector::Dist2D(At,Target)<10000||R->LoiterAngle84>0){R->LoiterAngle84=1;Target=Dest->A-Dest->Direction()*80000;Target.Z=Dest->A.Z+5500;if(FMath::Abs(HeadingError(Rot.Yaw,Dest->B-Dest->A))<30)R->FlightPhase84=uint8(EPhase::Final);}}
   else if(Phase==EPhase::Final&&Dest){const FVector D=Dest->Direction();const float Along=FVector::DotProduct(At-Dest->A,D);Target=Dest->A+D*(Along+15000);Target.Z=Dest->A.Z+FMath::Max(0.f,float(-Along-15000)*.0524f);Desired=S.Takeoff*1.06f;R->Gear84=true;R->Thrust84=.35f;if(Along>Dest->Length()-10000&&!AircraftOnGround84){R->FlightPhase84=uint8(EPhase::Climb);R->Thrust84=1;if(Driver)Driver->Notify(TEXT("GO-AROUND / RUNWAY NOT CAPTURED"));}}
   else if(Phase==EPhase::Rollout&&Dest){Target=Dest->B;Desired=0;R->Thrust84=0;Brakes=true;if(Speed<15){R->FlightPhase84=uint8(EPhase::Parked);R->Autopilot84=false;EngineOn=false;R->FlightVelocity84=FVector::ZeroVector;if(Driver){Driver->Notify(TEXT("LANDED AT ")+Dest->Name,7);Driver->RequestSave40();}}}
   const float Error=HeadingError(Rot.Yaw,Target-At);Yaw=FMath::Clamp(Error*(AircraftOnGround84?.8f:.6f),AircraftOnGround84?-12.f:-7.f,AircraftOnGround84?12.f:7.f);Roll=AircraftOnGround84?0:FMath::Clamp(-Error*.7f,-22.f,22.f);Pitch=AircraftOnGround84?(Speed>=S.Takeoff&&Phase==EPhase::Takeoff?10:0):FMath::Clamp(float(FMath::RadiansToDegrees(FMath::Atan2(Target.Z-At.Z,FMath::Max(5000.,FVector::Dist2D(At,Target))))),-12.f,12.f);
   if(AircraftOnGround84&&Phase==EPhase::Taxi){Desired*=FMath::Clamp(1-FMath::Abs(Error)/65.f,.1f,1.f);if(Brakes&&Speed<20)Rot.Yaw+=FMath::Clamp(Error,-35*Step,35*Step);}
  }else{const bool Input=Driver&&PlayerSeat==-1&&!Driver->IsUIOpen();Pitch=FMath::Clamp(Rot.Pitch+(Input?Throttle*24*Step:0),-55.f,55.f);Roll=AircraftOnGround84?0:FMath::Clamp(Rot.Roll+(Input?-Steer*48*Step:0),-65.f,65.f);if(!Input){Pitch=FMath::FInterpTo(Pitch,0,Step,.7f);Roll=FMath::FInterpTo(Roll,0,Step,1.f);}float Rudder=Input?float(Held(Driver,EKeys::C))-float(Held(Driver,EKeys::Q)):0;Yaw=AircraftOnGround84?Steer*18: -FMath::Sin(FMath::DegreesToRadians(Roll))*14+Rudder*8;Desired=EngineOn?R->Thrust84*Spec().MaxSpeed:0;Brakes=AircraftOnGround84&&(Held(Driver,EKeys::SpaceBar)||R->Thrust84<.01f);}
  Rot.Yaw+=Yaw*Step;Rot.Pitch=Auto?FMath::FInterpTo(Rot.Pitch,Pitch,Step,1.3f):Pitch;Rot.Roll=Auto?FMath::FInterpTo(Rot.Roll,Roll,Step,AircraftOnGround84?4:1.8f):Roll;
  const float Accel=Brakes?Spec().Brake:(Desired>Speed?Spec().Acceleration:220.f);Speed=FMath::FInterpConstantTo(Speed,EngineOn?Desired:FMath::Max(0.f,Speed-110*Step),Step,Accel);if(R->Gear84&&!AircraftOnGround84)Speed=FMath::Min(Speed,S.Cruise*.82f);
  const FVector Forward=Rot.Vector();float VZ=Forward.Z*Speed;if(!AircraftOnGround84){float Lift=FMath::Clamp(Speed/(S.Takeoff*.90f),0.f,1.f);VZ-=(1-Lift)*1600;VZ=FMath::FInterpTo(R->FlightVelocity84.Z,VZ,Step,1.5f);}else if(Speed<S.Takeoff||Rot.Pitch<3)VZ=0;else AircraftOnGround84=false;
  FVector Velocity=FVector(Forward.X,Forward.Y,0).GetSafeNormal()*Speed;Velocity.Z=VZ;FVector Next=At+Velocity*Step;const float Surface=Ground84;
  if(Next.Z<=Surface+2){const bool Touch=!AircraftOnGround84;if(Touch&&(VZ<-650||FMath::Abs(Rot.Roll)>16||GearAlpha84<.95f||!LWAviation84::Runways().ContainsByPredicate([&](const auto& RW){return OnStrip(Next,RW,80);}))){SetActorLocation(Next);Explode();return;}
   if(Touch&&VZ<-250){FDamageEvent Damage;TakeDamage((-VZ-250)*.06f,Damage,nullptr,this);}AircraftOnGround84=true;Next.Z=Surface;Velocity.Z=0;Rot.Pitch=FMath::FInterpTo(Rot.Pitch,0,Step,3);Rot.Roll=FMath::FInterpTo(Rot.Roll,0,Step,4);if(Auto&&Touch)R->FlightPhase84=uint8(EPhase::Rollout);
  }
  // Sweep the fuselage as well as both wingtips. Ignore onboard residents; cabin
  // meshes remain query-only and never become physics obstacles to their parent.
  if(Speed>150){FCollisionQueryParams Q(NAME_None,false,this);if(Driver)Q.AddIgnoredActor(Driver);for(auto& N:Passengers)if(N)Q.AddIgnoredActor(N);bool HitObstacle=false;for(float Side:{-1.f,0.f,1.f}){FVector Offset=Rot.RotateVector(FVector(Side==0?Spec().HalfLength*.65f:0,Side*Spec().HalfWidth,Side==0?S.Floor+80:S.Floor));FHitResult Hit;if(GetWorld()->SweepSingleByChannel(Hit,At+Offset,Next+Offset,Rot.Quaternion(),ECC_WorldStatic,FCollisionShape::MakeSphere(45),Q)&&Hit.ImpactNormal.Z<.65f){HitObstacle=true;break;}}if(HitObstacle){if(Speed>1200||!AircraftOnGround84){SetActorLocation(Next);Explode();return;}Speed=0;Velocity=FVector::ZeroVector;Next=At;if(Driver)Driver->Notify(TEXT("TAXIWAY BLOCKED"));}}
  R->FlightVelocity84=Velocity;SetActorLocationAndRotation(Next,Rot,false,nullptr,ETeleportType::TeleportPhysics);
 }
 R->FlightEngine84=EngineOn;if(!TurbineAudio84&&EngineOn){TurbineAudio84=World->Sound(TEXT("AircraftTurbine84"),GetActorLocation(),1.3f);if(TurbineAudio84)TurbineAudio84->AttachToComponent(Root,FAttachmentTransformRules::KeepWorldTransform);}if(TurbineAudio84){if(!EngineOn)TurbineAudio84->Stop();else if(!TurbineAudio84->IsPlaying())TurbineAudio84->Play();TurbineAudio84->SetVolumeMultiplier(.45f+R->Thrust84*.85f);TurbineAudio84->SetPitchMultiplier(.65f+R->Thrust84*.65f);}
 TickAircraftCabin84(Dt);FlightDisplayClock84-=Dt;if(FlightDisplayClock84<=0){FlightDisplayClock84=.15f;DrawFlightDisplay84();}AircraftSyncClock84+=Dt;if(AircraftSyncClock84>=.5f){AircraftSyncClock84=0;SyncRecord();}
}
