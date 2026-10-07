#include "LWNewYork69.h"
#include "LWCampaignProduction77.h"
#include "LWCampaign76.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWNPCLife.h"
#include "LWCanada68.h"
#include "LWVehicle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/CapsuleComponent.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Kismet/KismetMathLibrary.h"

FVector LWProduction77::ArmElbow(FVector S,FVector T,FVector Pole,float U,float L){
 FVector Axis=(T-S).GetSafeNormal();if(Axis.IsNearlyZero())Axis=FVector(0,0,-1);
 const double D=FMath::Clamp((T-S).Size(),double(FMath::Abs(U-L)+.01),double(U+L-.01));
 FVector Side=(Pole-Axis*FVector::DotProduct(Pole,Axis)).GetSafeNormal();if(Side.IsNearlyZero())Side=FVector::CrossProduct(Axis,FVector::RightVector).GetSafeNormal();
 const double A=(U*U-L*L+D*D)/(2*D),H=FMath::Sqrt(FMath::Max(0.,U*U-A*A));return S+Axis*A+Side*H;
}
float LWProduction77::ContactEnvelope(float T,float D){return FMath::SmoothStep(0.f,.6f,T)*(1-FMath::SmoothStep(FMath::Max(.6f,D-.6f),D,T));}
float LWProduction77::FerryDuration(){return 160.f;}
FTransform LWProduction77::FerryPose(float S){
 const float T=FMath::Clamp(S/FerryDuration(),0.f,1.f),U=T*T*(3-2*T);
 const FVector A(LWNY69::Project(44.255,-76.088),70),B(LWNY69::Project(44.267,-76.100),70),C(LWNY69::Project(44.296,-76.125),70),D(LWNY69::Project(44.320,-76.140),70);
 FVector P=FMath::CubicInterp(A,(B-A)*3.,D,(D-C)*3.,U);
 FVector Tangent=6*U*(U-1)*A+(3*U*U-4*U+1)*(B-A)*3.+6*U*(1-U)*D+(3*U*U-2*U)*(D-C)*3.;
 const float Sea=FMath::Sin(T*PI);P.Z+=FMath::Sin(S*1.25f)*4*Sea;
 return FTransform(FRotator(FMath::Sin(S*.7f)*.45f*Sea,Tangent.Rotation().Yaw-90,FMath::Sin(S*.95f)*.6f*Sea),P);
}
ULWCampaignProduction77::ULWCampaignProduction77(){PrimaryComponentTick.bCanEverTick=false;}
UStaticMeshComponent* ULWCampaignProduction77::Add(FName Key,FVector V,FRotator R,FVector Scale,bool Solid,AActor* Parent){
 if(!Director||!Director->World)return nullptr;auto* Asset=Director->World->Mesh(Key);if(!Asset)return nullptr;
 AActor* Owner=Parent?Parent:Director.Get();auto* M=NewObject<UStaticMeshComponent>(Owner);M->SetupAttachment(Owner->GetRootComponent());M->SetMobility(EComponentMobility::Movable);M->SetStaticMesh(Asset);M->SetCollisionProfileName(Solid?TEXT("BlockAll"):TEXT("NoCollision"));M->SetCanEverAffectNavigation(false);M->SetRelativeScale3D(Scale);M->RegisterComponent();
 if(Parent){M->SetRelativeLocation(V);M->SetRelativeRotation(R);}else {M->SetWorldLocation(Director->At(V));M->SetWorldRotation(LWCampaign76::Frame(LWCampaign76::Stage(Director->State().Stage)->Site).Rotator()+R);}
 Meshes.Add(M);return M;
}
void ULWCampaignProduction77::Clear(){
 const FName SavedPerformance=Director&&Director->Player?Director->State().Performance77:NAME_None;
 if(Boat&&Director&&Director->Player){auto* P=Director->Player.Get();P->SetBase(static_cast<UPrimitiveComponent*>(nullptr));P->GetCharacterMovement()->bImpartBaseVelocityX=P->GetCharacterMovement()->bImpartBaseVelocityY=P->GetCharacterMovement()->bImpartBaseVelocityZ=true;}
 StopPerformance();for(auto& M:Meshes)if(IsValid(M))M->DestroyComponent();Meshes.Empty();for(auto& L:Lights)if(L)L->DestroyComponent();Lights.Empty();
 if(Boat&&Director&&Director->Player&&Director->Player->GetMovementBase()==BoatDeck)Director->Player->SetBase(static_cast<UPrimitiveComponent*>(nullptr));
 if(Boat)Boat->Destroy();Boat=nullptr;BoatDeck=nullptr;Gangway=nullptr;Case=nullptr;Tool=nullptr;Cuffs=nullptr;Mechanisms.Empty();WreckPanels.Empty();WreckRest.Empty();Crowd.Empty();
 if(Director&&Director->Player)Director->State().Performance77=SavedPerformance;
}
void ULWCampaignProduction77::Build(){
 const auto* S=LWCampaign76::Stage(Director->State().Stage);if(!S)return;
 if(S->Site==TEXT("rome"))BuildCanal();
 if(S->Site==TEXT("canada")&&S->Id!=TEXT("canada_settle")){
  for(auto Pair:{TPair<FName,FVector>(TEXT("Bed65"),FVector(760,600,0)),TPair<FName,FVector>(TEXT("Sink65"),FVector(-950,240,0))})if(auto* M=Director->World->Mesh(Pair.Key)){Pair.Value.Z-=M->GetBoundingBox().Min.Z;Add(Pair.Key,Pair.Value);}
 }
 Case=Add(TEXT("Case77"),FVector(0,300,82),FRotator::ZeroRotator,FVector(1),false);if(Case)Case->SetVisibility(false);
 Tool=Add(TEXT("RepairTool77"),FVector::ZeroVector,FRotator::ZeroRotator,FVector(1),false);if(Tool)Tool->SetVisibility(false);
 Cuffs=Add(TEXT("Restraint77"),FVector::ZeroVector,FRotator::ZeroRotator,FVector(1),false);if(Cuffs)Cuffs->SetVisibility(false);
 if(S->Id==TEXT("resistance_exit")||S->Id==TEXT("final_evacuation")||S->Id==TEXT("meridian_inside"))StartCrowd(S->Id);
 if(!Director->State().Performance77.IsNone()){
  Performance=Director->State().Performance77;Performer=Director->People.FindRef(Director->State().Performer77);Recipient=Director->People.FindRef(Director->State().Recipient77);PerformanceTime=Director->State().PerformanceTime77;PerformanceDuration=Director->State().PerformanceDuration77;
 }
}
void ULWCampaignProduction77::PlaceCast(){
 if(!Director)return;const FName Stage=Director->State().Stage;
 if(Stage==TEXT("guard_convoy"))if(auto* N=Director->People.FindRef(TEXT("tomas")).Get()){
  // Front passenger cushion in the actual transport van, facing its windscreen.
  N->SetActorLocation(Director->At(FVector(907,343,56)),false,nullptr,ETeleportType::TeleportPhysics);N->SetActorRotation(LWCampaign76::Frame(TEXT("guard")).Rotator());N->MoveTime=0;N->LifeAnimation->PerformancePose77=1;N->GetCharacterMovement()->DisableMovement();N->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  if(auto* Guard=Director->People.FindRef(TEXT("imani")).Get()){Guard->SetActorLocation(Director->At(FVector(907,257,56)),false,nullptr,ETeleportType::TeleportPhysics);Guard->SetActorRotation(N->GetActorRotation());Guard->LifeAnimation->PerformancePose77=3;Guard->GetCharacterMovement()->DisableMovement();Guard->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);Guard->MoveTime=0;}
 }
 if(Stage==TEXT("bell_attack"))if(auto* N=Director->People.FindRef(TEXT("lena")).Get()){
  N->SetActorLocation(Director->At(FVector(620,250,100)),false,nullptr,ETeleportType::TeleportPhysics);N->SetActorRotation(LWCampaign76::Frame(TEXT("bellwether")).Rotator()+FRotator(0,90,0));N->LifeAnimation->PerformancePose77=4;N->MoveTime=0;N->GetCharacterMovement()->DisableMovement();N->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  // The assault begins with Mara tending the casualty, while the defenders
  // cover the entrance and records. A generic line of cast members across the
  // room made the medic continually avoid the stationary guards.
  for(const auto& Mark:{TPair<FName,FVector>(TEXT("mara"),FVector(520,202,92)),TPair<FName,FVector>(TEXT("ivo"),FVector(-230,-360,92)),TPair<FName,FVector>(TEXT("tessa"),FVector(-210,10,92))})if(auto* Person=Director->People.FindRef(Mark.Key).Get()){
   Person->SetActorLocation(Director->At(Mark.Value),false,nullptr,ETeleportType::TeleportPhysics);Person->MoveTime=0;Person->ResetCompanionNavigation();
  }
 }
 if((Stage==TEXT("carrier_hall")||Stage==TEXT("carrier_battle"))&&Director->Carrier)if(auto* N=Director->People.FindRef(TEXT("richardson")).Get()){
  N->SetActorLocation(Director->Carrier->GetActorTransform().TransformPosition(FVector(0,210,246)),false,nullptr,ETeleportType::TeleportPhysics);N->SetActorRotation(LWCampaign76::Frame(TEXT("transport")).Rotator()+FRotator(0,-90,0));N->LifeAnimation->PerformancePose77=3;N->MoveTime=0;N->GetCharacterMovement()->DisableMovement();N->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
  if(LWCampaign76::Alive(Director->State(),TEXT("executive_aide77"))){
   auto* Guard=GetWorld()->SpawnActor<ALWCampaignPerson76>();if(Guard){Guard->Campaign=Director;Guard->Person=TEXT("executive_aide77");Guard->ConfigureResident(TEXT("executive_aide77"),TEXT("campaign76"),TEXT("Executive escort"),3,0);Guard->ChatterTime=99999;Guard->Health=Director->State().Health.Contains(Guard->Person)?Director->State().Health[Guard->Person]:150;Guard->MaximumHealth=150;Guard->LegendaryInitialized=true;Guard->Tags.AddUnique(TEXT("DirectedMotion77"));Guard->SetActorLocation(Director->Carrier->GetActorTransform().TransformPosition(FVector(-60,230,282)));Guard->SetActorRotation(N->GetActorRotation());Guard->GetCharacterMovement()->DisableMovement();Guard->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);Guard->Gun->SetVisibility(false);Director->People.Add(Guard->Person,Guard);Director->Actors.Add(Guard);}
  }
 }
 if(Stage==TEXT("canada_tracing")||Stage==TEXT("canada_investigate")){
  if(auto* N=Director->People.FindRef(TEXT("chen")).Get()){N->SetActorLocation(Director->At(FVector(0,400,92)),false,nullptr,ETeleportType::TeleportPhysics);N->SetActorRotation(FRotator(0,-90,0));}
  if(auto* N=Director->People.FindRef(TEXT("lena")).Get())N->SetActorLocation(Director->At(FVector(145,420,92)),false,nullptr,ETeleportType::TeleportPhysics);
  int I=0;for(FName Id:{FName(TEXT("tomas")),FName(TEXT("ruth")),FName(TEXT("milo"))})if(auto* N=Director->People.FindRef(Id).Get()){N->SetActorLocation(Director->At(FVector(-370+I*350,20,92)),false,nullptr,ETeleportType::TeleportPhysics);I++;}
 }
}
void ULWCampaignProduction77::Contact(ALWCampaignPerson76* N,FVector Target,int Side,float Weight){if(N&&!N->bDead&&N->LifeAnimation){N->LifeAnimation->HandTargets77[Side]=Target;N->LifeAnimation->HandWeights77[Side]=FMath::Clamp(Weight,0.f,1.f);}}
void ULWCampaignProduction77::StopPerformance(){
 for(auto N:{Performer.Get(),Recipient.Get()})if(N&&N->LifeAnimation){N->LifeAnimation->HandWeights77[0]=N->LifeAnimation->HandWeights77[1]=0;if(N->LifeAnimation->PerformancePose77==2)N->LifeAnimation->PerformancePose77=0;}
 Performer.Reset();Recipient.Reset();Performance=NAME_None;PerformanceTime=0;WorkReady=true;if(Tool){Tool->SetVisibility(false);Tool->SetWorldScale3D(FVector(1));}if(Case)Case->SetVisibility(false);
 if(Director)Director->State().Performance77=NAME_None;
}
void ULWCampaignProduction77::StartPerformance(FName Type,FName Who,FName Other,float Duration){
 if(Performance==Type&&Performer.IsValid()&&Performer->Person==Who)return;StopPerformance();Performance=Type;Performer=Director->People.FindRef(Who);Recipient=Director->People.FindRef(Other);PerformanceDuration=Duration;
 auto& S=Director->State();S.Performance77=Type;S.Performer77=Who;S.Recipient77=Other;S.PerformanceTime77=0;S.PerformanceDuration77=Duration;
}
void ULWCampaignProduction77::Work(FName Action,float Time,float Duration){
 FName Who=TEXT("della"),Type=TEXT("repair"),Other;
 const FString Id=Action.ToString();
 if(Id.Contains(TEXT("treat"))||Id.Contains(TEXT("medical"))||Id.Contains(TEXT("oxygen"))||Id.Contains(TEXT("wash"))){Who=TEXT("mara");Type=TEXT("treatment");Other=Action==TEXT("treat_lena")?FName(TEXT("lena")):NAME_None;}
 else if(Id.Contains(TEXT("seal"))||Id.Contains(TEXT("case"))||Id.Contains(TEXT("parcel"))||Id.Contains(TEXT("prescription"))){Who=TEXT("chen");Type=TEXT("handoff");}
 else if(Action==TEXT("library_shelf"))Who=TEXT("jules");
 if(Action==TEXT("library_shelf")||Id.Contains(TEXT("brace"))||Id.Contains(TEXT("window")))Type=TEXT("brace");
 else if(Id.Contains(TEXT("gate"))||Id.Contains(TEXT("drain")))Type=TEXT("crank");
 else if(Id.Contains(TEXT("roster"))||Id.Contains(TEXT("manifest"))||Id.Contains(TEXT("count")))Type=TEXT("write");
 else if(Id.Contains(TEXT("wash")))Type=TEXT("wash");
 auto Available=[&](FName Id2){const auto* Actor=Director->People.FindRef(Id2).Get();return Actor&&!Actor->bDead;};
 if(!Available(Who)){Who=NAME_None;const auto* S=LWCampaign76::Stage(Director->State().Stage);for(FName Id2:S->Cast)if(Available(Id2)){Who=Id2;break;}}
 // Personal hygiene is performed by the player at the sink. Recruiting a
 // helper here sends that actor behind the basin and can block the rest quest.
 if(Type==TEXT("wash"))Who=NAME_None;
 WorkReady=true;
 StartPerformance(Type,Who,Other,Duration);PerformanceTime=Time;
 const auto* Stage=LWCampaign76::Stage(Director->State().Stage);const auto* A=Stage->Actions.FindByPredicate([&](const auto& V){return V.Id==Action;});if(!A)return;
 if(auto* N=Performer.Get()){
  WorkContact=Director->At(A->At+FVector(0,0,95));
  for(const auto& Actor:Director->Actors)if(auto* Node=Cast<ALWCampaignNode76>(Actor);Node&&Node->Action==Action){WorkContact=Node->Body->Bounds.Origin;WorkContact.Z=FMath::Clamp(WorkContact.Z,Director->At(A->At).Z+55,Director->At(A->At).Z+125);WorkContact.X-=FMath::Max(0.,Node->Body->Bounds.BoxExtent.X-4);break;}
  FVector Target(WorkContact.X-45,WorkContact.Y,Director->At(A->At).Z+92);
  if(auto* Patient=Recipient.Get()){WorkContact=Patient->Parts[4]->GetComponentLocation()+FVector(0,0,12);Target=Patient->GetActorLocation()+Patient->GetActorRightVector()*100-Patient->GetActorForwardVector()*48;Target.Z=Director->At(A->At).Z+92;}
  WorkReady=FVector::Dist2D(N->GetActorLocation(),Target)<12;
  if(!WorkReady){N->Mark=Target;N->MoveTime=Duration+2;}else {N->MoveTime=0;N->SetActorRotation(FRotator(0,(WorkContact-N->GetActorLocation()).Rotation().Yaw,0));}
 }
}
void ULWCampaignProduction77::Beat(FName Scene,int32 Index){
 const auto* S=LWCampaign76::Scene(Scene);if(!S||!S->Beats.IsValidIndex(Index))return;
 const auto& B=S->Beats[Index];
 auto Start=[&](FName Type,FName Who,FName To,float Duration){StartPerformance(Type,Who,To,Duration);Director->BeatWait=FMath::Max(Director->BeatWait,Duration);};
 if(Scene==TEXT("convoy_restraints")&&Index==0)Start(TEXT("restraint"),TEXT("imani"),TEXT("tomas"),4);
 if(Scene==TEXT("convoy_release")&&Index==0)Start(TEXT("release"),TEXT("imani"),TEXT("tomas"),4);
 if(Scene==TEXT("route_witness")&&Index==2)Start(TEXT("seal"),TEXT("chen"),TEXT("lena"),4);
 if(Scene==TEXT("weller_tape")&&Index==0)Start(TEXT("handoff"),TEXT("simon"),TEXT("hannah"),4);
 if(Scene==TEXT("border_arrival")&&Index==2)Start(TEXT("handoff"),TEXT("claire"),TEXT("mara"),4);
 if(Scene==TEXT("tracing")&&B.Who==TEXT("tomas"))Start(TEXT("phone"),TEXT("tomas"),NAME_None,4);
 if(Scene==TEXT("world_records")&&Index==0)Start(TEXT("handoff"),TEXT("chen"),TEXT("lena"),4);
 if(Scene==TEXT("broken_office")&&Index==0){Director->BeatWait=FMath::Max(Director->BeatWait,10.f);Director->State().CarrierDamage77=FMath::Max(.01f,Director->State().CarrierDamage77);}
}
void ULWCampaignProduction77::Update(float Dt){
 if(!Director||!Director->Player)return;auto& S=Director->State();
 if(!Performance.IsNone()&&Performer.IsValid()&&!Performer->bDead){
  auto* N=Performer.Get();auto* Partner=Recipient.Get();bool ContactReady=true;
  if(Partner&&Director->WorkAction.IsNone()&&(Performance==TEXT("handoff")||Performance==TEXT("release")||Performance==TEXT("restraint")||Performance==TEXT("seal")||Performance==TEXT("treatment"))){
   const float Separation=FVector::Dist2D(N->GetActorLocation(),Partner->GetActorLocation());
   if(Separation>95){const FVector Dir=(N->GetActorLocation()-Partner->GetActorLocation()).GetSafeNormal2D();N->Mark=Partner->GetActorLocation()+Dir*78;N->MoveTime=5;ContactReady=false;}
   else {N->MoveTime=0;N->SetActorRotation(FRotator(0,(Partner->GetActorLocation()-N->GetActorLocation()).Rotation().Yaw,0));if(Performance==TEXT("handoff")||Performance==TEXT("seal"))Partner->SetActorRotation(FRotator(0,(N->GetActorLocation()-Partner->GetActorLocation()).Rotation().Yaw,0));}
  }
  if(Director->WorkAction.IsNone()&&ContactReady)PerformanceTime+=Dt;
  if(Director->SceneOpen)Director->BeatWait=FMath::Max(Director->BeatWait,PerformanceDuration-PerformanceTime);
  const float W=LWProduction77::ContactEnvelope(PerformanceTime,PerformanceDuration);FVector Target=N->GetActorTransform().TransformPosition(FVector(37,8,20));
  auto* Other=Recipient.Get();
  if(Performance==TEXT("treatment")){
   N->LifeAnimation->PerformancePose77=WorkReady?2:0;if(Other){Target=Other->Parts[4]->GetComponentLocation()+FVector(0,0,12);Other->LifeAnimation->PerformancePose77=4;}
   else Target=!Director->WorkAction.IsNone()?WorkContact:N->GetActorTransform().TransformPosition(FVector(40,6,4));
   Target+=N->GetActorRightVector()*FMath::Sin(PerformanceTime*5)*3;Contact(N,Target,1,W);Contact(N,Target-N->GetActorRightVector()*14,0,W);
   if(Tool){Tool->SetStaticMesh(Director->World->Mesh(TEXT("DressingRoll77")));Tool->SetVisibility(W>.05f);Tool->SetWorldLocation(Target);Tool->SetWorldRotation(FRotator(0,PerformanceTime*100,90));}
  }else if(Performance==TEXT("restraint")||Performance==TEXT("release")){
   if(Other){Other->LifeAnimation->PerformancePose77=1;Target=Other->GetActorTransform().TransformPosition(FVector(24,-16,14));Contact(Other,Target+Other->GetActorRightVector()*5,1,1);Contact(Other,Target-Other->GetActorRightVector()*5,0,1);Contact(N,Target,1,W);}
   if(Cuffs){Cuffs->SetVisibility(Performance!=TEXT("release")||PerformanceTime<2.7f);Cuffs->SetWorldLocation(Target);}
  }else if(Performance==TEXT("handoff")||Performance==TEXT("seal")){
   if(Other){const FVector Mid=(N->Parts[4]->GetComponentLocation()+Other->Parts[3]->GetComponentLocation())*.5-FVector(0,0,22);Target=Mid;Contact(Other,Mid,0,FMath::SmoothStep(1.5f,2.4f,PerformanceTime)*W);}
   Contact(N,Target,1,W);if(Case){Case->SetVisibility(W>.02);Case->SetWorldLocation(Target);Case->SetWorldRotation(N->GetActorRotation());}
  }else if(Performance==TEXT("phone")){
   Target=N->Parts[1]->GetComponentLocation()+N->GetActorRightVector()*9-FVector(0,0,7);Contact(N,Target,1,W);
   if(Tool){Tool->SetStaticMesh(Director->World->Mesh(TEXT("RepairTool77")));Tool->SetWorldLocation(Target);Tool->SetWorldRotation(N->GetActorRotation());Tool->SetWorldScale3D(FVector(1.5,2,1));Tool->SetVisibility(W>.05);}
  }else{
   Target=!Director->WorkAction.IsNone()?WorkContact:N->GetActorTransform().TransformPosition(FVector(40,12,25));Target+=FVector(0,0,FMath::Sin(PerformanceTime*7)*2);
   if(Performance==TEXT("crank"))Target+=N->GetActorRightVector()*FMath::Cos(PerformanceTime*3)*10+FVector(0,0,FMath::Sin(PerformanceTime*3)*10);
   if(Performance==TEXT("write"))Target+=N->GetActorRightVector()*FMath::Sin(PerformanceTime*9)*4;
   if(Performance==TEXT("wash"))Target+=N->GetActorRightVector()*FMath::Sin(PerformanceTime*8)*3;
   Contact(N,Target,1,W);Contact(N,Target-N->GetActorRightVector()*16,0,W);
   if(Tool){Tool->SetStaticMesh(Director->World->Mesh(TEXT("RepairTool77")));Tool->SetVisibility(W>.05&&Performance!=TEXT("brace")&&Performance!=TEXT("wash"));Tool->SetWorldLocation(Target);Tool->SetWorldScale3D(Performance==TEXT("write")?FVector(.35):FVector(1));Tool->SetWorldRotation(N->GetActorRotation()+FRotator(0,0,FMath::Sin(PerformanceTime*7)*30));}
  }
  S.PerformanceTime77=PerformanceTime;if(PerformanceTime>=PerformanceDuration&&Director->WorkAction.IsNone()){StopPerformance();S.Performance77=NAME_None;}
 }
 if(Director->State().Stage==TEXT("guard_convoy")&&!S.Values.FindRef(TEXT("tomas_released"))&&Performance.IsNone())if(auto* N=Director->People.FindRef(TEXT("tomas")).Get()){
  N->LifeAnimation->PerformancePose77=1;const FVector Mid=N->GetActorTransform().TransformPosition(FVector(24,-16,14));Contact(N,Mid+N->GetActorRightVector()*5,1);Contact(N,Mid-N->GetActorRightVector()*5,0);if(Cuffs){Cuffs->SetVisibility(true);Cuffs->SetWorldLocation(Mid);}
 }
 UpdateCarrier(Dt);UpdateCrowd(Dt);
 if(Mechanisms.Num()>=4){const bool Running=S.Values.FindRef(TEXT("terminal_working"))!=0;const float Target=Running?1:0;MechanismPhase=FMath::FInterpConstantTo(MechanismPhase,Target,Dt,.09f);
  S.Values.Add(TEXT("canal_phase77"),FMath::RoundToInt(MechanismPhase*10000));
  Mechanisms[0]->SetRelativeRotation(FRotator(0,-90-MechanismPhase*65,0));Mechanisms[1]->SetRelativeRotation(FRotator(0,90+MechanismPhase*65,0));
  Mechanisms[2]->SetRelativeRotation(FRotator(90,0,MechanismPhase*1080));Mechanisms[3]->SetRelativeRotation(FRotator(MechanismPhase*1440,0,90));
 }
}
