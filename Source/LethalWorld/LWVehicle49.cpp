#include "LWAircraft84.h"
#include "LWVehicle.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWZombie.h"
#include "LWResident.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
FVector ALWVehicle::DriverEye()const{if(IsAircraft84())return LWAviation84::Seat(Spec().Id,-1)+FVector(0,0,83);
 if(IsSolarRV74())return CabinToActor74(FVector(65,-55,220));
 if(FName(Spec().Id)==TEXT("apex_ev"))return CabinToActor74(FVector(-68,-43,114));
 if(IsExpansion57())return FVector(LWTraffic::FrontOffset(Spec())+(IsHelicopter57()?20:-35),-43,IsHelicopter57()?96:69);
 const FName M(Spec().Id);float Z=M==TEXT("rv")?145:M==TEXT("bus")||M==TEXT("boxtruck")?109:M==TEXT("supercar")?44:M==TEXT("muscle")?59:M==TEXT("sedan")?61:M==TEXT("dirtbike")?75:69;
 return FVector(LWTraffic::FrontOffset(Spec())+(M==TEXT("rv")?65:-35),Spec().Seats==1?0:M==TEXT("rv")?-55:-43,Z);
}
FString ALWVehicle::Prompt()const{if(Held49||Airborne49)return TEXT("KEEP CLEAR");FString S=VehiclePrompt49();if(!IsAircraft84()&&!SpawnPlacementPending&&Record()&&!Record()->Exploded&&!Driver&&!Chauffeur&&!ConvoyOwner&&FMath::Abs(Speed)<120)S+=TEXT("  [SHIFT+E] FLIP / UNSTICK");return S;}
bool ALWVehicle::Flip49(ALWCharacter* P){
 if(!P||!P->CanAct()||FVector::Dist2D(P->GetActorLocation(),GetActorLocation())>Spec().HalfLength+450)return false;
 if(Driver||Chauffeur||ConvoyOwner||Passengers.ContainsByPredicate([](auto& N){return IsValid(N);})||Grabber49.IsValid()||Airborne49||FMath::Abs(Speed)>120||!Record()||Record()->Exploded){P->Notify(TEXT("STOP AND EMPTY THE VEHICLE FIRST"));return false;}
 FCollisionQueryParams Q(SCENE_QUERY_STAT(VehicleFlip49),false,this);Q.AddIgnoredActor(P);const FRotator Upright(0,GetActorRotation().Yaw,0);
 for(FVector Offset:{FVector::ZeroVector,FVector(200,0,0),FVector(-200,0,0),FVector(0,200,0),FVector(0,-200,0),FVector(400,0,0),FVector(-400,0,0)}){
  FVector At=GetActorLocation()+Upright.RotateVector(Offset);float Highest=-BIG_NUMBER;bool Supported=true;
  for(float X:{-Spec().HalfLength*.85f,0.f,Spec().HalfLength*.85f})for(float Y:{-Spec().HalfWidth*.85f,Spec().HalfWidth*.85f}){FVector Sample=At+Upright.RotateVector(FVector(X,Y,0));FHitResult H;const float Terrain=World->HeightAt(FVector2D(Sample));if(!GetWorld()->LineTraceSingleByChannel(H,FVector(Sample.X,Sample.Y,FMath::Max(Sample.Z,Terrain)+800),FVector(Sample.X,Sample.Y,Terrain-1000),ECC_WorldStatic,Q)||H.ImpactNormal.Z<.65f){Supported=false;break;}Highest=FMath::Max(Highest,float(H.ImpactPoint.Z));}
  if(!Supported||FMath::Abs(Highest-At.Z)>Spec().HalfLength+1500)continue;At.Z=Highest+80;
  if(GetWorld()->OverlapBlockingTestByChannel(At,Upright.Quaternion(),ECC_Pawn,FCollisionShape::MakeBox(Chassis->GetUnscaledBoxExtent()+FVector(5)),FCollisionQueryParams(NAME_None,false,this)))continue;
  SetActorLocationAndRotation(At,Upright,false,nullptr,ETeleportType::TeleportPhysics);Speed=Throttle=Steer=0;EngineOn=false;RecoveryClock=0;SyncRecord();P->RequestSave40();VehicleSound42(TEXT("CarImpact"),.4f);P->Notify(TEXT("VEHICLE RECOVERED"));return true;
 }P->Notify(TEXT("NOT ENOUGH CLEAR SPACE TO RECOVER VEHICLE"));return false;
}
bool ALWVehicle::Grab49(ALWZombie* Boss){
 if(!Boss||Boss->bDead||Grabber49.IsValid()||Airborne49||SpawnPlacementPending||!Record()||Record()->Exploded||Record()->Stored45)return false;
 Held49=true;Grabber49=ThrowIgnore49=Boss;Speed=Throttle=Steer=0;SelfDriving74=AutoDriving=Boarding=false;CabinVelocity74=FVector::ZeroVector;CabinMove=FVector2D::ZeroVector;EngineOn=false;StopDriveAudio();Chassis->IgnoreActorWhenMoving(Boss,true);if(Driver){Driver->ClosePanels();Driver->Notify(TEXT("HOLD ON!"));}return true;
}
void ALWVehicle::Throw49(FVector V){Held49=false;Grabber49.Reset();FlightVelocity49=V;Airborne49=true;FlightTime49=0;FlightBounces49=0;FlightHitPlayer49=false;Speed=Throttle=Steer=0;if(!V.IsNearlyZero())SetActorRotation(FRotator(0,V.Rotation().Yaw,0));}
bool ALWVehicle::TickThrown49(float Dt){
 if(!Held49&&!Airborne49)return false;if(Held49&&!Grabber49.IsValid())Throw49(FVector::ZeroVector);
 auto* P=Cast<ALWCharacter>(UGameplayStatics::GetPlayerPawn(this,0));
 TickDamage(Dt);TickGlass(Dt);TickInstruments42(Dt);
 if(auto* Boss=Grabber49.Get()){
  if(Boss->bDead||Boss->Missing(3)||Boss->Missing(4)||Record()->Exploded){Throw49(FVector::ZeroVector);}
  else{FVector Target=Boss->GetActorLocation()+Boss->GetActorForwardVector()*(Boss->GetCapsuleComponent()->GetScaledCapsuleRadius()+Spec().HalfLength+140)+FVector(0,0,Boss->GetCapsuleComponent()->GetScaledCapsuleHalfHeight()*.65f);
   FHitResult Hit;SetActorLocationAndRotation(FMath::VInterpTo(GetActorLocation(),Target,Dt,3),FRotator(0,Boss->GetActorRotation().Yaw+90,0),true,&Hit);if(Hit.bBlockingHit)Throw49(FVector::ZeroVector);SyncRecord();return true;
  }
 }
 FlightTime49+=Dt;
 for(float Remaining=Dt;Remaining>0&&Airborne49;){float Step=FMath::Min(Remaining,.016f);Remaining-=Step;FlightVelocity49.Z-=980*Step;const FVector Start=GetActorLocation(),End=Start+FlightVelocity49*Step;const float ImpactSpeed=FlightVelocity49.Size();
  Speed=ImpactSpeed;HitPedestrians(End,GetActorRotation());Speed=0;
  if(P&&!Driver&&!FlightHitPlayer49&&P->Health>0){FCollisionQueryParams Q(NAME_None,false,this);TArray<FHitResult> Hits;FCollisionObjectQueryParams Pawns(ECC_Pawn);GetWorld()->SweepMultiByObjectType(Hits,Start,End,GetActorQuat(),Pawns,FCollisionShape::MakeBox(Chassis->GetScaledBoxExtent()),Q);for(const auto& H:Hits)if(H.GetActor()==P){FlightHitPlayer49=true;UGameplayStatics::ApplyDamage(P,FMath::Clamp(ImpactSpeed*.03f,15.f,100.f),nullptr,this,nullptr);P->LaunchCharacter(FlightVelocity49.GetSafeNormal()*600+FVector(0,0,180),true,true);break;}}
  FHitResult Hit;SetActorLocation(End,true,&Hit);if(Hit.bBlockingHit){const float D=FMath::Clamp(ImpactSpeed*.025f,5.f,100.f);auto Occupants=Passengers;if(Driver)UGameplayStatics::ApplyDamage(Driver,D*.5f,nullptr,this,nullptr);for(auto& N:Occupants)if(N)UGameplayStatics::ApplyDamage(N,D*.5f,nullptr,this,nullptr);if(Chauffeur)UGameplayStatics::ApplyDamage(Chauffeur,D*.5f,nullptr,this,nullptr);UGameplayStatics::ApplyPointDamage(this,D,FlightVelocity49.GetSafeNormal(),Hit,nullptr,this,nullptr);if(auto* Other=Cast<ALWVehicle>(Hit.GetActor())){if(Other->Driver)UGameplayStatics::ApplyDamage(Other->Driver,D*.5f,nullptr,this,nullptr);UGameplayStatics::ApplyPointDamage(Other,D,FlightVelocity49.GetSafeNormal(),Hit,nullptr,this,nullptr);}World->Sound(TEXT("CarImpact"),Hit.ImpactPoint,1);FlightVelocity49=FlightVelocity49.MirrorByVector(Hit.ImpactNormal)*.25f;SetActorLocation(GetActorLocation()+Hit.ImpactNormal*4,false);if(++FlightBounces49>=2||ImpactSpeed<500){Airborne49=false;FlightVelocity49=FVector::ZeroVector;}}
  if(FlightTime49>15&&FlightVelocity49.Size()<100)Airborne49=false;
 }
 if(!Airborne49){if(auto* B=ThrowIgnore49.Get())Chassis->IgnoreActorWhenMoving(B,false);ThrowIgnore49.Reset();SyncRecord();if(P)P->RequestSave40();}return true;
}

void ALWVehicle::AdjustSeat50(float Forward,float Up,float Dt){
 if(!FMath::IsFinite(Dt)||Dt<0)return;
 SeatOffset50.X=FMath::Clamp(SeatOffset50.X+FMath::Clamp(Forward,-1.f,1.f)*FMath::Min(Dt,.1f)*18.,-24.,20.);
 SeatOffset50.Z=FMath::Clamp(SeatOffset50.Z+FMath::Clamp(Up,-1.f,1.f)*FMath::Min(Dt,.1f)*18.,-15.,15.);
}
