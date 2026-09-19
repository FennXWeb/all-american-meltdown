#include "LWBunker45.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWWorldObject.h"
#include "LWVehicle.h"
#include "LWResident.h"
#include "LWWorldTextComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMeshActor.h"
#include "Engine/StaticMesh.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

ALWBunker45::ALWBunker45(){PrimaryActorTick.bCanEverTick=true;RootComponent=CreateDefaultSubobject<USceneComponent>(TEXT("Root"));BuildCamera=CreateDefaultSubobject<UCameraComponent>(TEXT("BuildCamera"));BuildCamera->SetupAttachment(RootComponent);Preview=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Preview"));Preview->SetupAttachment(RootComponent);Preview->SetCollisionEnabled(ECollisionEnabled::NoCollision);Preview->SetVisibility(false);}
ALWBunker45* ALWBunker45::Ensure(ALWCharacter* P){if(!P||!P->World)return nullptr;if(!IsValid(P->BunkerManager45)){auto* B=P->GetWorld()->SpawnActor<ALWBunker45>();P->BunkerManager45=B;B->Player=P;B->World=P->World;B->Rebuild();}return P->BunkerManager45;}
const TArray<FLWFurniture45>& ALWBunker45::Catalog(){static const TArray<FLWFurniture45> C={
 {TEXT("bed"),TEXT("HomeBedV13"),TEXT("bed"),TEXT("Single bed"),18},{TEXT("chair"),TEXT("ChairV3"),TEXT("chair"),TEXT("Dining chair"),5},{TEXT("locker"),TEXT("LockerV4"),TEXT("locker"),TEXT("Storage locker"),15},
 {TEXT("table"),TEXT("DiningTableV13"),TEXT("decor"),TEXT("Dining table"),12},{TEXT("desk"),TEXT("Desk"),TEXT("locker"),TEXT("Writing desk"),14},{TEXT("bookcase"),TEXT("BookcaseV13"),TEXT("locker"),TEXT("Bookcase"),12},
 {TEXT("pantry"),TEXT("PantryV13"),TEXT("locker"),TEXT("Pantry cabinet"),16},{TEXT("sideboard"),TEXT("SideboardV13"),TEXT("locker"),TEXT("Sideboard"),12},{TEXT("nightstand"),TEXT("NightstandV13"),TEXT("locker"),TEXT("Bedside drawer"),7},
 {TEXT("cooker"),TEXT("StoveV4"),TEXT("cooker"),TEXT("Kitchen stove"),25},{TEXT("sink"),TEXT("SinkV4"),TEXT("sink"),TEXT("Sink"),20},{TEXT("water"),TEXT("FridgeV4"),TEXT("water"),TEXT("Water dispenser"),25},
 {TEXT("workbench"),TEXT("WeaponBench39"),TEXT("workbench"),TEXT("Weapon workbench"),35},{TEXT("radio"),TEXT("RadioV4"),TEXT("radio"),TEXT("Radio"),10},{TEXT("lamp"),TEXT("TableLampV13"),TEXT("decor"),TEXT("Table lamp"),6},
 {TEXT("bin"),TEXT("WasteBinV13"),TEXT("decor"),TEXT("Waste bin"),4},{TEXT("crate"),TEXT("Crate"),TEXT("locker"),TEXT("Supply crate"),8},{TEXT("generator"),TEXT("Generator"),TEXT("decor"),TEXT("Generator"),30},
 {TEXT("supplies"),TEXT("DeskSetV13"),TEXT("decor"),TEXT("Desk accessories"),4},{TEXT("dishes"),TEXT("TableSettingV13"),TEXT("decor"),TEXT("Table setting"),3}};return C;}
UStaticMeshComponent* ALWBunker45::Box(FVector At,FVector Size,FName Material,bool Collision){auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(At,FRotator::ZeroRotator);A->SetMobility(EComponentMobility::Movable);auto* M=A->GetStaticMeshComponent();M->SetStaticMesh(World->Mesh(TEXT("Cube")));M->SetMaterial(0,World->Material(Material));M->SetWorldScale3D(Size/100);M->SetCollisionEnabled(Collision?ECollisionEnabled::QueryAndPhysics:ECollisionEnabled::NoCollision);M->SetCollisionProfileName(Collision?TEXT("BlockAll"):TEXT("NoCollision"));Built.Add(A);return M;}
ALWWorldObject* ALWBunker45::Terminal(FName Id,FVector At,FName Action){auto* O=World->SpawnObject(ELWObjectKind::Furniture,Id,At);O->UseType=Action;O->Body->SetStaticMesh(World->Mesh(TEXT("Cube")));O->Body->SetRelativeLocation(FVector(8,0,0));O->Body->SetRelativeScale3D(FVector(.16,.7,.82));O->Body->SetMaterial(0,World->Material(TEXT("Steel")));
 O->Part(TEXT("Cube"),FVector(17,0,7),FVector(.035,.60,.57),FRotator::ZeroRotator,TEXT("Rubber"));
 auto* Screen=NewObject<UStaticMeshComponent>(O);Screen->SetupAttachment(O->GetRootComponent());Screen->SetStaticMesh(World->Mesh(TEXT("Cube")));Screen->SetRelativeLocation(FVector(19,0,7));Screen->SetRelativeScale3D(FVector(.012,.48,.42));Screen->SetCollisionEnabled(ECollisionEnabled::NoCollision);Screen->RegisterComponent();
 auto* ScreenMat=UMaterialInstanceDynamic::Create(World->Material(TEXT("Sky")),O);ScreenMat->SetVectorParameterValue(TEXT("SkyColor"),FLinearColor(.004f,.025f,.008f));Screen->SetMaterial(0,ScreenMat);
 O->Part(TEXT("Cube"),FVector(19,0,-33),FVector(.29,.62,.065),FRotator(0,0,0),TEXT("Steel"));
 for(int Row=0;Row<3;++Row)for(int Key=0;Key<9;++Key)O->Part(TEXT("Cube"),FVector(10+Row*6,-24+Key*6,-28.8),FVector(.035,.045,.02),FRotator::ZeroRotator,TEXT("Rubber"));
 for(int Side:{-1,1})O->Part(TEXT("Cube"),FVector(17,Side*32,9),FVector(.04,.04,.67),FRotator::ZeroRotator,TEXT("Steel"));
 auto* Label=NewObject<ULWWorldTextComponent>(O);Label->SetupAttachment(O->GetRootComponent());Label->SetRelativeLocation(FVector(20,0,10));Label->SetText(FText::FromString(Action==TEXT("base45")?TEXT("SHELTER\nCONTROL"):Action==TEXT("garage45")?TEXT("GARAGE"):TEXT("LIFT")));Label->SetHorizontalAlignment(EHTA_Center);Label->SetWorldSize(7);Label->SetTextRenderColor(FColor(85,210,100));Label->RegisterComponent();Built.Add(O);return O;}
void ALWBunker45::Rebuild(){
 Close();TransferCar=nullptr;SurfacePlatform=nullptr;LiftGate=nullptr;if(Player->bSafehouse&&Player->GetActorLocation().Z<Center().Z-350)Player->SetActorLocation(Center()+FVector(-500,0,115),false,nullptr,ETeleportType::TeleportPhysics);BuildQueue.Empty();for(AActor* A:Built)if(IsValid(A))A->Destroy();Built.Empty();Furniture.Empty();Gates.Empty();GateFloors.Empty();Lift=nullptr;CabButton=nullptr;MovingLift=false;Floor=Destination=0;LiftZ=FloorZ(0)+20;
 for(int I=0;I<World->BunkerParts.Num();++I){auto* A=World->BunkerParts[I].Get();if(!IsValid(A))continue;auto* M=A->FindComponentByClass<UStaticMeshComponent>();if(!M||!M->GetStaticMesh())continue;
  const FString N=M->GetStaticMesh()->GetName();auto* O=Cast<ALWWorldObject>(A);if(N.Contains(TEXT("Cube"))||N.Contains(TEXT("CeilingLight"))||(O&&O->Kind!=ELWObjectKind::Furniture&&O->Kind!=ELWObjectKind::Container))continue;
  FName Id=O?O->RecordId:FName(*FString::Printf(TEXT("bunker_original45_%d"),I));Furniture.Add(Id,A);A->Tags.AddUnique(Id);
  if(!Originals.Contains(Id)){FLWPlaced45 V;V.Transform=A->GetActorTransform();V.Use=O?O->UseType:TEXT("decor");V.Cost=12;Originals.Add(Id,V);}A->SetActorTransform(Originals[Id].Transform);A->SetActorHiddenInGame(false);A->SetActorEnableCollision(true);
 }
 const FVector C=Center();if(!World->Containers.Contains(TEXT("bunker_work45"))){FLWContainerRecord Supply;Supply.Id=TEXT("bunker_work45");Supply.Context=TEXT("bunker");Supply.Width=12;Supply.Height=14;World->Containers.Add(Supply.Id,Supply);}auto* Supply=World->SpawnObject(ELWObjectKind::Furniture,TEXT("bunker_work45"),C+FVector(-1700,-140,24));Supply->SetFurniture(TEXT("locker"));Built.Add(Supply);Terminal(TEXT("bunker_terminal45"),C+FVector(-1885,340,150),TEXT("base45"));
 Surface=ALWWorld::BunkerDoorPosition()+FVector(0,-2700,0);Surface.Z=World->HeightAt(FVector2D(Surface))+18;
 if(Player->Bunker45.Garage){SurfacePlatform=Box(Surface,FVector(1500,700,20),TEXT("Steel"));Box(Surface+FVector(0,450,50),FVector(90,70,100),TEXT("Steel"));Terminal(TEXT("garage_surface45"),Surface+FVector(0,450,100),TEXT("surface45"));}
 for(int F=1;F<=Player->Bunker45.BedroomFloors;++F)QueueFloor(F);
 if(Player->Bunker45.Utilities.Num())QueueFloor(10);
 if(Player->Bunker45.Garage)QueueFloor(11,true);
 const bool Expanded=Player->Bunker45.BedroomFloors>0||Player->Bunker45.Garage||Player->Bunker45.Utilities.Num()>0;
 if(Expanded){
  Lift=Box(C+FVector(1700,0,20),FVector(340,320,20),TEXT("Steel"));LiftGate=Box(C+FVector(1535,0,155),FVector(15,320,270),TEXT("Steel"),false);LiftGate->SetVisibility(false);
  auto* EntryGate=Box(C+FVector(1515,0,160),FVector(18,320,280),TEXT("Steel"));Gates.Add(EntryGate);GateFloors.Add(0);
  int Bottom=Player->Bunker45.Garage?11:Player->Bunker45.Utilities.Num()?10:Player->Bunker45.BedroomFloors;float Depth=Bottom*460.f;for(int S:{-1,1})Box(C+FVector(1700,S*178,200-Depth*.5f),FVector(360,16,Depth+400),TEXT("Steel"));Box(C+FVector(1885,0,200-Depth*.5f),FVector(16,370,Depth+400),TEXT("Steel"));
  CabButton=Terminal(TEXT("lift_cab45"),C+FVector(1800,-120,105),TEXT("lift45"));
  Terminal(TEXT("lift_call45_0"),C+FVector(1470,210,105),TEXT("call45_0"));
 }else Box(C+FVector(1700,0,0),FVector(360,340,40),TEXT("Concrete"));
 BuildQueue.Add([this](){ApplyFurniture();Message=TEXT("Construction complete");});
}
void ALWBunker45::QueueFloor(int Index,bool Garage){
 BuildQueue.Add([this,Index,Garage](){const FVector C(-1700,1700,FloorZ(Index));float W=Garage?5000:3840,H=Garage?4900:3140;
  // Four slabs leave an actual elevator shaft, rather than a platform passing through floors.
  Box(C+FVector((-W/2+1520)/2,0,0),FVector(W/2+1520,H,40),TEXT("Concrete"));Box(C+FVector((1880+W/2)/2,0,0),FVector(W/2-1880,H,40),TEXT("Concrete"));for(int S:{-1,1})Box(C+FVector(1700,S*(H/4+85),0),FVector(360,H/2-170,40),TEXT("Concrete"));
  Box(C+FVector((-W/2+1520)/2,0,425),FVector(W/2+1520,H,30),TEXT("Steel"));Box(C+FVector((1880+W/2)/2,0,425),FVector(W/2-1880,H,30),TEXT("Steel"));for(int S:{-1,1})Box(C+FVector(1700,S*(H/4+85),425),FVector(360,H/2-170,30),TEXT("Steel"));for(int S:{-1,1}){Box(C+FVector(S*W/2,0,210),FVector(24,H,420),TEXT("Concrete"));Box(C+FVector(0,S*H/2,210),FVector(W,24,420),TEXT("Concrete"));}
  auto* Gate=Box(C+FVector(1515,0,160),FVector(18,320,280),TEXT("Steel"));Gates.Add(Gate);GateFloors.Add(Index);Terminal(FName(*FString::Printf(TEXT("lift_call45_%d"),Index)),C+FVector(1450,215,110),FName(*FString::Printf(TEXT("call45_%d"),Index)));
  auto* L=GetWorld()->SpawnActor<AActor>();auto* Light=NewObject<UPointLightComponent>(L);L->SetRootComponent(Light);Light->SetWorldLocation(C+FVector(0,0,350));Light->SetIntensity(45000);Light->SetAttenuationRadius(Garage?3400:2400);Light->SetCastShadows(false);Light->RegisterComponent();Built.Add(L);
  if(Garage){
   for(int Row:{-1,1})for(int Col:{-1,0,1}){FVector At=C+FVector(Col*1500,Row*1300,365);auto* A=GetWorld()->SpawnActor<AStaticMeshActor>(At,FRotator::ZeroRotator);A->SetMobility(EComponentMobility::Movable);A->GetStaticMeshComponent()->SetStaticMesh(World->Mesh(TEXT("CeilingLightV13")));A->GetStaticMeshComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);Built.Add(A);auto* P=NewObject<UPointLightComponent>(A);P->SetupAttachment(A->GetRootComponent());P->SetRelativeLocation(FVector(0,0,-30));P->SetMobility(EComponentMobility::Movable);P->SetIntensity(250000);P->SetAttenuationRadius(1800);P->SetCastShadows(false);P->RegisterComponent();}
   for(int I=0;I<4;++I){auto* F=World->SpawnObject(ELWObjectKind::Furniture,FName(*FString::Printf(TEXT("bunker_garage45_%d"),I)),C+FVector(-2300,(-1.5+I)*260,24),FRotator(0,90,0));F->SetFurniture(I%2?TEXT("locker"):TEXT("workbench"));Built.Add(F);Furniture.Add(F->RecordId,F);}
   Terminal(TEXT("garage_terminal45"),C+FVector(1200,320,110),TEXT("garage45"));for(int B=0;B<10;++B){FVector At(-1600+(B%5)*800,(B/5)?1500:-1500,22);for(int S:{-1,1})Box(C+At+FVector(S*365,0,0),FVector(5,1700,2),TEXT("Bone"),false);
    auto* Label=GetWorld()->SpawnActor<AActor>();auto* Text=NewObject<ULWWorldTextComponent>(Label);Label->SetRootComponent(Text);Text->SetWorldLocation(C+FVector(At.X,B<5?-2400:2400,185));Text->SetWorldRotation(FRotator(0,B<5?90:-90,0));Text->SetText(FText::FromString(FString::Printf(TEXT("BAY %02d"),B+1)));Text->SetWorldSize(48);Text->SetHorizontalAlignment(EHTA_Center);Text->SetTextRenderColor(FColor(230,210,125));Text->RegisterComponent();Built.Add(Label);}return;}
 });
 if(Index<10)for(int I=0;I<10;++I)BuildQueue.Add([this,Index,I](){const FVector C(-1700,1700,FloorZ(Index));int S=I<5?-1:1;float X=-1200+(I%5)*600;
  for(int E:{-1,1})Box(C+FVector(X+E*300,S*650,210),FVector(18,800,420),TEXT("Concrete"));Box(C+FVector(X,S*1050,210),FVector(600,18,420),TEXT("Concrete"));for(int E:{-1,1})Box(C+FVector(X+E*190,S*250,210),FVector(220,18,420),TEXT("Concrete"));Box(C+FVector(X,S*250,355),FVector(160,18,130),TEXT("Concrete"));
  auto* D=World->SpawnObject(ELWObjectKind::Door,FName(*FString::Printf(TEXT("bed45_d_%d_%d"),Index,I)),C+FVector(X+(S<0?80:-80),S*250,24),FRotator(0,S<0?180:0,0));Built.Add(D);
  for(int J=0;J<3;++J){FName Id(*FString::Printf(TEXT("bunker_exp45_%d_%d_%d"),Index,I,J));auto* F=World->SpawnObject(ELWObjectKind::Furniture,Id,C+FVector(X+(J==0?-150:180),S*(J==2?530:760),24));F->SetFurniture(J==0?TEXT("bed"):J==1?TEXT("locker"):TEXT("chair"));if(J==0)F->Body->SetStaticMesh(World->Mesh(TEXT("HomeBedV13")));Built.Add(F);Furniture.Add(Id,F);}
 });
 if(Index==10){int I=0;for(FName Type:Player->Bunker45.Utilities){int Room=I++;BuildQueue.Add([this,Type,Room](){FVector At=UtilityPosition(Type);Box(At+FVector(0,0,-95),FVector(1000,800,10),TEXT("Wood"));for(int S:{-1,1})Box(At+FVector(S*510,0,110),FVector(18,800,400),TEXT("Concrete"));Box(At+FVector(0,400,110),FVector(1000,18,400),TEXT("Concrete"));for(int J=0;J<3;++J){auto* F=World->SpawnObject(ELWObjectKind::Furniture,FName(*FString::Printf(TEXT("bunker_job45_%s_%d"),*Type.ToString(),J)),At+FVector((J-1)*240,150,-86));F->SetFurniture(J==0?TEXT("workbench"):J==1?TEXT("chair"):TEXT("locker"));Built.Add(F);Furniture.Add(F->RecordId,F);}});}}
}
FVector ALWBunker45::UtilityPosition(FName Type)const{const TArray<FName> Types={TEXT("trade"),TEXT("salvage"),TEXT("recruit"),TEXT("contracts"),TEXT("medical"),TEXT("farm")};int I=FMath::Max(0,Types.Find(Type));return FVector(-1700-1150+(I%3)*1100,1700+(I<3?-1:1)*850,FloorZ(10)+110);}
void ALWBunker45::ApplyFurniture(){for(auto& Pair:Player->Bunker45.Furniture){auto& V=Pair.Value;AActor* A=Furniture.FindRef(Pair.Key);if(!A&&!V.Removed&&!V.Model.IsNone()){auto* F=World->SpawnObject(ELWObjectKind::Furniture,Pair.Key,V.Transform.GetLocation(),V.Transform.Rotator());F->SetFurniture(V.Use);F->Body->SetStaticMesh(World->Mesh(V.Model));Built.Add(F);Furniture.Add(Pair.Key,F);A=F;}if(!IsValid(A))continue;A->SetActorHiddenInGame(V.Removed);A->SetActorEnableCollision(!V.Removed);if(!V.Removed)A->SetActorTransform(V.Transform);if(auto* C=World->Containers.Find(Pair.Key))C->Position=V.Transform.GetLocation();}}
void ALWBunker45::Open(int NewTab){if(!Player||Player->Vehicle)return;Player->ClosePanels();Player->BaseUI45=true;Focus46=NAME_None;Entries46.Empty();Player->SetMenuInput(true);Tab=NewTab;Page=0;Player->bTrigger=false;Player->CancelReload();}
void ALWBunker45::Close(){if(!Player)return;Player->BaseUI45=false;Player->BuildMode45=false;Preview->SetVisibility(false);if(auto* PC=Cast<APlayerController>(Player->Controller))PC->SetViewTarget(Player);Player->GetCharacterMovement()->SetMovementMode(MOVE_Walking);Player->SetMenuInput(Player->bMenu);SelectedFurniture=NAME_None;SelectedCatalog=-1;}
void ALWBunker45::Tick(float Dt){Super::Tick(Dt);if(!IsValid(Player)||!IsValid(World)||!Player->bStarted||Player->bMenu)return;
 if(BuildQueue.Num()){const double Until=FPlatformTime::Seconds()+.002;do{auto Job=MoveTemp(BuildQueue[0]);BuildQueue.RemoveAt(0);Job();}while(BuildQueue.Num()&&FPlatformTime::Seconds()<Until);}
 if(Player->BaseUI45)TickTerminal46();if(Player->BuildMode45)TickBuild(Dt);TickLift(Dt);TickGarage(Dt);TickUtilities(Dt);
}


