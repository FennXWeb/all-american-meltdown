#include "Kismet/GameplayStatics.h"
#include "LWLoading45.h"
#include "ProceduralMeshComponent.h"
#include "LWInteriors65.h"
#include "LWUnderground.h"
#include "LWDungeon.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "LWResident.h"
#include "Components/StaticMeshComponent.h"
#include "LWWorldTextComponent.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "EngineUtils.h"
void ALWUndergroundDoor::Use(ALWCharacter* P){if(!P||!P->CanAct()||P->Vehicle)return;if(PrepareInterior68){FLWLoadingScope45 Loading(TEXT("Entering underground"));auto Work=MoveTemp(PrepareInterior68);Work();}const FVector From=P->GetActorLocation();P->ClosePanels();P->CancelReload();P->GetCharacterMovement()->StopMovementImmediately();P->SetActorLocation(Destination,false,nullptr,ETeleportType::TeleportPhysics);P->World->Sound(TEXT("BunkerDoor"),Destination);int I=0;for(TActorIterator<ALWResident> N(GetWorld());N;++N)if(P->RPG.Crew.ContainsByPredicate([&](const auto& C){return C.Id==N->ResidentId&&C.Following;})&&FVector::Dist2D(N->GetActorLocation(),From)<1600){FVector To=Destination+FVector((I%3-1)*100,100+(I/3)*100,0);N->SetActorLocation(To,false,nullptr,ETeleportType::TeleportPhysics);N->ResetCompanionNavigation();I++;}P->PersistWorldChange();}
void ALWChunk::BuildingUnderground(ALWWorld* W,const LWGen::FSite& Site){const FVector Base=FVector(Site.Position,0)-GetActorLocation();const FRotator R(0,Site.Yaw,0);Box(W,TEXT("Concrete"),Base+FVector(0,0,15),FVector(2100,1900,30),R);for(int Side:{-1,1})Box(W,TEXT("Concrete"),Base+R.RotateVector(FVector(Side*850,0,240)),FVector(80,1500,480),R);Box(W,TEXT("Steel"),Base+FVector(0,0,500),FVector(1800,1600,60),R);Add(W,TEXT("Generator"),NAME_None,Base+R.RotateVector(FVector(-550,350,30)),FVector(1),R);
 LWGen::FSite Sub=Site;Sub.Type=Site.Type;Sub.Id=Site.Id^0x29aaa;Sub.Size=LWDungeons::Size(Sub.Type);
 auto PrepareInterior=[this,W,Site,Sub,R](){
 auto* Child=GetWorld()->SpawnActor<ALWChunk>();if(!Child)return;Residents.Add(Child);Child->SyncCollision68=true;Child->Terrain->bUseAsyncCooking=false;const auto& Profile=LWDungeons::Profile(Sub.Type);{LWInteriors65::FScope UndergroundInterior65(Child,W,Sub);Child->BuildingDungeon(W,Sub);
 if(Site.Type==54){FRandomStream Random(Site.Id);for(int F=0;F<Profile.Floors;F++)for(int I=0;I<Profile.Columns*Profile.Rows;I++){const auto V=LWDungeons::Room(Sub.Type,F,I);for(int Side:{-1,1})for(int Sign:{-1,1})Child->Add(W,TEXT("Sphere"),TEXT("Concrete"),FVector(Site.Position,0)+R.RotateVector(V+FVector(Side*590,Sign*570,210)),FVector(3.2f,3.1f,4.7f+Random.FRand()),R);}for(int F=0;F<Profile.Floors;F++)for(int I=0;I<Profile.Columns*Profile.Rows;I++){const FVector Room=LWDungeons::Room(Sub.Type,F,I);for(int Side:{-1,1})for(int Corner:{-1,1}){const FVector Rock=Room+FVector(Side*750,Corner*480,240);Child->Add(W,TEXT("Rubble"),TEXT("Concrete"),FVector(Site.Position,0)+R.RotateVector(Rock),FVector(4,4,8),R+FRotator(Side*90,0,0),false);}}TInlineComponentArray<UTextRenderComponent*> Labels;Child->GetComponents(Labels);for(auto* Label:Labels)if(Label->Text.ToString().Contains(TEXT("WATER")))Label->SetText(FText::FromString(TEXT("FRACTURE CAVERNS / OLD PUMP WORKS")));}
 }
 Child->SetActorLocation(FVector(0,0,-9000));for(auto A:Child->Residents)if(IsValid(A)){A->AddActorWorldOffset(FVector(0,0,-9000));if(auto* O=Cast<ALWWorldObject>(A))if(auto* Record=W->Containers.Find(O->RecordId))Record->Position=O->GetActorLocation();if(auto* D=Cast<ALWDungeon>(A))for(auto& G:D->Guards)G.Position.Z-=9000;}
 };
 auto* Returning=UGameplayStatics::GetPlayerPawn(this,0);const bool RestoreInterior=Returning&&Returning->GetActorLocation().Z<-5000&&FVector::Dist2D(Returning->GetActorLocation(),FVector(Site.Position,0))<Sub.Size.Size()*.5+600;
 if(RestoreInterior)PrepareInterior();
 const FVector Arrival=FVector(Site.Position,-9000)+R.RotateVector(LWDungeons::Room(Sub.Type,0,0)+FVector(0,0,96));
 auto Door=[&](FVector At,FVector To,bool IsExit){auto* O=GetWorld()->SpawnActor<ALWUndergroundDoor>(At,R);O->Configure(W,ELWObjectKind::Container,FName(*FString::Printf(TEXT("underground_%u_%s"),Site.Id,IsExit?TEXT("exit"):TEXT("entry"))));O->Body->SetStaticMesh(W->Mesh(TEXT("Cube")));O->Body->SetMaterial(0,W->Material(TEXT("Steel")));O->Body->SetRelativeScale3D(FVector(2.5,.25,2.6));O->Body->SetRelativeLocation(FVector(0,0,130));O->Destination=To;O->Exit=IsExit;if(!IsExit&&!RestoreInterior)O->PrepareInterior68=MoveTemp(PrepareInterior);Residents.Add(O);};Door(FVector(Site.Position,30)+R.RotateVector(FVector(0,300,0)),Arrival,false);Door(Arrival+R.RotateVector(FVector(0,-420,-96)),FVector(Site.Position,130)+R.RotateVector(FVector(0,-550,0)),true);
 Box(W,TEXT("Steel"),Base+R.RotateVector(FVector(0,-790,380)),FVector(1650,35,160),R);auto* Label=NewObject<ULWWorldTextComponent>(this);Label->SetupAttachment(RootComponent);Label->SetRelativeLocation(Base+R.RotateVector(FVector(0,-810,350)));Label->SetRelativeRotation(R+FRotator(0,-90,0));Label->SetWorldSize(32);Label->SetHorizontalAlignment(EHTA_Center);Label->SetText(FText::FromString(LWPlaces::Name(Site.Type)));Label->RegisterComponent();}
