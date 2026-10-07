#include "LWAircraft84.h"
#include "LWVehicle.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "CanvasItem.h"
#include "Engine/World.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/PlayerController.h"

FVector LWAviation84::Seat(FName Model,int I){const auto S=Spec(Model);if(I<1)return FVector(S.HalfCabin+10,I<0?-55:55,S.Floor+48);
 if(Model==TEXT("airbus")){const int N=I-1;const float Y[]={-141,-91,-41,85,135};return FVector(1110-(N/5)*110,Y[N%5],S.Floor+48);}
 if(Model==TEXT("private_jet"))return FVector(480-((I-1)/2)*220,(I%2?-1:1)*76,S.Floor+48);
 if(I==14)return FVector(-820,-110,S.Floor+48);
 return FVector(1070-((I-1)/2)*155,(I%2?-1:1)*104,S.Floor+48);
}
FVector ALWVehicle::AircraftEye84()const{return PlayerSeat==-2?CabinEye:LWAviation84::Seat(Spec().Id,PlayerSeat)+FVector(0,0,83);}
void ALWVehicle::BuildAircraft84(){
 const auto S=LWAviation84::Spec(Spec().Id);const bool Jet=FName(Spec().Id)==TEXT("private_jet"),Luxury=FName(Spec().Id)!=TEXT("airbus");
 for(auto& M:Details)if(M)M->DestroyComponent();Details.Empty();AircraftSeats84.Empty();AircraftSeatBacks84.Empty();SuiteDoors84.Empty();AircraftShades84.Empty();BinLids84.Empty();AircraftScreens84.Empty();AircraftSeatRest84.Empty();CabinObstacles84.Empty();Root->SetRelativeTransform(FTransform::Identity);
 Chassis->SetBoxExtent(FVector(Spec().HalfLength-60,Jet?140:200,Jet?160:225));Chassis->SetCollisionEnabled(ECollisionEnabled::QueryOnly);Chassis->SetCollisionResponseToAllChannels(ECR_Ignore);Chassis->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 Body->SetStaticMesh(World->Mesh(Jet?TEXT("AV84_JetShell"):Luxury?TEXT("AV84_AirbusLuxuryShell"):TEXT("AV84_AirbusShell")));Body->SetRelativeTransform(FTransform::Identity);Body->EmptyOverrideMaterials();Body->SetCollisionEnabled(ECollisionEnabled::QueryOnly);Body->SetCollisionResponseToAllChannels(ECR_Block);
 auto Add=[&](FName Mesh,FVector At,FName Tag=NAME_None,FRotator Rot=FRotator::ZeroRotator,FVector Scale=FVector(1)){Part(Mesh,At,Scale,Rot);auto* M=Details.Last().Get();M->SetCollisionEnabled(Tag.IsNone()?ECollisionEnabled::NoCollision:ECollisionEnabled::QueryOnly);M->SetCollisionResponseToAllChannels(ECR_Ignore);M->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);M->SetCanEverAffectNavigation(false);if(!Tag.IsNone())M->ComponentTags.Add(Tag);return M;};
 GearMesh84=Add(Jet?TEXT("AV84_JetGear"):TEXT("AV84_AirbusGear"),FVector::ZeroVector);
 AircraftDoor84=Add(Jet?TEXT("AV84_JetDoor"):TEXT("AV84_AirbusDoor"),FVector(Spec().HalfLength-490,Jet?140:192,S.Floor),TEXT("entry84"));
 Add(TEXT("AV84_Cockpit"),FVector(S.HalfCabin+100,0,S.Floor),TEXT("flight84"));
 // Three forward-facing displays share one live render target, with no backwards text.
 FlightDisplay84=NewObject<UTextureRenderTarget2D>(this);FlightDisplay84->RenderTargetFormat=RTF_RGBA8;FlightDisplay84->ClearColor=FLinearColor(.008,.014,.022);FlightDisplay84->InitAutoFormat(1024,640);FlightDisplay84->UpdateResourceImmediate();
 auto* ScreenMat=UMaterialInstanceDynamic::Create(World->Material(TEXT("EV74_Display")),this);ScreenMat->SetTextureParameterValue(TEXT("DisplayTexture"),FlightDisplay84);
 for(float Y:{-60.f,0.f,60.f}){auto* M=Add(TEXT("EV74_Display"),FVector(S.HalfCabin+76,Y,S.Floor+99),Y==0?FName(TEXT("flight84")):FName(TEXT("gear84")),FRotator::ZeroRotator,FVector(1,.8,.8));M->SetMaterial(0,ScreenMat);}
 for(int I=-1;I<S.Seats-1;I++){FVector At=LWAviation84::Seat(Spec().Id,I)-FVector(0,0,48);const bool Couch=Spec().Id==FName(TEXT("luxury_airbus"))&&I==14;auto* M=Add(Couch?TEXT("AV84_Couch"):Luxury?TEXT("AV84_LuxurySeat"):TEXT("AV84_Seat"),At,Couch?FName(TEXT("couch84")):FName(*FString::Printf(TEXT("seat84_%d"),I)),FRotator(0,Couch?90:0,0));AircraftSeats84.Add(M);AircraftSeatBacks84.Add(Couch?nullptr:Add(Luxury?TEXT("AV84_LuxurySeatBack"):TEXT("AV84_SeatBack"),At+FVector(-20,0,55),FName(*FString::Printf(TEXT("seat84_%d"),I))));AircraftSeatRest84.Add(At);if(Couch)CabinObstacles84.Add(M->GetStaticMesh()->GetBoundingBox().TransformBy(M->GetRelativeTransform()));else if(I>0)CabinObstacles84.Add(FBox(At+FVector(-38,Luxury?-37:-24,0),At+FVector(35,Luxury?37:24,145)));}
 int Window=0;const int WindowRows=Jet?10:24;for(int Row=0;Row<WindowRows;Row++)for(int Side:{-1,1}){
  float X=Jet?-690+Row*150:-1150+Row*100;FVector At(X,Side*(Jet?143:196),S.Floor+(Jet?112:128));
  auto Tag=FName(*FString::Printf(TEXT("shade84_%d"),Window++));Add(Jet?TEXT("AV84_JetWindow"):TEXT("AV84_AirbusWindow"),At,Tag,FRotator(0,Side<0?180:0,0),FVector(Jet?1.5:1,1,1));AircraftShades84.Add(Add(Jet?TEXT("AV84_JetShade"):TEXT("AV84_AirbusShade"),At+FVector(0,-Side*3,0),Tag,FRotator(0,Side<0?180:0,0),FVector(Jet?1.5:1,1,1)));
 }
 int Bin=0;for(int Row=0;Row<(Jet?4:20);Row++)for(int Side:{-1,1}){FVector At(Jet?-350+Row*160:-1050+Row*110,Side*(Jet?78:126),S.Floor+178);FName Tag(*FString::Printf(TEXT("bin84_%d"),Bin++));Add(TEXT("AV84_Bin"),At,Tag,FRotator(0,Side<0?180:0,0));BinLids84.Add(Add(TEXT("AV84_BinLid"),At,Tag,FRotator(0,Side<0?180:0,0)));}
 auto Furniture=[&](FName Mesh,FVector At,FName Tag,FRotator Rot=FRotator::ZeroRotator){auto* M=Add(Mesh,At,Tag,Rot);if(M->GetStaticMesh())CabinObstacles84.Add(M->GetStaticMesh()->GetBoundingBox().TransformBy(M->GetRelativeTransform()));};
 if(Luxury){const float Back=Jet?-610:-1140;if(Jet)Furniture(TEXT("AV84_Bed"),FVector(Back,0,S.Floor),TEXT("bed84"));Furniture(TEXT("AV84_Bar"),FVector(Jet?-200:-260,-S.Width+28,S.Floor),TEXT("bar84"));Furniture(TEXT("AV84_Fridge"),FVector(Jet?-390:-600,S.Width-35,S.Floor),TEXT("fridge84"),FRotator(0,90,0));
  if(!Jet){for(float Y:{-98.f,98.f})Furniture(TEXT("AV84_Bed"),FVector(-1210,Y,S.Floor),FName(Y<0?TEXT("suite84_1"):TEXT("suite84_2")));}
  if(!Jet){auto Wall=[&](FVector At,FVector Size){auto* M=Add(TEXT("Cube"),At,NAME_None,FRotator::ZeroRotator,Size/100);M->SetMaterial(0,World->Material(TEXT("AV84_Walnut")));CabinObstacles84.Add(FBox(At-Size*.5,At+Size*.5));};Wall(FVector(-1260,0,S.Floor+105),FVector(360,6,210));for(float Y:{-155.f,0.f,155.f})Wall(FVector(-1080,Y,S.Floor+105),FVector(6,Y==0?118:42,210));for(int I=0;I<2;I++)SuiteDoors84.Add(Add(TEXT("AV84_SuiteDoor"),FVector(-1080,I?130:-60,S.Floor),FName(*FString::Printf(TEXT("suite_door84_%d"),I))));}
  auto* TV=Add(TEXT("AV84_TV"),FVector(Jet?-390:-980,0,S.Floor+180),TEXT("tv84"),FRotator(0,180,0));TVMaterials66.Add(UMaterialInstanceDynamic::Create(World->Material(TEXT("EV74_Display")),this));TVMaterials66.Last()->SetTextureParameterValue(TEXT("DisplayTexture"),FlightDisplay84);auto* Display=Add(TEXT("EV74_Display"),TV->GetRelativeLocation()+FVector(4,0,0),TEXT("tv84"),FRotator(0,180,0),FVector(1,1.5,1.5));Display->SetMaterial(0,TVMaterials66.Last());AircraftScreens84.Add(Display);
  TVTarget66=NewObject<UTextureRenderTarget2D>(this);TVTarget66->RenderTargetFormat=RTF_RGBA8;TVTarget66->InitAutoFormat(512,288);TVTarget66->UpdateResourceImmediate();TVCamera66=NewObject<USceneCaptureComponent2D>(this);TVCamera66->SetupAttachment(Root);TVCamera66->SetRelativeLocation(FVector(0,0,S.Floor+340));TVCamera66->SetRelativeRotation(FRotator(-18,0,0));TVCamera66->FOVAngle=95;TVCamera66->bCaptureEveryFrame=false;TVCamera66->bCaptureOnMovement=false;TVCamera66->CaptureSource=ESceneCaptureSource::SCS_FinalColorLDR;TVCamera66->TextureTarget=TVTarget66;TVCamera66->HiddenActors.Add(this);TVCamera66->MaxViewDistanceOverride=1800000;TVCamera66->RegisterComponent();
 }
 Add(TEXT("EV74_ControlPanel"),FVector(S.HalfCabin-70,Jet?125:170,S.Floor+120),TEXT("cabin84"),FRotator(0,90,0),FVector(1.4));
 for(float X=-S.HalfCabin;X<S.HalfCabin;X+=500){auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(Root);L->SetRelativeLocation(FVector(X,0,S.Floor+215));L->SetAttenuationRadius(650);L->SetIntensity(2200);L->SetCastShadows(false);L->RegisterComponent();CabinLamps66.Add(L);Add(TEXT("Cube"),FVector(X,0,S.Floor+223),NAME_None,FRotator::ZeroRotator,FVector(2.5,.08,.03))->SetMaterial(0,World->Material(TEXT("AV84_Light")));}
 EngineOn=Record()->FlightEngine84;Speed=Record()->FlightVelocity84.Size();GearAlpha84=Record()->Gear84?1:0;AircraftOnGround84=GetActorLocation().Z< LWAviation84::Surface(FVector2D(GetActorLocation()),World->HeightAt(FVector2D(GetActorLocation())))+80;
 TickAircraftCabin84(0);DrawFlightDisplay84();
}
FName ALWVehicle::AircraftFocus84(const ALWCharacter* P)const{if(!P)return NAME_None;FCollisionQueryParams Q(NAME_None,true,P);Q.AddIgnoredComponent(Chassis.Get());FHitResult Hit;FVector Start=P->Camera->GetComponentLocation();if(GetWorld()->LineTraceSingleByChannel(Hit,Start,Start+P->Camera->GetForwardVector()*650,ECC_Visibility,Q)&&Hit.GetActor()==this&&Hit.GetComponent())for(FName Tag:Hit.GetComponent()->ComponentTags)if(!Tag.IsNone())return Tag;
 if(!P->Vehicle&&AircraftOnGround84)return TEXT("entry84");return NAME_None;}
