#include "LWElectric74.h"
#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWResident.h"
#include "LWNavigation.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"
#include "Engine/Canvas.h"
#include "Engine/StaticMesh.h"
#include "Engine/Engine.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "CanvasItem.h"

FVector ALWVehicle::CabinToActor74(FVector V)const{return Root->GetRelativeTransform().TransformPosition(V);}
FVector ALWVehicle::ActorToCabin74(FVector V)const{return Root->GetRelativeTransform().InverseTransformPosition(V);}
float ALWVehicle::BatteryCapacity74()const{return LWElectric74::Capacity(IsSolarRV74());}
float ALWVehicle::BatteryCharge74()const{const auto* R=Record();if(!R)return 0;return R->BatteryKWh74<0?BatteryCapacity74()*.85f:FMath::IsFinite(R->BatteryKWh74)?FMath::Clamp(R->BatteryKWh74,0.f,BatteryCapacity74()):0;}
void ALWVehicle::TickBattery74(float Dt){
 auto* R=Record();if(!R||!World)return;R->BatteryKWh74=BatteryCharge74();
 const double Now=(World->DayNumber-1)*24.+World->TimeOfDay;
 if(R->SolarHours74<0||R->SolarHours74>Now)R->SolarHours74=Now;
 RoofCheck74-=Dt;if(RoofCheck74<=0){RoofCheck74=2;FHitResult H;FCollisionQueryParams Q(NAME_None,false,this);if(Driver)Q.AddIgnoredActor(Driver);FVector Top=Root->GetComponentTransform().TransformPosition(FVector(IsSolarRV74()?-400:-90,0,IsSolarRV74()?367:144));RoofClear74=!GetWorld()->LineTraceSingleByChannel(H,Top,Top+FVector(0,0,100000),ECC_Visibility,Q);}
 const float RoofKW=IsSolarRV74()?8.4f:1.2f;
 const float Sky=RoofClear74?FMath::Clamp(1-World->CloudAmount*.8f,.12f,1.f):0;
 SolarKW74=RoofKW*Sky*LWElectric74::Sunlight(World->TimeOfDay);
 // Travel consumption uses elapsed simulation seconds; the game-day solar cycle also
 // supports sleeping and streamed-out parking. Never charge a destroyed vehicle.
 PowerKW74=EngineOn?(IsSolarRV74()?1.4f:.4f)+FMath::Abs(Speed)*(IsSolarRV74()?.055f:.028f)+FMath::Max(0.f,Throttle)*(IsSolarRV74()?65:140):0;
 if(WasBrake42&&Speed>100)PowerKW74=-FMath::Min(IsSolarRV74()?60.f:90.f,Speed*.025f);
 if(!R->Exploded){
  const double Solar=LWElectric74::SunHours(FMath::Max(R->SolarHours74,Now-72),Now)*RoofKW*Sky;
  R->BatteryKWh74=FMath::Clamp(R->BatteryKWh74+float(Solar)-PowerKW74*FMath::Max(0.f,Dt)/3600.f,0.f,BatteryCapacity74());
 }
 R->SolarHours74=Now;
 if(EngineOn&&!HasFuel()){EngineOn=false;Throttle=0;if(AutoDriving||Boarding)StopRequested=true;if(Driver)Driver->Notify(TEXT("BATTERY EMPTY / SOLAR CHARGING AVAILABLE IN DAYLIGHT"));}
}

