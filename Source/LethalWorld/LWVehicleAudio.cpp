#include "LWVehicle.h"
#include "LWWorld.h"
#include "LWAudioCatalog.h"
#include "LWCharacter.h"
#include "Components/AudioComponent.h"
#include "Engine/World.h"

void ALWVehicle::VehicleSound42(FName Event,float Volume){
 if(!World)return;
 if(!World->AudioCatalog||!World->AudioCatalog->Slots.Contains(Event)){
  const FString N=Event.ToString();
  if(N.StartsWith(TEXT("CarStart42")))Event=TEXT("CarIgnition");
  else if(N.Contains(TEXT("Door"))||N.Contains(TEXT("Cargo")))Event=TEXT("CarDoor");
  else if(N.Contains(TEXT("Glove"))||N.Contains(TEXT("Switch"))||N==TEXT("CarSeat")||N==TEXT("CarHandbrake"))Event=TEXT("Click");
 }
 World->Sound(Event,GetActorLocation(),Volume);
}
void ALWVehicle::StopDriveAudio(){for(auto& A:DriveAudio)if(IsValid(A)){A->Stop();A->DestroyComponent();}DriveAudio.Empty();DriveAudioGain.Empty();}
void ALWVehicle::TickDriveAudio(float Dt,bool Brake,float Gas){
 if(!World)return;
 const FName Model(Spec().Id);const bool Diesel=Model==TEXT("rv")||Model==TEXT("bus")||Model==TEXT("boxtruck");
 const bool Sport=Model==TEXT("muscle")||Model==TEXT("supercar"),Bike=Model==TEXT("dirtbike");
 const float V=FMath::Abs(Speed),Deceleration=FMath::Max(0.f,(FMath::Abs(AudioSpeed)-V)/FMath::Max(Dt,.001f));AudioSpeed=Speed;
 const float Idle=Diesel?650.f:Bike?1400.f:800.f,Redline=Diesel?3200.f:Bike?11000.f:Sport?8200.f:6500.f;
 if(!AudioWasEngine&&EngineOn)VehicleSound42(FName(*(FString(TEXT("CarStart42"))+(Diesel?TEXT("Diesel"):Bike?TEXT("Bike"):Sport?TEXT("Sport"):TEXT("Petrol")))),.65f);
 if(AudioWasEngine&&!EngineOn)VehicleSound42(TEXT("CarEngineStop"),.6f);
 AudioWasEngine=EngineOn;if(!EngineOn&&V<3&&!WipersOn&&DriveAudio.IsEmpty()){EngineRPM=0;AudioGear=1;return;}ShiftTimer=FMath::Max(0.f,ShiftTimer-Dt);AirBrakeCooldown42=FMath::Max(0.f,AirBrakeCooldown42-Dt);
 const float Ratios[]={3.15f,2.16f,1.57f,1.19f,.94f,.75f};
 const float RoadRPM=FMath::Clamp(V/FMath::Max(1.f,Spec().MaxSpeed),0.f,1.25f)*(Redline-Idle);
 const float LoadTarget=EngineOn?FMath::Clamp(FMath::Abs(Gas),0.f,1.f):0;
 AudioLoad42=FMath::FInterpTo(AudioLoad42,LoadTarget,Dt,LoadTarget>AudioLoad42?5.f:8.f);
 float Coupled=Idle+RoadRPM*Ratios[FMath::Clamp(AudioGear-1,0,5)]/.75f;
 if(EngineOn&&ShiftTimer<=0&&Speed>=0){
  int Next=AudioGear;if(Coupled>Redline*(.76f+.14f*AudioLoad42)&&AudioGear<6)++Next;
  else if(Coupled<Redline*.28f&&AudioGear>1)--Next;
  if(Next!=AudioGear){AudioGear=Next;ShiftTimer=.42f;VehicleSound42(TEXT("CarGearShift"),.18f+AudioLoad42*.18f);}
 }
 if(V<20&&ShiftTimer<=0)AudioGear=1;
 Coupled=Idle+RoadRPM*Ratios[FMath::Clamp(AudioGear-1,0,5)]/.75f;
 const float TargetRPM=EngineOn?FMath::Clamp(Coupled+AudioLoad42*FMath::Max(0.f,1-V/700)*(Redline-Idle)*.20f,Idle,Redline):0;
 EngineRPM=FMath::FInterpTo(EngineRPM,TargetRPM,Dt,ShiftTimer>.22f?10.f:5.f);
 const float Rev=FMath::Clamp((EngineRPM-Idle)/(Redline-Idle),0.f,1.f);
 // Equal-power crossfades between three separately generated, steady RPM layers.
 const float IdleWeight=FMath::Clamp(1-Rev/.28f,0.f,1.f);
 const float HighWeight=FMath::Clamp((Rev-.35f)/.65f,0.f,1.f);
 const float LowWeight=FMath::Max(0.f,1-IdleWeight-HighWeight);
 const float Harsh=Brake?FMath::Clamp(Deceleration/FMath::Max(1.f,Spec().Brake),0.f,1.f):0;
 const float Corner=FMath::Abs(SteeringAngle)*V/FMath::Max(1.f,Spec().MaxSpeed)/25;
 const float ShiftGain=ShiftTimer>.23f?.7f:1.f;
 const float Motor=EngineOn?(.38f+.22f*AudioLoad42)*ShiftGain*1.8f:0;
 const float Road=FMath::Clamp(V/Spec().MaxSpeed,0.f,1.f);
 const float Levels[]={Motor*FMath::Sqrt(IdleWeight),Motor*FMath::Sqrt(LowWeight),Motor*FMath::Sqrt(HighWeight),Road*.32f,Harsh*.30f*FMath::Clamp(V/350,0.f,1.f),FMath::Clamp((Harsh-.65f)*2+Corner-.65f,0.f,1.f)*FMath::Clamp(V/600,0.f,1.f)*.5f,WipersOn?.24f:0,Road*Road*.23f};
 const FString Prefix=FString(TEXT("Vehicle42_"))+Spec().Id+TEXT("_");
 const FName Slots[]={FName(*(Prefix+TEXT("Idle"))),FName(*(Prefix+TEXT("Low"))),FName(*(Prefix+TEXT("High"))),TEXT("CarTires"),TEXT("CarBrake"),TEXT("CarSkid"),TEXT("CarWipers"),TEXT("CarWind42")};
 if(DriveAudio.Num()!=8){StopDriveAudio();DriveAudio.SetNum(8);DriveAudioGain.SetNumZeroed(16);}
 bool Audible=false;
 for(int I=0;I<8;++I){
  auto& A=DriveAudio[I];Audible|=Levels[I]>.001f;
  // Keep the engine layers running together through shifts; do not restart them on each crossfade.
  if(!IsValid(A)&&(Levels[I]>.003f||(I<3&&EngineOn))){
   FName Actual=Slots[I];if(I<3&&(!World->AudioCatalog||!World->AudioCatalog->Slots.Contains(Actual)))Actual=I==0?TEXT("CarEngine"):Diesel?TEXT("CarLoadDiesel"):Bike?TEXT("CarLoadBike"):Sport?TEXT("CarLoadSport"):TEXT("CarLoadPetrol");
   A=World->Sound(Actual,GetActorLocation(),1.f);
   if(A){DriveAudioGain[I]=A->VolumeMultiplier;DriveAudioGain[I+8]=A->PitchMultiplier;A->SetVolumeMultiplier(0);A->AttachToComponent(Root,FAttachmentTransformRules::KeepWorldTransform);A->SetRelativeLocation(I<3?FVector(Model==TEXT("rv")?-820:Model==TEXT("supercar")?-150:95,0,65):FVector::ZeroVector);}
  }
  if(!IsValid(A))continue;
  A->SetVolumeMultiplier(FMath::FInterpTo(A->VolumeMultiplier,DriveAudioGain[I]*Levels[I],Dt,8));
  const float Pitch=I==0?FMath::Lerp(.93f,1.18f,Rev):I==1?FMath::Lerp(.74f,1.32f,Rev):I==2?FMath::Lerp(.72f,1.13f,Rev):I==3?FMath::Lerp(.85f,1.15f,Road):1;
  A->SetPitchMultiplier(DriveAudioGain[I+8]*Pitch);
  const bool Inside=Driver&&Driver->Vehicle==this;
  A->SetLowPassFilterEnabled(Inside&&I!=6);A->SetLowPassFilterFrequency(Inside?(I<3?2100.f:3400.f):20000.f);
 }
 AudioQuiet42=Audible?0:AudioQuiet42+Dt;if(AudioQuiet42>1){StopDriveAudio();AudioQuiet42=0;}
 if(Brake&&!WasBrake42&&V<50)VehicleSound42(TEXT("CarHandbrake"),.3f);WasBrake42=Brake;
 if(Diesel&&Brake&&V<40&&Deceleration>100&&AirBrakeCooldown42<=0){VehicleSound42(TEXT("CarAirBrake"),.6f);AirBrakeCooldown42=2;}
}