FString ALWVehicle::AircraftPrompt84(FName A)const{FString S=A.ToString();if(A==TEXT("entry84"))return AircraftDoorOpen84?TEXT("EXIT AIRCRAFT"):TEXT("OPEN / BOARD AIRCRAFT");if(S.StartsWith(TEXT("seat84_"))){int I=FCString::Atoi(*S.Mid(7));return I<0?TEXT("PILOT SEAT"):FString::Printf(TEXT("SEAT %d / SIT OR RECLINE"),I+1);}if(S.StartsWith(TEXT("shade84_")))return TEXT("WINDOW SHADE");if(S.StartsWith(TEXT("bin84_")))return TEXT("CARRY-ON STORAGE");if(A==TEXT("flight84"))return TEXT("FLIGHT COMPUTER / AUTOPILOT");if(A==TEXT("gear84"))return Record()&&Record()->Gear84?TEXT("RETRACT LANDING GEAR"):TEXT("LOWER LANDING GEAR");if(A==TEXT("cabin84"))return TEXT("CABIN CONTROLS");if(S.StartsWith(TEXT("suite_door84_")))return TEXT("SUITE DOOR");if(A==TEXT("bed84")||S.StartsWith(TEXT("suite84")))return TEXT("PRIVATE BED");if(A==TEXT("bar84"))return TEXT("MINIBAR");if(A==TEXT("fridge84"))return TEXT("REFRIGERATOR");if(A==TEXT("tv84"))return TEXT("TV / FLIGHT INFORMATION");if(A==TEXT("couch84"))return TEXT("LOUNGE SEATING");return S.ToUpper();}
void ALWVehicle::OpenAircraftStorage84(ALWCharacter* P,FName Action){FName Id(*(RecordId.ToString()+TEXT("_")+Action.ToString()));auto& R=World->Containers.FindOrAdd(Id);R.Id=Id;R.Context=Action;R.Width=8;R.Height=Action.ToString().StartsWith(TEXT("bin"))?6:10;R.Unlocked=true;R.Position=GetActorLocation();
 if(CargoObject&&CargoObject->RecordId!=Id){CargoObject->Destroy();CargoObject=nullptr;}if(!CargoObject){CargoObject=World->SpawnObject(ELWObjectKind::Container,Id,P->GetActorLocation());if(!CargoObject)return;CargoObject->SetActorHiddenInGame(true);CargoObject->SetActorEnableCollision(false);CargoObject->SetActorTickEnabled(false);}CargoObject->SetActorLocation(P->GetActorLocation());P->OpenContainer(CargoObject);}
