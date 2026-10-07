#include "LWCampaignProduction77.h"
#include "LWCampaign76.h"
#include "LWCharacter.h"
#include "LWWorld.h"
#include "LWNPCLife.h"
#include "LWCanada68.h"
#include "LWVehicle.h"
#include "Components/StaticMeshComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/AudioComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "Engine/World.h"
namespace {
void SeatPassengers77(ULWCampaignProduction77* R){
 auto* D=R->Director.Get();const auto* S=LWCampaign76::Stage(D->State().Stage);int I=0;if(!S)return;
 for(FName Id:S->Cast){if(!D->CastAvailable(Id)||D->Hostile(Id))continue;
  const bool Helm=Id==TEXT("ada")||(Id==TEXT("hank")&&!S->Cast.Contains(TEXT("ada")));auto* N=D->People.FindRef(Id).Get();const int Bench=I/2;const FVector Seat=Helm?FVector(0,442,145):FVector((Bench%2?1:-1)*172+(I%2?30:-30),-470+(Bench/2%3)*230,64);
  const FVector Pos=R->Boat->GetActorTransform().TransformPosition(Seat);
  if(!N){const auto* Person=LWCampaign76::Person(Id);if(!Person)continue;FActorSpawnParameters Params;Params.SpawnCollisionHandlingOverride=ESpawnActorCollisionHandlingMethod::AlwaysSpawn;N=D->GetWorld()->SpawnActor<ALWCampaignPerson76>(Pos,FRotator::ZeroRotator,Params);if(!N)continue;N->Campaign=D;N->Person=Id;N->ConfigureResident(FName(*(TEXT("c76_")+Id.ToString())),TEXT("campaign76"),Person->Name,I,Person->Female?1:0);N->Health=D->State().Health.Contains(Id)?D->State().Health[Id]:150;N->MaximumHealth=150;N->LegendaryInitialized=true;N->ChatterTime=99999;D->Actors.Add(N);D->People.Add(Id,N);}
  if(N->bDead)continue;N->SetActorLocation(Pos,false,nullptr,ETeleportType::TeleportPhysics);N->SetActorRotation(R->Boat->GetActorRotation()+FRotator(0,Helm?90:-90,0));N->MoveTime=0;N->LifeAnimation->PerformancePose77=Helm?0:3;N->LifeAnimation->HandWeights77[0]=N->LifeAnimation->HandWeights77[1]=0;if(Helm)N->Tags.AddUnique(TEXT("FerryCaptain77"));
  N->GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);N->AttachToComponent(R->Boat->GetRootComponent(),FAttachmentTransformRules::KeepWorldTransform);N->GetCharacterMovement()->DisableMovement();if(N->Gun)N->Gun->SetVisibility(false);if(!Helm)I++;
 }
}
}

