#pragma once
#include "LWWorldObject.h"
#include "LWVehicleState.h"
#include "LWVehicleSpec.h"
#include "LWVehicle.generated.h"
UCLASS()
class LETHALWORLD_API ALWVehicle:public ALWWorldObject {
 GENERATED_BODY()
public:
 bool IsAircraft84()const{return LWTraffic::IsAircraft(Spec().Id);}
 void BuildAircraft84();void TickAircraft84(float Dt);void TickAircraftCabin84(float Dt);
 bool UseAircraft84(class ALWCharacter* P,FName Action);FName AircraftFocus84(const class ALWCharacter* P)const;FString AircraftPrompt84(FName Action)const;
 void ToggleAutopilot84();void OpenAircraftStorage84(class ALWCharacter* P,FName Action);void DrawFlightDisplay84();
 FVector AircraftEye84()const;bool ChangeAircraftSeat84(int Seat);void StandAircraft84();
 UPROPERTY() TObjectPtr<UStaticMeshComponent> GearMesh84;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> AircraftDoor84;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> AircraftSeats84;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> AircraftSeatBacks84;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> SuiteDoors84;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> BinLids84;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> AircraftShades84;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> AircraftScreens84;
 UPROPERTY() TObjectPtr<class UTextureRenderTarget2D> FlightDisplay84;
 UPROPERTY() TObjectPtr<class UAudioComponent> TurbineAudio84;
 TArray<FVector> AircraftSeatRest84;TArray<FBox> CabinObstacles84;
 float GearAlpha84=1,AlarmClock84=0,FlightDisplayClock84=0,AircraftSyncClock84=0;
 bool AircraftOnGround84=true,AircraftDoorOpen84=false;
 bool IsElectric74()const{return LWTraffic::IsElectric(Spec().Id);}
 bool IsCamper74()const{return LWTraffic::IsCamper(Spec().Id);}
 bool IsSolarRV74()const{return FName(Spec().Id)==TEXT("solstice_rv");}
 bool SelfDriving74=false;float DashClock74=0,SolarKW74=0,PowerKW74=0,RoofCheck74=0,CabinPitch74=0;bool RoofClear74=true;
 FVector CabinVelocity74=FVector::ZeroVector;
 UPROPERTY() TObjectPtr<class UTextureRenderTarget2D> DashTarget74;
 UPROPERTY() TObjectPtr<class UMaterialInstanceDynamic> DashMaterial74;
 TArray<FVector2D> DashRoads74;FVector2D DashRoadCenter74=FVector2D(1.e12,1.e12);
 void BuildElectric74();void TickElectric74(float Dt);void DrawDashboard74();void TickElectricAudio74(float Dt,bool Brake,float Gas);
 float BatteryCapacity74()const;float BatteryCharge74()const;void TickBattery74(float Dt);
 void ToggleSelfDrive74();bool TickSelfDriveGate74(float Dt);bool UseElectric74(class ALWCharacter* P,FName Action);
 FVector CabinToActor74(FVector V)const;FVector ActorToCabin74(FVector V)const;
 void BuildLuxury66();void TickLuxury66(float Dt);bool UseLuxury66(class ALWCharacter* P,FName Action);
 void RepairCoach80();
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> SlideSeals80;
 FString LuxuryLabel66(FName Action)const;bool ReadyToDrive66();bool ClearExtension66()const;
 FVector BunkPosition66(int Bunk)const;float SeatYaw66(int Seat)const;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> Slides66;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> ShadesMesh66;
 UPROPERTY() TArray<TObjectPtr<class UPointLightComponent>> CabinLamps66;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> AwningMesh66;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Water66;
 UPROPERTY() TArray<TObjectPtr<class UTextRenderComponent>> Screens66;
 UPROPERTY() TArray<TObjectPtr<class UMaterialInstanceDynamic>> TVMaterials66;
 UPROPERTY() TObjectPtr<class UAudioComponent> TVAudio66;
 UPROPERTY() TObjectPtr<class USceneCaptureComponent2D> TVCamera66;
 UPROPERTY() TObjectPtr<class UTextureRenderTarget2D> TVTarget66;
 TArray<FVector> SlideRest66;TArray<int32> SlideSide66;TArray<FVector> ShadeRest66;
 float SlideAlpha66=0,AwningAlpha66=0,ScreenClock66=0;bool PendingIgnition66=false;
 TWeakObjectPtr<class ALWZombie> Grabber49,ThrowIgnore49;bool Held49=false;
 FVector FlightVelocity49=FVector::ZeroVector;bool Airborne49=false,FlightHitPlayer49=false;float FlightTime49=0;int FlightBounces49=0;
 bool Flip49(class ALWCharacter* P);bool TickThrown49(float Dt);bool Grab49(class ALWZombie* Boss);void Throw49(FVector Velocity);FString VehiclePrompt49()const;
 bool TryIncline45(FVector Desired,FRotator Rotation);
 void ApplyGarage45();
 UPROPERTY() TArray<TObjectPtr<class ALWResident>> Passengers;
 int32 PlayerSeat=-1; // -1 driver, 0..Seats-2 passengers (legacy passenger indices).
 UPROPERTY() TObjectPtr<class ALWResident> Chauffeur;
 bool AutoDriving=false,StopRequested=false,Boarding=false,DriverDismissed=false;
 float BoardingClock=0,RepathClock=0,StuckClock=0,ReverseClock=0;
 int32 RecoveryAttempts=0,DriveStep=1;
 FVector2D DriveGoal=FVector2D::ZeroVector;
 TArray<FVector2D> DrivePath;
 bool DriveComplete=false;
 FVector CabinEye=FVector::ZeroVector;FVector2D CabinMove=FVector2D::ZeroVector;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> CamperDoor;
 bool CamperDoorOpen=false;float CamperDoorAngle=0,RecoveryClock=0;
 void TickCamperEntry(float Dt);FName CamperFocus(const ALWCharacter* P)const;void UseCamper(ALWCharacter* P,FName Action);void OpenCamperStorage(ALWCharacter* P,FName Action);void RecoverTerrain(float Dt);
 UPROPERTY() TArray<TObjectPtr<class UAudioComponent>> DriveAudio;
 TArray<float> DriveAudioGain;float EngineRPM=800,AudioSpeed=0,ShiftTimer=0;int AudioGear=1;bool AudioWasEngine=false;
 UPROPERTY() TObjectPtr<ALWVehicle> ConvoyLeader;
 UPROPERTY() TObjectPtr<ALWCharacter> ConvoyOwner;
 bool BoardConvoy(ALWResident* NPC,ALWCharacter* Owner);void TickConvoy(float Dt);
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> DamageFX;
 float DamageClock=0;bool WreckShown=false;
 UPROPERTY() TObjectPtr<class UProceduralMeshComponent> DentedBody;
 TArray<TArray<FVector>> DentOriginal;
 bool bDentsDirty=false;float LastDentBuild=-100;
 void AddDent(FVector Point,FVector Direction,float Damage);void RebuildDents();
 void TickDamage(float Dt);void Explode();
 void TickDriveAudio(float Dt,bool Brake,float Gas);void StopDriveAudio();
 void BuildDetail42();void TickInstruments42(float Dt);void VehicleSound42(FName Event,float Volume=1.f);
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> GaugeNeedles42;
 UPROPERTY() TArray<TObjectPtr<UStaticMeshComponent>> GaugeLamps42;
 UPROPERTY() TObjectPtr<class UMaterialInstanceDynamic> HeadLens42;
 UPROPERTY() TObjectPtr<class UMaterialInstanceDynamic> BrakeLens42;
 float HornCooldown42=0;
 UPROPERTY() TObjectPtr<class UTextRenderComponent> GearDisplay42;
 float Coolant42=20,AudioLoad42=0,AirBrakeCooldown42=0,AudioQuiet42=0;bool WasBrake42=false;

