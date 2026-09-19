#include "LWInteractable.h"
#include "Engine/World.h"
#include "LWWorld.h"
#include "LWCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AudioComponent.h"
ALWInteractable::ALWInteractable()
{
    Mesh=CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ReliefTerminal")); SetRootComponent(Mesh);
    Mesh->SetCollisionProfileName(TEXT("BlockAll"));
}
void ALWInteractable::Configure(ALWWorld* World) { Mesh->SetStaticMesh(World->Mesh(TEXT("Generator")));Hum=World->Sound(TEXT("Generator"),GetActorLocation()+FVector(0,0,50),.22f); }
void ALWInteractable::EndPlay(const EEndPlayReason::Type Reason){if(Hum)Hum->Stop();Super::EndPlay(Reason);}
FString ALWInteractable::Prompt() const
{
    const float Wait=ReadyAt-GetWorld()->GetTimeSeconds();
    return Wait>0?FString::Printf(TEXT("RELIEF STATION // RECYCLE %02ds"),FMath::CeilToInt(Wait)):TEXT("[E] FIELD RATIONS / WATER / FIRST AID");
}
void ALWInteractable::Use(ALWCharacter* P)
{
    if(GetWorld()->GetTimeSeconds()<ReadyAt) return;
    P->Health=100; P->Hunger=100; P->Thirst=100;
    const int Missing=FMath::Max(0,18-P->AmmoCount(TEXT("ammo_12g")));if(Missing)P->GiveItem(TEXT("ammo_12g"),Missing);P->SyncAmmoHUD();
    ReadyAt=GetWorld()->GetTimeSeconds()+90;
    P->Notify(TEXT("RELIEF DISPENSED // STAY ALIVE"),4);
    ALWWorld::Get(this)->Sound(TEXT("Credit"),GetActorLocation(),.65f);
    P->RequestSave40();
}