void ALWVehicle::ToggleSelfDrive74(){
 if(!IsElectric74()||!Driver||Driver->IsUIOpen()||!Record())return;
 if(SelfDriving74){RequestDriverStop();return;}
 if(Held49||Airborne49||Record()->Exploded||Record()->Health<=0){Driver->Notify(TEXT("AUTOPILOT UNAVAILABLE"));return;}
 if(Chauffeur){Driver->Notify(TEXT("DISMISS THE DRIVER FIRST"));return;}
 if(!Driver->bWaypoint){Driver->Notify(TEXT("SET A MAP WAYPOINT FIRST"));return;}
 if(!Record()->Hotwired&&!Driver->HasKey(Record()->VIN)){Driver->Notify(TEXT("KEY OR HOTWIRE REQUIRED"));return;}
 if(!HasFuel()){Driver->Notify(TEXT("BATTERY EMPTY"));return;}
 bool Complete=false;auto Path=LWNavigation::FindPath(FVector2D(GetActorLocation()),Driver->Waypoint,World->Seed,&Complete);
 if(Path.Num()<2){Driver->Notify(TEXT("NO DRIVABLE ROUTE TO WAYPOINT"));return;}
 SelfDriving74=true;AutoDriving=false;Boarding=false;StopRequested=false;DriverDismissed=true;
 DriveGoal=Driver->Waypoint;DrivePath=MoveTemp(Path);DriveStep=1;DriveComplete=Complete;StuckClock=ReverseClock=0;RecoveryAttempts=0;
 ReadyToDrive66();MarkLastDriven();Driver->Notify(IsSolarRV74()?TEXT("AUTOPILOT ENGAGED / SPACE TO WALK THE CABIN"):TEXT("AUTOPILOT ENGAGED"));
}
bool ALWVehicle::TickSelfDriveGate74(float Dt){
 if(!Driver||!Record())return false;
 if(!Driver->bWaypoint||!HasFuel()||Record()->Health<=0||Record()->Exploded||Held49||Airborne49||Driver->Health<=0)StopRequested=true;
 if(StopRequested){Throttle=Steer=0;AutoDriving=true;if(FMath::Abs(Speed)<5){Speed=0;Signal=0;SelfDriving74=AutoDriving=Boarding=StopRequested=false;EngineOn=false;Driver->Notify(TEXT("AUTOPILOT PARKED"));}return false;}
 if(!ReadyToDrive66()){Throttle=Steer=0;AutoDriving=false;return false;}
 CamperDoorOpen=false;EngineOn=true;AutoDriving=true;return true;
}
bool ALWVehicle::UseElectric74(ALWCharacter* P,FName Action){
 if(!IsElectric74()||!P)return false;
 if(Action==TEXT("autopilot74")||Action==TEXT("feature")){if(P==Driver)ToggleSelfDrive74();return true;}
 if(Action==TEXT("climate74")){Record()->CabinLights66=!Record()->CabinLights66;P->Notify(Record()->CabinLights66?TEXT("AMBIENT LIGHTING ON"):TEXT("AMBIENT LIGHTING OFF"));P->RequestSave40();return true;}
 return false;
}

