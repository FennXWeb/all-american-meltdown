#pragma once
#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LWWorldObject.h"
#include "LWRPG.h"
#include "LWOpening.h"
#include "LWVehicleState.h"
#include "LWEncounter.h"
#include "LWCardGame.h"
#include "LWMissionRecovery37.h"
#include "LWBunker45State.h"
#include "LWSaveGame.generated.h"
UCLASS()
class LETHALWORLD_API ULWSaveGame : public USaveGame
{
    GENERATED_BODY()
public:
    UPROPERTY() FLWBunker45State Bunker45;
    UPROPERTY() TMap<FName,FLWMissionRecovery37> MissionRecovery37;
    UPROPERTY() FLWCardGame Cards;
    UPROPERTY() FLWEncounterState Encounters;
    UPROPERTY() FLWRPGState RPG;
    UPROPERTY() FLWIdentity Identity;
    UPROPERTY() TMap<FName,FLWVehicleRecord> Vehicles;
    UPROPERTY() FName SeatedVehicle;
    // Legacy saves omitted the default value. Keep the CDO at 1; writers explicitly stamp 2.
    UPROPERTY() int32 Version=1;
    UPROPERTY() TArray<FLWItemInstance> Inventory;
    UPROPERTY() TArray<FLWItemInstance> Stash;
    UPROPERTY() TMap<FName,FLWContainerRecord> WorldContainers;
    UPROPERTY() TMap<FName,int32> PropStates;
    UPROPERTY() FGuid ActiveWeaponId;
    UPROPERTY() FName LastDeathBag;
    UPROPERTY() bool HasWaypoint=false;
    UPROPERTY() FVector2D Waypoint=FVector2D::ZeroVector;
    UPROPERTY() int32 TownSetting=1;
    UPROPERTY() int32 POISetting=1;
    UPROPERTY() int32 TerrainSetting=1;
    UPROPERTY() int32 Difficulty=1;
    UPROPERTY() float SurfaceWetness=0;
    UPROPERTY() int32 Seed=198706;
    UPROPERTY() uint8 PlayerStance54=0; // 0 standing, 1 crouched, 2 prone; legacy saves default to standing.
    UPROPERTY() FVector Position=FVector(0,2200,250);
    UPROPERTY() FRotator View;
    UPROPERTY() float Health=100;
    UPROPERTY() float Stamina=100;
    UPROPERTY() float Hunger=100;
    UPROPERTY() float Thirst=100;
    UPROPERTY() int64 Money=0;
    UPROPERTY() int32 Shells=6;
    UPROPERTY() int32 ReserveShells=24;
    UPROPERTY() int32 Weapon=0;
    UPROPERTY() int32 Kills=0;
    UPROPERTY() int32 DayNumber=1;
    UPROPERTY() float TimeOfDay=6.f;
    UPROPERTY() TArray<uint32> Killed;
    UPROPERTY() float Sensitivity=.075f;
    UPROPERTY() float MasterVolume=.8f;
    UPROPERTY() bool Crust=true;
};
