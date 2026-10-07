#include "LWVehicle.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "LWWorld.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/TextRenderComponent.h"
#include "Components/AudioComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"

namespace {
const FVector Seats66[]={FVector(60,65,61),FVector(-165,-85,61),FVector(-345,-85,61),FVector(-212,86,61),FVector(-300,86,61)};
}
float ALWVehicle::SeatYaw66(int Seat)const{return !IsCamper74()?0:Seat==1?180:Seat>=3?-90:0;}
FVector ALWVehicle::BunkPosition66(int Bunk)const{
 const int N=FMath::Clamp(Bunk,0,5);return Root->GetComponentTransform().TransformPosition(FVector(-708,N<3?-87:87,79+(N%3)*73));
}
void ALWVehicle::BuildLuxury66(){
 if(!IsCamper74())return;
 Passengers.SetNum(5);
 // Replace the old shell and interior, retaining working driving hardware only.
 for(auto& C:Details){if(!IsValid(C))continue;const FName T=C->ComponentTags.IsEmpty()?NAME_None:C->ComponentTags[0];
  const FString N=C->GetStaticMesh()?C->GetStaticMesh()->GetName():TEXT("");
  if(Wheels.Contains(C)||Wipers.Contains(C)||C==SteeringWheel||C==GloveLid||C==Windshield||C==CamperDoor||GaugeNeedles42.Contains(C)||GaugeLamps42.Contains(C)||N.StartsWith(TEXT("SM_Dial42")))continue;
  if(T==TEXT("ignition")||T==TEXT("radio")||T==TEXT("glove")||T==TEXT("lights")||T==TEXT("wipers")||T==TEXT("left")||T==TEXT("right"))continue;
  C->SetVisibility(false);C->SetCollisionEnabled(ECollisionEnabled::NoCollision);C->ComponentTags.Empty();
 }
 // Spotlights inherit from point lights. Only retire the explicitly marked old
 // cabin fixtures; the headlamps must survive this rebuild and garbage collection.
 TArray<UPointLightComponent*> OldLights;GetComponents(OldLights);for(auto* L:OldLights)if(L->ComponentHasTag(TEXT("LegacyCamperCabinLight")))L->DestroyComponent();
 Body->EmptyOverrideMaterials();Body->SetStaticMesh(World->Mesh(TEXT("RV66_Shell")));Body->SetCollisionEnabled(ECollisionEnabled::QueryOnly);Body->SetCollisionResponseToAllChannels(ECR_Ignore);Body->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
 PaintWet.Empty();for(int M=0;M<Body->GetNumMaterials();M++)if(Body->GetMaterial(M)&&Body->GetMaterial(M)->GetName().Contains(TEXT("RV66_Pearl")))PaintWet.Add(Body->CreateDynamicMaterialInstance(M));
 if(CamperDoor)for(int M=0;M<CamperDoor->GetNumMaterials();M++)if(auto* Mat=CamperDoor->GetMaterial(M)){const FString N=Mat->GetName();if(N.Contains(TEXT("Wood")))CamperDoor->SetMaterial(M,World->Material(TEXT("RV66_Ivory")));else if(N.Contains(TEXT("Metal")))CamperDoor->SetMaterial(M,World->Material(TEXT("RV66_Pearl")));}
 auto Add=[&](FName Mesh,FVector V,FName Action=NAME_None,FRotator Rot=FRotator::ZeroRotator,FVector Scale=FVector(1),int Slide=0){
  Part(Mesh,V,Scale,Rot);auto* C=Details.Last().Get();C->ComponentTags.Empty();if(!Action.IsNone())C->ComponentTags.Add(Action);
  C->SetCollisionEnabled(ECollisionEnabled::QueryOnly);C->SetCollisionResponseToAllChannels(ECR_Ignore);C->SetCollisionResponseToChannel(ECC_Visibility,ECR_Block);
  if(Slide){Slides66.Add(C);SlideRest66.Add(V);SlideSide66.Add(Slide);}return C;};
 Add(TEXT("RV66_Interior"),FVector::ZeroVector);
 Add(TEXT("RV66_Chair"),FVector(60,-55,61),TEXT("seat_-1"));
 for(int I=0;I<3;I++)Add(I?TEXT("RV66_Dinette"):TEXT("RV66_Chair"),Seats66[I],FName(*FString::Printf(TEXT("seat_%d"),I)),FRotator(0,SeatYaw66(I),0),FVector(1),I?-1:0);
 Add(TEXT("RV66_DiningSlide"),FVector(-255,-86,61),NAME_None,FRotator::ZeroRotator,FVector(1),-1);
 Add(TEXT("RV66_LoungeSlide"),FVector(-255,86,61),NAME_None,FRotator::ZeroRotator,FVector(1),1);
 Add(TEXT("RV66_Table"),FVector(-255,-85,61),TEXT("dining_storage"),FRotator::ZeroRotator,FVector(1),-1);
 Add(TEXT("RV66_Sofa"),FVector(-256,86,61),NAME_None,FRotator::ZeroRotator,FVector(1),1);
 for(int I=3;I<5;I++){auto* C=Add(TEXT("Cube"),Seats66[I]+FVector(0,-5,42),FName(*FString::Printf(TEXT("seat_%d"),I)),FRotator::ZeroRotator,FVector(.67,.47,.13),1);C->SetHiddenInGame(true);}
 Add(TEXT("RV66_Cooker"),FVector(-454,-91,64),TEXT("cooker"),FRotator(0,180,0));
 Add(TEXT("RV66_Sink"),FVector(-540,-91,64),TEXT("faucet66"),FRotator(0,180,0));
 // The faucet and cupboard are independent targets; retain the old cupboard's inventory.
 {auto* Door=Add(TEXT("Cube"),FVector(-540,-59,108),TEXT("sink_storage"),FRotator::ZeroRotator,FVector(.76,.02,.72));Door->SetHiddenInGame(true);}
 Add(TEXT("RV66_Fridge"),FVector(-474,91,64),TEXT("fridge"));
 Add(TEXT("RV66_Wardrobe"),FVector(-554,91,64),TEXT("wardrobe"));
 for(int I=0;I<2;I++)Add(TEXT("RV66_Overhead"),FVector(-454-I*86,-101,263),I?TEXT("pantry"):TEXT("kitchen_storage"),FRotator(0,180,0));
 // Six independently assignable sleeping berths, each with a lamp, ladder and privacy curtain.
 for(int Side:{-1,1}){
  Add(TEXT("RV66_Ladder"),FVector(-622,Side*48,64),NAME_None,FRotator(0,90,0));
  for(int Level=0;Level<3;Level++){
   const int I=(Side<0?0:3)+Level;const float Z=64+Level*73;
   Add(TEXT("RV66_Bunk"),FVector(-708,Side*87,Z),FName(*FString::Printf(TEXT("bunk66_%d"),I)),FRotator(0,Side<0?180:0,0));
  }
 }
 Add(TEXT("RV66_MasterBed"),FVector(-902,0,64),TEXT("bed"));
 // Bed-side drawers and the original cargo IDs preserve old stored possessions.
 Add(TEXT("RV66_Overhead"),FVector(-881,-99,265),TEXT("bed_storage"),FRotator(0,180,0));
 for(int Side:{-1,1})for(int I=0;I<7;I++){
  auto* C=Add(TEXT("Cube"),FVector(-870+I*125,Side*130,93),FName(*FString::Printf(TEXT("cargo_%d_%d"),Side,I)),FRotator::ZeroRotator,FVector(1.18,.035,.30));C->SetMaterial(0,World->Material(TEXT("RV66_Pearl")));
  Add(TEXT("RV66_Switch"),FVector(-870+I*125,Side*132,97),FName(*FString::Printf(TEXT("cargo_%d_%d"),Side,I)),FRotator(0,-Side*90,0),FVector(.5));
 }
 Add(TEXT("RV66_Switch"),FVector(-28,117,172),TEXT("cabinlight66"),FRotator(0,90,0));
 Add(TEXT("RV66_Switch"),FVector(-34,117,148),TEXT("slides66"),FRotator(0,90,0));
 Add(TEXT("RV66_Switch"),FVector(-40,117,125),TEXT("awning66"),FRotator(0,90,0));
 Add(TEXT("RV66_Switch"),FVector(-807,44,166),TEXT("bedlight66"),FRotator(0,180,0));
 for(int I=0;I<5;I++){
  auto* L=NewObject<UPointLightComponent>(this);L->SetupAttachment(Root);L->SetRelativeLocation(FVector(-860+I*205,0,316));L->SetIntensity(6000);L->SetAttenuationRadius(420);L->SetLightColor(FLinearColor(1.f,.86f,.67f));L->SetCastShadows(true);L->RegisterComponent();CabinLamps66.Add(L);
  for(float Y:{-48.f,48.f}){auto* C=Add(TEXT("RV66_Downlight"),FVector(-860+I*205,Y,329));C->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
 }
 for(float X:{-785.f,-595.f,-395.f,-100.f})Add(TEXT("RV66_CeilingVent"),FVector(X,0,328));
 for(float X:{-530.f,-475.f,-852.f})Add(TEXT("RV66_Outlet"),FVector(X,-116,180),NAME_None,FRotator(0,180,0));
 // Flat televisions face the lounge and master bed. Live broadcast text is drawn in-world.
 for(int I=0;I<2;I++){
  const FVector V=I?FVector(-811,0,283):FVector(-406,-79,267);const FRotator Rot(0,180,0);
  auto* TV=Add(TEXT("RV66_TV"),V,TEXT("tv66"),Rot);for(int M=0;M<TV->GetNumMaterials();M++)if(TV->GetMaterial(M)&&TV->GetMaterial(M)->GetName().Contains(TEXT("RV66_Screen")))TVMaterials66.Add(TV->CreateDynamicMaterialInstance(M));
  auto* Text=NewObject<UTextRenderComponent>(this);Text->SetupAttachment(Root);Text->SetRelativeLocation(V+FVector(2.8,0,10));Text->SetRelativeRotation(FRotator::ZeroRotator);Text->SetHorizontalAlignment(EHTA_Center);Text->SetWorldSize(4.1);Text->SetTextRenderColor(FColor(168,230,209));Text->SetCastShadow(false);Text->RegisterComponent();Screens66.Add(Text);
 }
 TVTarget66=NewObject<UTextureRenderTarget2D>(this);TVTarget66->RenderTargetFormat=RTF_RGBA8;TVTarget66->ClearColor=FLinearColor::Black;TVTarget66->InitAutoFormat(512,288);TVTarget66->UpdateResourceImmediate();
 TVCamera66=NewObject<USceneCaptureComponent2D>(this);TVCamera66->SetupAttachment(Root);TVCamera66->SetRelativeLocation(FVector(100,155,320));TVCamera66->SetRelativeRotation(FRotator(-18,105,0));TVCamera66->FOVAngle=95;TVCamera66->bCaptureEveryFrame=false;TVCamera66->bCaptureOnMovement=false;TVCamera66->CaptureSource=ESceneCaptureSource::SCS_FinalColorLDR;TVCamera66->TextureTarget=TVTarget66;TVCamera66->HiddenActors.Add(this);TVCamera66->MaxViewDistanceOverride=12000;TVCamera66->RegisterComponent();
 for(auto& Mat:TVMaterials66)Mat->SetTextureParameterValue(TEXT("SceneView"),TVTarget66);
 Water66=Add(TEXT("RV66_Water"),FVector(-540,-89,174));Water66->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 AwningMesh66=Add(TEXT("RV66_Awning"),FVector(-430,130,317));AwningMesh66->SetCollisionEnabled(ECollisionEnabled::NoCollision);
 auto Window=[&](FVector At,FVector Size,FRotator Rot,int Slide=0,bool AddGlass=true){
  if(AddGlass&&Rot.Yaw==0){auto* Glass=Add(TEXT("Cube"),At,NAME_None,Rot,Size/100,Slide);Glass->ComponentTags.Add(TEXT("CoachGlazing80"));Glass->SetMaterial(0,World->Material(TEXT("V42_Glass")));Glass->SetCollisionEnabled(ECollisionEnabled::NoCollision);}
  const int I=ShadesMesh66.Num();const FVector Top=At+Rot.RotateVector(FVector(0,0,Size.Z*.5))+(Rot.Yaw==0?FVector(0,At.Y>0?-3:3,0):FVector::ZeroVector);
  auto* Shade=Add(TEXT("RV66_Shade"),Top,FName(*FString::Printf(TEXT("shade66_%d"),I)),Rot,FVector(Size.X/100,1,Size.Z/100),Slide);ShadesMesh66.Add(Shade);ShadeRest66.Add(FVector(Size.X/100,Size.Z,Top.Z));
  // Top roller remains targetable when the shade is retracted.
  auto* Rail=Add(TEXT("Cube"),Top,FName(*FString::Printf(TEXT("shade66_%d"),I)),Rot,FVector(Size.X/100,.06,.035),Slide);Rail->SetMaterial(0,World->Material(TEXT("RV66_Brass")));
 };
 for(int Side:{-1,1}){
  for(auto W:{FVector2D(-892,186),FVector2D(-706.5,189),FVector2D(-506.5,217),FVector2D(-68,96)})Window(FVector(W.X,Side*124,250),FVector(W.Y,1,104),FRotator::ZeroRotator);
  Window(FVector(-255,Side*121,233),FVector(253,1,124),FRotator::ZeroRotator,Side);
 }
 Window(FVector(151,-123,250),FVector(122,1,104),FRotator::ZeroRotator);
 Window(FVector(152,123,250),FVector(120,1,104),FRotator::ZeroRotator);
 // Windshield blind is authored on the tilted glazing plane, and the door blind travels with its leaf.
 Window(FVector(184,0,217),FVector(218,1,144),FRotator(0,90,-9.59));
 const int DoorShade=ShadesMesh66.Num();Window(FVector(37,123,223),FVector(85,1,90),FRotator::ZeroRotator,0,false);
 if(CamperDoor){ShadesMesh66[DoorShade]->AttachToComponent(CamperDoor,FAttachmentTransformRules::KeepWorldTransform);Details.Last()->AttachToComponent(CamperDoor,FAttachmentTransformRules::KeepWorldTransform);}
 // Append the previously missing driver-side pane; preserve all saved blind IDs.
 Window(FVector(36,-123,250),FVector(120,1,104),FRotator::ZeroRotator);
 for(int I=0;I<3;I++){auto* Art=Add(TEXT("RV66_Art"),FVector(-408-I*195,117,214),NAME_None);for(int M=0;M<Art->GetNumMaterials();M++)if(Art->GetMaterial(M)&&Art->GetMaterial(M)->GetName().Contains(TEXT("RV66_Linen")))Art->SetMaterial(M,World->Material(FName(*FString::Printf(TEXT("Art%02d_65"),I+2))));}
 Add(TEXT("RV66_Plant"),FVector(-510,90,257));
 SlideAlpha66=Record()->Slides66?1:0;AwningAlpha66=Record()->Awning66?1:0;TickLuxury66(0);
}

bool ALWVehicle::ClearExtension66()const{
 FCollisionQueryParams Q(NAME_None,true,this);if(Driver)Q.AddIgnoredActor(Driver);
 for(int Side:{-1,1}){const FVector At=Root->GetComponentTransform().TransformPosition(FVector(-255,Side*169,191));
  if(GetWorld()->OverlapBlockingTestByChannel(At,GetActorQuat(),ECC_Visibility,FCollisionShape::MakeBox(FVector(140,43,125)),Q))return false;
 }return true;
}
bool ALWVehicle::ReadyToDrive66(){
 if(!IsCamper74())return true;auto* R=Record();if(!R)return false;
 if(R->Slides66||R->Awning66||SlideAlpha66>.001f||AwningAlpha66>.001f){R->Slides66=R->Awning66=false;if(Driver&&!PendingIgnition66)Driver->Notify(TEXT("RETRACTING SLIDE-OUTS AND AWNING"));PendingIgnition66=true;EngineOn=false;Throttle=0;return false;}return true;
}
void ALWVehicle::TickLuxury66(float Dt){
 if(!IsCamper74()||!Record())return;auto* R=Record();
 if(R->Exploded){R->TV66=0;R->Faucet66=false;R->CabinLights66=R->BedroomLights66=false;ScreenClock66=0;PendingIgnition66=false;}
 if(EngineOn&&(R->Slides66||R->Awning66||SlideAlpha66>.001f||AwningAlpha66>.001f))ReadyToDrive66();
 const float PreviousSlide=SlideAlpha66;SlideAlpha66=FMath::FInterpConstantTo(SlideAlpha66,R->Slides66?1.f:0.f,Dt,.16f);AwningAlpha66=FMath::FInterpConstantTo(AwningAlpha66,R->Awning66?1.f:0.f,Dt,.23f);
 if(Dt==0||PreviousSlide!=SlideAlpha66)for(int I=0;I<Slides66.Num();I++)if(IsValid(Slides66[I]))Slides66[I]->SetRelativeLocation(SlideRest66[I]+FVector(0,SlideSide66[I]*80*SlideAlpha66,0));
 for(auto& Seal:SlideSeals80)if(IsValid(Seal)){Seal->SetRelativeScale3D(FVector(1,FMath::Max(.001f,SlideAlpha66),1));Seal->SetVisibility(SlideAlpha66>.001f);}
 if(AwningMesh66){AwningMesh66->SetRelativeScale3D(FVector(1,FMath::Max(.018f,AwningAlpha66*2.6f),1));AwningMesh66->SetVisibility(AwningAlpha66>.001f);}
 for(int I=0;I<ShadesMesh66.Num();I++){auto* Shade=ShadesMesh66[I].Get();const float Target=R->Shades66.Contains(I)?ShadeRest66[I].Y/100:.02f;FVector Scale=Shade->GetRelativeScale3D();Scale.Z=Dt>0?FMath::FInterpConstantTo(Scale.Z,Target,Dt,1.2f):Target;Shade->SetRelativeScale3D(Scale);}
 for(int I=0;I<CabinLamps66.Num();I++)CabinLamps66[I]->SetVisibility(I<2?R->BedroomLights66:R->CabinLights66);
 if(Water66)Water66->SetVisibility(R->Faucet66&&!R->Exploded);
 ScreenClock66-=Dt;if(ScreenClock66<=0){ScreenClock66=.5f;const int H=int(World->TimeOfDay)%24,M=int(World->TimeOfDay*60)%60;
  const FString Text=R->TV66==0?TEXT(""):R->TV66==1?FString::Printf(TEXT("CIVIL BROADCAST\n%02d:%02d\nSEEK SHELTER\nKEEP YOUR RADIO ON"),H,M):R->TV66==2?FString::Printf(TEXT("COACH STATUS\n%s %.0f / %.0f %s\n%02d:%02d\n%s"),IsElectric74()?TEXT("BATTERY"):TEXT("FUEL"),IsElectric74()?BatteryCharge74():FuelLitres(),IsElectric74()?BatteryCapacity74():FuelCapacity(),IsElectric74()?TEXT("kWh"):TEXT("L"),H,M,R->Slides66?TEXT("CAMP MODE"):TEXT("TRAVEL MODE")):FString::Printf(TEXT("AMERICAN ROAD\n%s\nA BETTER TOMORROW\nBEGINS WITH YOU"),(int(GetWorld()->GetTimeSeconds()/3)%2)?TEXT("     > > >"):TEXT("> > >"));
  for(auto& Screen:Screens66){Screen->SetText(FText::FromString(Text));Screen->SetVisibility(R->TV66>0&&R->TV66<3);}
  for(auto& Mat:TVMaterials66){Mat->SetScalarParameterValue(TEXT("Power"),R->TV66>0?1.f:0.f);Mat->SetScalarParameterValue(TEXT("Live"),R->TV66==3?1.f:0.f);}
  if(R->TV66==3&&TVCamera66)if(auto* P=UGameplayStatics::GetPlayerPawn(this,0))if(FVector::DistSquared(P->GetActorLocation(),GetActorLocation())<FMath::Square(1800.f))TVCamera66->CaptureScene();
 }
 if(R->TV66>0&&R->TV66<3&&!TVAudio66){TVAudio66=World->Sound(TEXT("CarRadio"),GetActorLocation(),.14f);if(TVAudio66)TVAudio66->AttachToComponent(Root,FAttachmentTransformRules::KeepWorldTransform);}
 if((R->TV66==0||R->TV66==3)&&TVAudio66){TVAudio66->Stop();TVAudio66->DestroyComponent();TVAudio66=nullptr;}
 if(PendingIgnition66&&SlideAlpha66<=.001f&&AwningAlpha66<=.001f){PendingIgnition66=false;if((Driver||Chauffeur)&&HasFuel()&&R->Health>0&&!R->Exploded&&(R->Hotwired||(Driver&&Driver->HasKey(R->VIN)))){EngineOn=true;if(Driver)Driver->Notify(TEXT("READY TO DRIVE"));}}
 for(int I=0;I<Passengers.Num();I++)if(IsValid(Passengers[I])){Passengers[I]->SetActorRelativeLocation(SeatLocation(I));Passengers[I]->SetActorRelativeRotation(FRotator(0,SeatYaw66(I),0));}
}

FString ALWVehicle::LuxuryLabel66(FName Action)const{
 const FString S=Action.ToString();const auto* R=Record();if(!R)return S.ToUpper();
 if(Action==TEXT("slides66"))return R->Slides66?TEXT("RETRACT SLIDE-OUTS"):TEXT("EXTEND SLIDE-OUTS");
 if(Action==TEXT("awning66"))return R->Awning66?TEXT("RETRACT AWNING"):TEXT("EXTEND AWNING");
 if(Action==TEXT("cabinlight66"))return TEXT("LOUNGE LIGHTS");if(Action==TEXT("bedlight66"))return TEXT("BEDROOM LIGHTS");
 if(Action==TEXT("tv66"))return R->TV66?TEXT("TV / CHANGE CHANNEL / OFF"):TEXT("TURN ON TV");
 if(Action==TEXT("faucet66"))return R->Faucet66?TEXT("TURN OFF FAUCET"):TEXT("TURN ON FAUCET / DRINK");
 if(S.StartsWith(TEXT("shade66_")))return R->Shades66.Contains(FCString::Atoi(*S.Mid(8)))?TEXT("OPEN SHADE"):TEXT("CLOSE SHADE");
 if(S.StartsWith(TEXT("bunk66_")))return FString::Printf(TEXT("BUNK %d / ASSIGN COMPANION"),FCString::Atoi(*S.Mid(7))+1);
 return S.ToUpper();
}
bool ALWVehicle::UseLuxury66(ALWCharacter* P,FName A){
 if(!P||!IsCamper74()||!Record())return false;const FString S=A.ToString();
 if(!S.Contains(TEXT("66")))return false;auto* R=Record();
 if((FMath::Abs(Speed)>5||AutoDriving||Boarding||Held49||Airborne49)&&!(IsSolarRV74()&&SelfDriving74&&P==Driver&&(A==TEXT("cabinlight66")||A==TEXT("bedlight66")||A==TEXT("tv66")||A==TEXT("faucet66")||S.StartsWith(TEXT("shade66_"))))){P->Notify(TEXT("PARK FIRST"));return true;}
 if(A==TEXT("slides66")||A==TEXT("awning66")){
  if(EngineOn||PendingIgnition66){P->Notify(TEXT("SWITCH OFF THE ENGINE FIRST"));return true;}
  if(A==TEXT("slides66")){if(!R->Slides66&&!ClearExtension66()){P->Notify(TEXT("NOT ENOUGH CLEARANCE BESIDE THE RV"));return true;}R->Slides66=!R->Slides66;}
  else {if(!R->Awning66){FCollisionQueryParams Q(NAME_None,true,this);Q.AddIgnoredActor(P);if(GetWorld()->OverlapBlockingTestByChannel(Root->GetComponentTransform().TransformPosition(FVector(-430,266,307)),GetActorQuat(),ECC_Visibility,FCollisionShape::MakeBox(FVector(350,134,18)),Q)){P->Notify(TEXT("AWNING IS OBSTRUCTED"));return true;}}R->Awning66=!R->Awning66;}
 }else if(A==TEXT("cabinlight66"))R->CabinLights66=!R->CabinLights66;
 else if(A==TEXT("bedlight66"))R->BedroomLights66=!R->BedroomLights66;
 else if(A==TEXT("tv66")){R->TV66=(R->TV66+1)%4;ScreenClock66=0;}
 else if(A==TEXT("faucet66")){R->Faucet66=!R->Faucet66;if(R->Faucet66){P->Thirst=FMath::Min(100.f,P->Thirst+20);P->Notify(TEXT("FILTERED WATER"));}}
 else if(S.StartsWith(TEXT("shade66_"))){const int I=FCString::Atoi(*S.Mid(8));if(ShadesMesh66.IsValidIndex(I)){if(R->Shades66.Contains(I))R->Shades66.Remove(I);else R->Shades66.Add(I);}}
 else if(S.StartsWith(TEXT("bunk66_"))){P->OpenRVHome66(this,FCString::Atoi(*S.Mid(7)));return true;}
 VehicleSound42(TEXT("CarSwitch"),.5f);P->RequestSave40();return true;
}
