#pragma once
#include "CoreMinimal.h"
#include "LWCampaign76State.generated.h"

// Campaign-specific facts travel in the existing RPG/manual-save/checkpoint payload.
USTRUCT()
struct FLWCampaign76State {
 GENERATED_BODY()
 UPROPERTY() bool Started=false;
 UPROPERTY() bool Paused=false;
 UPROPERTY() FName Stage;
 UPROPERTY() FName Scene;
 UPROPERTY() int32 Beat=0;
 UPROPERTY() FName AfterScene;
 UPROPERTY() int32 Revision=0;
 UPROPERTY() TMap<FName,int32> Values;
 UPROPERTY() TSet<FName> Decisions;
 UPROPERTY() TSet<FName> Rewards;
 UPROPERTY() TSet<FName> Completed;
 UPROPERTY() TArray<FString> Journal;
 UPROPERTY() TMap<FName,float> Health;
 UPROPERTY() TSet<int32> Defeated;
 UPROPERTY() float EncounterSeconds=0;
 UPROPERTY() FName Ending;
 UPROPERTY() float FerrySeconds77=0;
 UPROPERTY() bool Sailing77=false;
 UPROPERTY() float CarrierDamage77=0;
 UPROPERTY() FName Work77;
 UPROPERTY() float WorkSeconds77=0;
 UPROPERTY() FName Performance77;
 UPROPERTY() FName Performer77;
 UPROPERTY() FName Recipient77;
 UPROPERTY() float PerformanceTime77=0;
 UPROPERTY() float PerformanceDuration77=0;
};
