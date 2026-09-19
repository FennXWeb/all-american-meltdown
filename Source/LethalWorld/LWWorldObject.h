#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWInventory.h"
#include "LWWorldObject.generated.h"

USTRUCT()
struct FLWContainerRecord
{
    GENERATED_BODY()
    UPROPERTY() FName Id;
    UPROPERTY() FName Context;
    UPROPERTY() FName Settlement;
    UPROPERTY() FVector Position=FVector::ZeroVector;
    UPROPERTY() TArray<FLWItemInstance> Items;
    UPROPERTY() bool bDropped=false;
    UPROPERTY() bool bTrader=false;
    UPROPERTY() int32 Width=12;
    UPROPERTY() int32 Height=12;
    UPROPERTY() int32 LockTier=0;
    UPROPERTY() bool Unlocked=false;
    UPROPERTY() int32 LockAttempts=0;
};

UENUM()
enum class ELWObjectKind:uint8 { Container,Trader,BunkerEntrance,BunkerExit,Stash,Door,Window,FuelPump,Car,Sign,Furniture,MannequinDisplay,CardTable };

UCLASS()
class LETHALWORLD_API ALWWorldObject:public AActor
{
    GENERATED_BODY()
public:
    ALWWorldObject();
    virtual void Tick(float Dt) override;
    virtual float TakeDamage(float Damage,const FDamageEvent& Event,AController* Instigator,AActor* Causer) override;
    UPROPERTY() FName UseType;
    void SetFurniture(FName Type);void UseFurniture(class ALWCharacter* P);
    bool CanCompanionOpenDoor()const;
    bool RequestCompanionDoor(AActor* User);
    void TickCompanionDoor();
    bool CompanionOpenedDoor=false;double CompanionDoorUntil=0,ManualDoorUntil=0;
    bool bChanged=false;
    float DoorAngle=0,EffectTime=0;
    void ConfigureProp();
    void SignFace(int32 Type);
    UPROPERTY() TObjectPtr<class USceneComponent> Root;
    UPROPERTY() TObjectPtr<class UStaticMeshComponent> Body;
    UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Details;
    UPROPERTY() TObjectPtr<class ALWWorld> World;
    UPROPERTY() ELWObjectKind Kind=ELWObjectKind::Container;
    UPROPERTY() FName RecordId;
    TArray<FVector2D> Route;
    int32 RouteIndex=0;
    float VoiceTimer=12;
    void Configure(class ALWWorld* W,ELWObjectKind InKind,FName Id);
    virtual void Use(class ALWCharacter* P);
    virtual FString Prompt() const;
    void Part(FName Mesh,FVector Position,FVector Scale,FRotator Rotation=FRotator::ZeroRotator,FName Material=NAME_None);
};