 void StandInCamper();void TickCabinWalk(float Dt);
 FVector PlayerEye()const;
 int NearestSeat(const class ALWCharacter* P)const;
 bool ChangeSeat(int Seat);void TickAutopilot(float Dt);void RequestDriverStop();void BuildCamper();
 float ConvoyStartDelay=0;
 bool HasFreeCompanionSeat()const;
 bool SpawnPlacementPending=false;float SpawnPlacementRetry=0;
 bool ResolveSpawnPlacement();
 float FuelLitres()const;float FuelCapacity()const;bool HasFuel()const;
 void TickFuel(float Dt);bool Refuel(class ALWCharacter* P);
 void MarkLastDriven();
 static FName LastDrivenId(const class ALWWorld* W);
 static FGuid LastDrivenVIN(const class ALWWorld* W);
 bool Board(class ALWResident* NPC);void UnloadPassengers();
 bool IsExpansion57()const;bool HasTurret57()const;bool IsHelicopter57()const;
 void Build57();void TickTurret57(float Dt);bool TickFlight57(float Dt);void ReloadTurret57();
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Turret57;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Barrel57;
 UPROPERTY() TObjectPtr<UStaticMeshComponent> Rotor57;
 UPROPERTY() TObjectPtr<class UAudioComponent> ServoAudio57;
 TWeakObjectPtr<class ALWZombie> GunnerTarget57;
 FVector Velocity57=FVector::ZeroVector;float RotorSpeed57=0,Altitude57=0,ShotClock57=0,ReloadClock57=0,TargetClock57=0,TurretHeat57=0;
 bool TurretTrigger57=false;
 ALWVehicle();
 const LWTraffic::FSpec& Spec()const{return LWTraffic::Get(Record()?Record()->Model:FName(TEXT("sedan")));}
 FVector SeatLocation(int Seat)const;
 FVector DriverEye()const;
 FVector SeatOffset50=FVector::ZeroVector;void AdjustSeat50(float Forward,float Up,float Dt);
 void BuildVariant();void TickFeatures(float Dt);
 UPROPERTY() TArray<TObjectPtr<class UPointLightComponent>> EmergencyLights;
 UPROPERTY() TObjectPtr<class UAudioComponent> SirenAudio;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> FeaturePanel;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> Windshield;
 UPROPERTY() TObjectPtr<class UMaterialInstanceDynamic> GlassWet;
 float WindshieldWater=0,BodyWater=0;
 UPROPERTY() TArray<TObjectPtr<class UMaterialInstanceDynamic>> PaintWet;
 void TickGlass(float Dt);
 bool FeatureOn=false,SirenOn=false;float SteeringAngle=0,FeatureAngle=0;