void ALWVehicle::BuildElectric74(){
 if(!IsElectric74())return;
 const bool Coach=IsSolarRV74();
 if(Coach)Root->SetRelativeScale3D(LWElectric74::CabinScale(true));
 Body->EmptyOverrideMaterials();Body->SetStaticMesh(World->Mesh(Coach?TEXT("EV74_CoachShell"):TEXT("EV74_HyperShell")));
 Body->SetCollisionEnabled(ECollisionEnabled::QueryOnly);Body->SetCollisionResponseToAllChannels(ECR_Ignore);Body->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 PaintWet.Empty();for(int I=0;I<Body->GetNumMaterials();I++)if(Body->GetMaterial(I)&&Body->GetMaterial(I)->GetName().Contains(TEXT("EV74_Pearl"))){
  // Coach panels use neutral painted composite rather than strongly tinted metal.
  // Bind before dent copying; ApplyGarage45 still applies saved custom finishes.
  if(Coach)if(auto* Paint=World->Material(TEXT("EV83_CoachPearl")))Body->SetMaterial(I,Paint);
  PaintWet.Add(Body->CreateDynamicMaterialInstance(I));
 }
 for(auto& C:Details){if(!C||!C->GetStaticMesh())continue;const FString Name=C->GetStaticMesh()->GetName();
  if(Name.StartsWith(TEXT("SM_Dial42"))||GaugeNeedles42.Contains(C)||GaugeLamps42.Contains(C)){C->SetVisibility(false);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
  if(!Coach&&Name.StartsWith(TEXT("SM_Cabin42"))){C->SetVisibility(false);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
  if(Coach&&Name==TEXT("SM_RV66_Interior")){C->SetStaticMesh(World->Mesh(TEXT("EV74_CoachInterior")));C->EmptyOverrideMaterials();}
  if(Coach)for(int M=0;M<C->GetNumMaterials();M++)if(auto* Mat=C->GetMaterial(M)){FString N=Mat->GetName();if(N.Contains(TEXT("RV66_Ivory")))C->SetMaterial(M,World->Material(Name.Contains(TEXT("Chair"))||Name.Contains(TEXT("Sofa"))||Name.Contains(TEXT("Dinette"))?TEXT("EV74_Leather"):TEXT("EV74_Ivory")));else if(N.Contains(TEXT("RV66_Linen")))C->SetMaterial(M,World->Material(TEXT("EV74_Leather")));else if(N.Contains(TEXT("RV66_Brass")))C->SetMaterial(M,World->Material(TEXT("EV74_Titanium")));else if(N.Contains(TEXT("RV66_Pearl")))C->SetMaterial(M,World->Material(TEXT("EV83_CoachPearl")));}
 }
 GaugeNeedles42.Empty();GaugeLamps42.Empty();if(GearDisplay42)GearDisplay42->SetVisibility(false);
 auto Add=[&](FName Mesh,FVector At,FName Tag=NAME_None,FRotator Rot=FRotator::ZeroRotator){Part(Mesh,At,FVector(1),Rot);auto* C=Details.Last().Get();C->SetCollisionEnabled(ECollisionEnabled::QueryOnly);C->SetCollisionResponseToAllChannels(ECR_Ignore);C->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);if(!Tag.IsNone())C->ComponentTags.Add(Tag);return C;};
 if(!Coach){
  Add(TEXT("EV74_HyperInterior"),FVector::ZeroVector);
  for(auto& W:Wheels){W->SetStaticMesh(World->Mesh(TEXT("EV74_Wheel")));W->EmptyOverrideMaterials();W->SetRelativeScale3D(FVector(1));FVector At=W->GetRelativeLocation();At.Z=35.3;W->SetRelativeLocation(At);}
  if(Windshield){Windshield->SetStaticMesh(World->Mesh(TEXT("EV74_HyperGlass")));Windshield->SetRelativeTransform(FTransform::Identity);Windshield->SetMaterial(0,World->Material(TEXT("EV74_Window")));GlassWet=Windshield->CreateDynamicMaterialInstance(0);}
  for(auto& W:Wipers){W->SetRelativeLocation(FVector(67,W->GetRelativeLocation().Y*.7f,93));W->SetRelativeScale3D(FVector(.4,.4,.4));}
  GloveLid->SetRelativeLocation(FVector(20,54,65));
 }
 else if(Windshield){Windshield->SetMaterial(0,World->Material(TEXT("EV74_Window")));GlassWet=Windshield->CreateDynamicMaterialInstance(0);}
 SteeringWheel->SetStaticMesh(World->Mesh(TEXT("EV74_Yoke")));SteeringWheel->EmptyOverrideMaterials();SteeringWheel->SetRelativeLocation(Coach?FVector(117,-55,170):FVector(-5,-43,84));
 const FVector ScreenAt=Coach?FVector(143,-4,190):FVector(20,0,100);
 Add(TEXT("EV74_DashFrame"),ScreenAt,TEXT("autopilot74"));
 auto* Display=Add(TEXT("EV74_Display"),ScreenAt-FVector(.8,0,0),TEXT("autopilot74"));
 DashTarget74=NewObject<UTextureRenderTarget2D>(this);DashTarget74->RenderTargetFormat=RTF_RGBA8;DashTarget74->ClearColor=FLinearColor(.014,.02,.026);DashTarget74->InitAutoFormat(1024,640);DashTarget74->UpdateResourceImmediate();
 DashMaterial74=UMaterialInstanceDynamic::Create(World->Material(TEXT("EV74_Display")),this);DashMaterial74->SetTextureParameterValue(TEXT("DisplayTexture"),DashTarget74);Display->SetMaterial(0,DashMaterial74);
 if(Coach){
  Add(TEXT("EV74_ControlPanel"),FVector(-72,117,187),TEXT("autopilot74"),FRotator(0,90,0));
  Add(TEXT("EV74_ControlPanel"),FVector(-419,-116,198),TEXT("climate74"),FRotator(0,-90,0));
  Add(TEXT("EV74_Coffee"),FVector(-490,-87,154),TEXT("cooker"));
  Add(TEXT("EV74_Spa"),FVector(-579,94,65),TEXT("faucet66"));
  for(int I=0;I<CabinLamps66.Num();I++)CabinLamps66[I]->SetLightColor(FLinearColor(.72,.86,1));
 }
 DrawDashboard74();
}

void ALWVehicle::TickElectric74(float Dt){
 if(!IsElectric74())return;CabinPitch74=FMath::FInterpTo(CabinPitch74,GetActorRotation().Pitch,Dt,5);
 DashClock74-=Dt;if(DashClock74<=0){DashClock74=.1f;if(Driver)DrawDashboard74();}
}
void ALWVehicle::DrawDashboard74(){
 if(!DashTarget74||!Record()||!World)return;
 UCanvas* C=nullptr;FVector2D Size;FDrawToRenderTargetContext Context;UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this,DashTarget74,C,Size,Context);
 if(C){
  auto Rect=[&](float X,float Y,float W,float H,FLinearColor Color){FCanvasTileItem T(FVector2D(X,Y),FVector2D(W,H),Color);T.BlendMode=SE_BLEND_Opaque;C->DrawItem(T);};
  auto Text=[&](FString S,float X,float Y,float Scale=1.5f,FLinearColor Col=FLinearColor(.8,.89,.91)){C->K2_DrawText(GEngine->GetLargeFont(),S,FVector2D(X,Y),FVector2D(Scale),Col,0,FLinearColor::Transparent,FVector2D::ZeroVector);};
  const FLinearColor Mint(.2,.95,.73),Dim(.11,.17,.20);Rect(0,0,1024,640,FLinearColor(.012,.018,.023));
  Text(IsSolarRV74()?TEXT("SOLSTICE  /  RESIDENCE"):TEXT("APEX  /  ELECTRIC"),28,20,1.05f);
  Text(FString::Printf(TEXT("%02d:%02d"),int(World->TimeOfDay)%24,int(World->TimeOfDay*60)%60),858,20,1.3);
  C->K2_DrawLine(FVector2D(28,66),FVector2D(996,66),1,Dim);
  Text(FString::Printf(TEXT("%03d"),FMath::RoundToInt(FMath::Abs(Speed)*.0223694f)),30,85,5.2,Mint);Text(TEXT("MPH"),40,224,1.25);
  Text(!EngineOn?TEXT("PARK"):Speed< -5?TEXT("REVERSE"):TEXT("DRIVE"),165,232,1.1);
  const float Charge=BatteryCharge74()/BatteryCapacity74();Rect(32,290,270,12,Dim);Rect(32,290,270*Charge,12,Charge>.15?Mint:FLinearColor(1,.3,.1));
  Text(FString::Printf(TEXT("%.0f%%  |  %.0f kWh"),Charge*100,BatteryCharge74()),32,318,1.1);
  Text(FString::Printf(TEXT("SOLAR  +%.1f kW"),SolarKW74),32,369,1.05f,Mint);Text(FString::Printf(TEXT("%s  %.0f kW"),PowerKW74<0?TEXT("REGEN"):TEXT("DRAW"),FMath::Abs(PowerKW74)),32,405,1.05f);
  const TCHAR* Weather[]={TEXT("CLEAR"),TEXT("CLOUDY"),TEXT("FOG"),TEXT("RAIN"),TEXT("THUNDERSTORM"),TEXT("DUST STORM"),TEXT("TORNADO"),TEXT("DENSE FOG"),TEXT("NUCLEAR WINTER"),TEXT("ASH SNOW"),TEXT("BLIZZARD")};
  Text(Weather[FMath::Clamp(World->WeatherType,0,10)],32,468,1.0);
  Rect(340,84,656,428,FLinearColor(.027,.043,.050));
  const FVector2D Here(GetActorLocation());const double Range=15000;
  if(FVector2D::Distance(Here,DashRoadCenter74)>4000){DashRoadCenter74=Here;DashRoads74.Empty();TArray<LWGen::FRoad> Roads;TArray<LWGen::FSite> Sites;LWGen::Gather(Here,World->Seed,Roads,Sites);for(const auto& R:Roads){DashRoads74.Add(R.A);DashRoads74.Add(R.B);}}
  auto Project=[&](FVector2D V){return FVector2D(668+(V.Y-Here.Y)/Range*300,298-(V.X-Here.X)/Range*196);};
  auto Line=[&](FVector2D A,FVector2D B,float W,FLinearColor Col){A=Project(A);B=Project(B);double Lo=0,Hi=1;FVector2D D=B-A;auto Clip=[&](double P,double Q){if(FMath::Abs(P)<1.e-8)return Q>=0;double T=Q/P;if(P<0)Lo=FMath::Max(Lo,T);else Hi=FMath::Min(Hi,T);return Lo<=Hi;};if(Clip(-D.X,A.X-344)&&Clip(D.X,992-A.X)&&Clip(-D.Y,A.Y-88)&&Clip(D.Y,508-A.Y))C->K2_DrawLine(A+D*Lo,A+D*Hi,W,Col);};
  for(int I=0;I+1<DashRoads74.Num();I+=2)Line(DashRoads74[I],DashRoads74[I+1],2,FLinearColor(.15,.25,.29));
  for(int I=FMath::Max(1,DriveStep);I<DrivePath.Num();I++)Line(DrivePath[I-1],DrivePath[I],4,Mint);
  if(Driver&&Driver->bWaypoint){auto Target=Project(Driver->Waypoint);if(Target.X>350&&Target.X<980&&Target.Y>90&&Target.Y<490)Text(TEXT("+"),Target.X-8,Target.Y-12,1.2,Mint);Text(FString::Printf(TEXT("DESTINATION  %.2f km"),FVector2D::Distance(Here,Driver->Waypoint)/100000.),361,469,.95f);}
  else Text(TEXT("SET A WAYPOINT ON YOUR MAP"),360,469,.8f);
  const FVector2D Nose(GetActorForwardVector());Line(Here-Nose*480,Here+Nose*480,7,FLinearColor::White);Text(TEXT("N"),958,99,.9f);
  Rect(28,546,968,66,SelfDriving74?FLinearColor(.03,.22,.18):Dim);
  Text(StopRequested?TEXT("AUTOPILOT  /  STOPPING"):SelfDriving74?TEXT("AUTOPILOT ACTIVE  /  USE SCREEN TO STOP"):TEXT("USE SCREEN  /  ENGAGE AUTOPILOT"),46,566,1.05f,Mint);
 }
 UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this,Context);
}