void ULWCampaignProduction77::BuildFerry(){
 if(Boat)return;Boat=GetWorld()->SpawnActor<AActor>();auto* Root=NewObject<USceneComponent>(Boat);Boat->SetRootComponent(Root);Root->SetMobility(EComponentMobility::Movable);Root->RegisterComponent();
 Boat->SetActorTransform(LWProduction77::FerryPose(Director->State().FerrySeconds77));
 BoatDeck=Add(TEXT("Ferry77"),FVector::ZeroVector,FRotator::ZeroRotator,FVector(1),true,Boat);
 Gangway=Add(TEXT("Gangway77"),FVector(0,-720,10),FRotator::ZeroRotator,FVector(1),true,Boat);
 // Nav lights give an actual heading and shore silhouettes remain visible during passage.
 for(int Side:{-1,1}){
  auto* L=NewObject<UPointLightComponent>(Boat);L->SetupAttachment(Root);L->SetRelativeLocation(FVector(Side*160,470,245));L->SetLightColor(Side<0?FLinearColor(.12,1,.2):FLinearColor(1,.08,.04));L->SetIntensity(1600);L->SetAttenuationRadius(350);L->SetCastShadows(false);L->RegisterComponent();Lights.Add(L);
 }
 SeatPassengers77(this);
 if(Director->RoomSound){Director->RoomSound->Stop();Director->RoomSound->DestroyComponent();}
 Director->RoomSound=Director->World->Sound(TEXT("CarLoadDiesel"),Boat->GetActorLocation(),.36f);
}
bool ULWCampaignProduction77::BeginSailing(){
 if(!Director||!Director->Player||Director->State().Sailing77)return false;auto* P=Director->Player.Get();
 if(!Director->State().Values.FindRef(TEXT("humanitarian_permit"))||!LWCanada68::Deposit(P))return false;
 Director->Pause();P->ClosePanels();P->StopAttack();P->CancelReload();if(P->Vehicle)P->Vehicle->Exit(true);P->StandFromChair(true);Director->State().Scene=NAME_None;
 Director->State().Sailing77=true;Director->State().FerrySeconds77=0;Boarding=true;BuildFerry();if(!BoatDeck){Director->State().Sailing77=false;return false;}
 const FVector Foot=Boat->GetActorTransform().TransformPosition(FVector(0,-690,113));P->SetActorLocation(Foot,false,nullptr,ETeleportType::TeleportPhysics);P->GetCharacterMovement()->StopMovementImmediately();P->SetBase(BoatDeck);
 P->Controller->SetControlRotation(Boat->GetActorRotation()+FRotator(0,90,0));P->World->Stream(Foot,true);
 P->ClearWaypoint();P->Notify(TEXT("Board the ferry. Move forward onto the passenger deck."),6);P->RequestSave40();return true;
}
void ULWCampaignProduction77::RestoreSailing(){
 if(!Director||Boat)return;BuildFerry();if(!BoatDeck)return;auto* P=Director->Player.Get();
 // Save/load reconstructs the physical vessel at the saved journey time, before
 // restoring the player's floor base. No journey rewards are replayed.
 const FVector Local=Boat->GetActorTransform().InverseTransformPosition(P->GetActorLocation());
 if(FMath::Abs(Local.X)>245||Local.Y<-640||Local.Y>220||Local.Z<85||Local.Z>230)P->SetActorLocation(Boat->GetActorTransform().TransformPosition(FVector(0,-350,113)),false,nullptr,ETeleportType::TeleportPhysics);
 P->SetBase(BoatDeck);P->ClearWaypoint();Boarding=Director->State().FerrySeconds77<=0;
}
void ULWCampaignProduction77::UpdateSailing(float Dt){
 if(!Director||!Director->State().Sailing77)return;RestoreSailing();auto* P=Director->Player.Get();if(!Boat||!BoatDeck||P->IsUIOpen())return;
 const FVector Local=Boat->GetActorTransform().InverseTransformPosition(P->GetActorLocation());
 if(Boarding){if(Local.Y<-610)return;Boarding=false;P->Notify(TEXT("Lines aboard. Keep the center aisle clear."),5);Director->World->Sound(TEXT("CarIgnition"),Boat->GetActorLocation(),.35f);}
 const FTransform Old=Boat->GetActorTransform();Director->State().FerrySeconds77=FMath::Min(LWProduction77::FerryDuration(),Director->State().FerrySeconds77+Dt);
 Boat->SetActorTransform(LWProduction77::FerryPose(Director->State().FerrySeconds77),false,nullptr,ETeleportType::TeleportPhysics);
 for(const auto& Pair:Director->People)if(auto* N=Pair.Value.Get();N&&N->ActorHasTag(TEXT("FerryCaptain77")))for(int Side=0;Side<2;Side++)Contact(N,Boat->GetActorTransform().TransformPosition(FVector(Side?20:-20,457,170)),Side,1);
 if(Director->RoomSound){Director->RoomSound->SetWorldLocation(Boat->GetActorLocation());Director->RoomSound->SetPitchMultiplier(.8f+FMath::Sin(Director->State().FerrySeconds77/LWProduction77::FerryDuration()*PI)*.3f);}
 Gangway->SetRelativeRotation(FRotator(FMath::Lerp(0.f,80.f,FMath::SmoothStep(0.f,5.f,Director->State().FerrySeconds77)),0,0));Gangway->SetCollisionEnabled(Director->State().FerrySeconds77>2?ECollisionEnabled::NoCollision:ECollisionEnabled::QueryAndPhysics);
 // Carry world-space walking motion through the moving frame exactly once. The
 // base is deliberately detached: engine based movement would apply it a second time.
 P->SetBase(static_cast<UPrimitiveComponent*>(nullptr));P->SetActorLocation(Boat->GetActorTransform().TransformPosition(Local),false,nullptr,ETeleportType::TeleportPhysics);
 P->GetCharacterMovement()->Velocity=Boat->GetActorQuat().RotateVector(Old.GetRotation().UnrotateVector(P->GetCharacterMovement()->Velocity));
 if(Local.Z<40||FMath::Abs(Local.X)>310||Local.Y<-810||Local.Y>770){P->SetActorLocation(Boat->GetActorTransform().TransformPosition(FVector(0,-350,113)),false,nullptr,ETeleportType::TeleportPhysics);P->GetCharacterMovement()->StopMovementImmediately();P->Notify(TEXT("You regain your footing inside the rail."),4);}
 P->GetCharacterMovement()->bImpartBaseVelocityX=P->GetCharacterMovement()->bImpartBaseVelocityY=P->GetCharacterMovement()->bImpartBaseVelocityZ=false;
 if(Director->State().FerrySeconds77>=LWProduction77::FerryDuration()){
  Director->State().Values.Add(TEXT("ferry_landed77"),1);Director->State().Sailing77=false;
  P->GetCharacterMovement()->bImpartBaseVelocityX=P->GetCharacterMovement()->bImpartBaseVelocityY=P->GetCharacterMovement()->bImpartBaseVelocityZ=true;
  P->Notify(TEXT("Ontario landing. Northbank transport is waiting for Toronto."),6);Director->SetStage(TEXT("canada_arrival"));
 }
}