 UPROPERTY() TObjectPtr<class UBoxComponent> Chassis;
 UPROPERTY() TObjectPtr<class ALWCharacter> Driver;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> SteeringWheel;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> GloveLid;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Wipers;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Wheels;
 UPROPERTY() TArray<TObjectPtr<class USpotLightComponent>> Headlamps;
 UPROPERTY() TArray<TObjectPtr<class UPointLightComponent>> Indicators;
 UPROPERTY() TObjectPtr<class UAudioComponent> EngineAudio;
 UPROPERTY() TObjectPtr<class UAudioComponent> RadioAudio;
 UPROPERTY() TObjectPtr<ALWWorldObject> GloveObject;
 UPROPERTY() TObjectPtr<ALWWorldObject> CargoObject;
 TMap<TWeakObjectPtr<class ALWZombie>,float> ImpactTimes;
 void HitPedestrians(FVector End,FRotator Rotation);
 float Speed=0,Throttle=0,Steer=0,SignalClock=0,WheelSpin=0,GloveAngle=0;
 bool EngineOn=false,Headlights=false,WipersOn=false,GloveOpen=false;
 int Signal=0,RadioChannel=0;
 void InitializeVehicle();void SyncRecord();
 FLWVehicleRecord* Record()const;
 virtual void Tick(float Dt)override;
 virtual void EndPlay(const EEndPlayReason::Type Reason)override;
 virtual void Use(class ALWCharacter* P)override;
 virtual FString Prompt()const override;
 virtual float TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C)override;
 bool Enter(class ALWCharacter* P,int Seat=-1);bool Exit(bool Force=false);
 void Ignition();void Control(FName Action);FName FocusControl()const;FString CabinPrompt()const;
 FName GloveId()const;FName CargoId()const{return RecordId;}
};