void ALWVehicle::TickElectricAudio74(float Dt,bool Brake,float Gas){
 const float V=FMath::Abs(Speed),Rev=FMath::Clamp(V/Spec().MaxSpeed,0.f,1.f);
 if(EngineOn!=AudioWasEngine)VehicleSound42(TEXT("EV74_Ready"),.45f);AudioWasEngine=EngineOn;
 const FName Slots[]={IsSolarRV74()?FName(TEXT("EV74_CoachMotor")):FName(TEXT("EV74_HyperMotor")),TEXT("CarTires"),TEXT("CarWind42"),TEXT("CarWipers"),TEXT("CarBrake")};
 if(DriveAudio.Num()!=5){StopDriveAudio();DriveAudio.SetNum(5);DriveAudioGain.SetNumZeroed(10);}
 const float Levels[]={EngineOn?(.23f+.66f*Rev+.3f*FMath::Abs(Gas)):0,Rev*.58f,Rev*Rev*.38f,WipersOn?.38f:0,Brake&&V>60?.3f:0};
 for(int I=0;I<5;I++){auto& A=DriveAudio[I];if(!IsValid(A)&&Levels[I]>.001f){A=World->Sound(Slots[I],GetActorLocation(),1);if(A){DriveAudioGain[I]=A->VolumeMultiplier;DriveAudioGain[I+5]=A->PitchMultiplier;A->SetVolumeMultiplier(0);A->AttachToComponent(Root,FAttachmentTransformRules::KeepWorldTransform);}}
  if(IsValid(A)){A->SetVolumeMultiplier(FMath::FInterpTo(A->VolumeMultiplier,DriveAudioGain[I]*Levels[I],Dt,6));A->SetPitchMultiplier(DriveAudioGain[I+5]*(I==0?.7f+Rev*1.8f:1.f));A->SetLowPassFilterEnabled(false);}}
 WasBrake42=Brake;EngineRPM=Rev*16000;AudioGear=1;
 if(!EngineOn&&V<1&&!WipersOn)StopDriveAudio();
}