bool ALWVehicle::ChangeAircraftSeat84(int Seat){if(!Driver||Seat < -1||Seat>=Spec().Seats-1)return false;if((!AircraftOnGround84||Speed>50)&&!Record()->Autopilot84&&Seat!=-1){Driver->Notify(TEXT("ENGAGE AUTOPILOT BEFORE LEAVING THE CONTROLS"));return false;}if(Seat>=0&&Passengers.IsValidIndex(Seat)&&IsValid(Passengers[Seat]))return false;if(Seat!=-1&&!Record()->Autopilot84)Record()->Thrust84=0;PlayerSeat=Seat;Driver->SeatPitch=0;Driver->SeatYaw=Spec().Id==FName(TEXT("luxury_airbus"))&&Seat==14?90:0;CabinMove=FVector2D::ZeroVector;CabinVelocity74=FVector::ZeroVector;Driver->TickVehicleSeat();return true;}
void ALWVehicle::StandAircraft84(){if(!Driver)return;if((!AircraftOnGround84||Speed>50)&&!Record()->Autopilot84){Driver->Notify(TEXT("STOP OR ENGAGE AUTOPILOT BEFORE WALKING IN THE CABIN"));return;}const auto S=LWAviation84::Spec(Spec().Id);CabinEye=FVector(FMath::Clamp(float(PlayerEye().X),-S.HalfCabin,S.HalfCabin-90),22,S.Floor+164);if(!Record()->Autopilot84)Record()->Thrust84=0;PlayerSeat=-2;CabinMove=FVector2D::ZeroVector;CabinVelocity74=FVector::ZeroVector;Driver->TickVehicleSeat();}
bool ALWVehicle::UseAircraft84(ALWCharacter* P,FName A){if(!P||!Record()||Record()->Exploded)return false;const FString Name=A.ToString();
 if(A==TEXT("entry84")){if(!AircraftOnGround84||Speed>30){P->Notify(TEXT("LAND AND PARK BEFORE OPENING THE DOOR"));return true;}if(!P->Vehicle){AircraftDoorOpen84=true;Enter(P,-2);}else if(AircraftDoorOpen84){Exit(false);}else{AircraftDoorOpen84=true;}return true;}
 if(P->Vehicle!=this)return false;
 if(A==TEXT("flight84")||A==TEXT("feature")){P->ClosePanels();P->RPGPanel=4;P->DialogueText=TEXT("Flight computer");P->DialogueChoices={Record()->Autopilot84?TEXT("Disengage autopilot"):TEXT("Engage autopilot"),TEXT("Claim aircraft"),TEXT("Start / stop engines"),TEXT("Leave")};P->DialogueActions={TEXT("air84_auto"),TEXT("air84_claim"),TEXT("air84_engine"),TEXT("bye")};P->SetMenuInput(true);return true;}
 if(A==TEXT("cabin84")){P->ClosePanels();P->RPGPanel=4;P->DialogueText=TEXT("Cabin controls");P->DialogueChoices={TEXT("Brightness +"),TEXT("Brightness −"),TEXT("Warm white"),TEXT("Cool white"),TEXT("Blue"),TEXT("Violet"),TEXT("Amber"),TEXT("Red +"),TEXT("Red −"),TEXT("Green +"),TEXT("Green −"),TEXT("Blue +"),TEXT("Blue −"),TEXT("Open all shades"),TEXT("Close all shades"),TEXT("Leave")};P->DialogueActions={TEXT("air84_brighter"),TEXT("air84_dimmer"),TEXT("air84_warm"),TEXT("air84_cool"),TEXT("air84_blue"),TEXT("air84_violet"),TEXT("air84_amber"),TEXT("air84_redplus"),TEXT("air84_redminus"),TEXT("air84_greenplus"),TEXT("air84_greenminus"),TEXT("air84_blueplus"),TEXT("air84_blueminus"),TEXT("air84_shadesopen"),TEXT("air84_shadesclose"),TEXT("bye")};P->SetMenuInput(true);return true;}
 if(A==TEXT("glove")||A==TEXT("gear84")){if(AircraftOnGround84){P->Notify(TEXT("GEAR LOCKED WHILE ON THE GROUND"));return true;}Record()->Gear84=!Record()->Gear84;return true;}
 if(Name.StartsWith(TEXT("seat84_"))){int I=FCString::Atoi(*Name.Mid(7));if(PlayerSeat==I){if(Record()->Reclined84.Contains(I))Record()->Reclined84.Remove(I);else Record()->Reclined84.Add(I);}else ChangeAircraftSeat84(I);return true;}
 if(Name.StartsWith(TEXT("suite_door84_"))){int I=FCString::Atoi(*Name.Mid(13));if(Record()->OpenSuites84.Contains(I))Record()->OpenSuites84.Remove(I);else Record()->OpenSuites84.Add(I);return true;}
 if(Name.StartsWith(TEXT("shade84_"))){int I=FCString::Atoi(*Name.Mid(8));if(Record()->Shades66.Contains(I))Record()->Shades66.Remove(I);else Record()->Shades66.Add(I);return true;}
 if(Name.StartsWith(TEXT("bin84_"))){int I=FCString::Atoi(*Name.Mid(6));if(Record()->Bins84.Contains(I))Record()->Bins84.Remove(I);else{Record()->Bins84.Add(I);OpenAircraftStorage84(P,A);}return true;}
 if(A==TEXT("bar84")||A==TEXT("fridge84")||A==TEXT("cargo")){OpenAircraftStorage84(P,A);return true;}
 if(A==TEXT("bed84")||Name.StartsWith(TEXT("suite84"))){P->OpenBedMenu(this);return true;}
 if(A==TEXT("tv84")){Record()->TV66=(Record()->TV66+1)%3;return true;}
 if(A==TEXT("couch84")){ChangeAircraftSeat84(Spec().Seats-2);return true;}
 if(A==TEXT("lights")){Record()->CabinLights66=!Record()->CabinLights66;return true;}return false;
}
void ALWVehicle::TickAircraftCabin84(float Dt){if(!Record())return;const auto S=LWAviation84::Spec(Spec().Id);
 GearAlpha84=FMath::FInterpConstantTo(GearAlpha84,Record()->Gear84?1.f:0.f,Dt,.22f);if(GearMesh84){GearMesh84->SetRelativeLocation(FVector(0,0,(1-GearAlpha84)*200));GearMesh84->SetVisibility(GearAlpha84>.05f);}if(AircraftDoor84)AircraftDoor84->SetRelativeRotation(FRotator(0,AircraftDoorOpen84?110:0,0));
 for(int I=0;I<AircraftShades84.Num();I++)AircraftShades84[I]->SetVisibility(Record()->Shades66.Contains(I));for(int I=0;I<BinLids84.Num();I++){FRotator R=BinLids84[I]->GetRelativeRotation();R.Roll=Record()->Bins84.Contains(I)?-70:0;BinLids84[I]->SetRelativeRotation(R);}for(int I=0;I<AircraftSeatBacks84.Num();I++)if(AircraftSeatBacks84[I])AircraftSeatBacks84[I]->SetRelativeRotation(FRotator(Record()->Reclined84.Contains(I-1)?-12:0,0,0));
 for(int I=0;I<SuiteDoors84.Num();I++)SuiteDoors84[I]->SetRelativeRotation(FRotator(0,Record()->OpenSuites84.Contains(I)?90:0,0));
 for(auto& L:CabinLamps66)if(L){L->SetIntensity(3000*Record()->CabinBrightness84);L->SetLightColor(Record()->CabinColor84);L->SetVisibility(Record()->CabinLights66);}for(auto& Screen:AircraftScreens84)if(Screen)Screen->SetVisibility(Record()->TV66!=0);
 ScreenClock66-=Dt;if(ScreenClock66<=0){ScreenClock66=.25f;for(auto& M:TVMaterials66)if(M)M->SetTextureParameterValue(TEXT("DisplayTexture"),Record()->TV66==2?TVTarget66.Get():FlightDisplay84.Get());if(Record()->TV66==2&&Driver&&TVCamera66)TVCamera66->CaptureScene();}
 if(!Driver||PlayerSeat!=-2)return;
 // Integrate entirely in airframe space. The camera inherits exactly one current
 // transform after flight, so floating origin shifts and bank never add drift.
 const FVector Input=FRotator(0,Driver->SeatYaw,0).RotateVector(FVector(CabinMove.X,CabinMove.Y,0).GetClampedToMaxSize(1));const bool Block=Driver->IsUIOpen();CabinVelocity74=FMath::VInterpTo(CabinVelocity74,Block?FVector::ZeroVector:Input*190,Dt,10);
 float Remaining=FMath::Min(Dt,.25f);while(Remaining>0){float Step=FMath::Min(Remaining,1.f/90);Remaining-=Step;FVector Delta=CabinVelocity74*Step;for(int Axis=0;Axis<2;Axis++){FVector Candidate=CabinEye;Candidate[Axis]+=Delta[Axis];Candidate.X=FMath::Clamp(Candidate.X,-S.HalfCabin+65,S.HalfCabin-65);Candidate.Y=FMath::Clamp(Candidate.Y,-S.Width+25,S.Width-25);bool Hit=false;for(const auto& B:CabinObstacles84){FBox Pad=B.ExpandBy(FVector(21,21,0));if(Candidate.X>Pad.Min.X&&Candidate.X<Pad.Max.X&&Candidate.Y>Pad.Min.Y&&Candidate.Y<Pad.Max.Y&&Pad.Min.Z<S.Floor+160&&Pad.Max.Z>S.Floor+12){Hit=true;break;}}for(int I=0;I<SuiteDoors84.Num();I++)if(!Record()->OpenSuites84.Contains(I)){const FVector H=SuiteDoors84[I]->GetRelativeLocation();if(FMath::Abs(Candidate.X-H.X)<24&&Candidate.Y>H.Y-91&&Candidate.Y<H.Y+21)Hit=true;}if(!Hit)CabinEye=Candidate;}CabinEye.Z=S.Floor+164;}
}
void ALWVehicle::DrawFlightDisplay84(){if(!FlightDisplay84||!Record())return;UCanvas* C=nullptr;FVector2D Size;FDrawToRenderTargetContext Context;UKismetRenderingLibrary::BeginDrawCanvasToRenderTarget(this,FlightDisplay84,C,Size,Context);if(C){auto Rect=[&](float X,float Y,float W,float H,FLinearColor Color){FCanvasTileItem T(FVector2D(X,Y),FVector2D(W,H),Color);T.BlendMode=SE_BLEND_Opaque;C->DrawItem(T);};auto Text=[&](FString V,float X,float Y,float Scale=1.25,FLinearColor Color=FLinearColor(.7,.9,.94)){C->K2_DrawText(GEngine->GetLargeFont(),V,FVector2D(X,Y),FVector2D(Scale),Color,0,FLinearColor::Transparent,FVector2D::ZeroVector);};Rect(0,0,1024,640,FLinearColor(.008,.015,.025));Text(Spec().Name,26,22);Text(FString::Printf(TEXT("%03.0f KT"),Speed*.0194384),30,85,2.5);Text(FString::Printf(TEXT("%05.0f FT"),GetActorLocation().Z*.0328084),540,85,2.5);Rect(35,178,945,2,FLinearColor(.1,.7,.75));
 Text(FString::Printf(TEXT("FUEL %03.0f%%    GEAR %s"),100*FuelLitres()/FuelCapacity(),Record()->Gear84?TEXT("DOWN"):TEXT("UP")),35,210);Text(FString::Printf(TEXT("THRUST %03.0f%%    HEADING %03.0f"),Record()->Thrust84*100,FMath::Fmod(GetActorRotation().Yaw+360,360)),35,260);
 static const TCHAR* Modes[]={TEXT("PARKED"),TEXT("TAXI"),TEXT("TAKEOFF"),TEXT("CLIMB"),TEXT("CRUISE"),TEXT("APPROACH"),TEXT("FINAL"),TEXT("ROLLOUT"),TEXT("ROAMING")};Text(Record()->Autopilot84?FString(TEXT("AUTO / "))+Modes[FMath::Min(int(Record()->FlightPhase84),8)]:TEXT("MANUAL FLIGHT"),35,320,1.6);
 if(auto* Field=LWAviation84::Runway(Record()->Destination84))Text(Field->Name,35,380);else Text(TEXT("NO DESTINATION / FREE FLIGHT"),35,380);
 Text(FString::Printf(TEXT("LOCAL %02d:%02d    V/S %+04.0f FPM"),int(World->TimeOfDay),int(FMath::Frac(World->TimeOfDay)*60),Record()->FlightVelocity84.Z*1.9685),35,445);
 if(FuelLitres()<FuelCapacity()*.08f||Record()->Health<199.9f)Text(Record()->Health<199.9f?TEXT("MASTER WARNING / AIRFRAME DAMAGE"):TEXT("FUEL LOW / LAND SOON"),35,530,1.4,FLinearColor(1,.15,.025));else Text(TEXT("G GEAR   R ENGINE   SPACE WALK"),35,530,1.2);
 }UKismetRenderingLibrary::EndDrawCanvasToRenderTarget(this,Context);}
