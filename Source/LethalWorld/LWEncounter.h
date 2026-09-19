#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LWWorldObject.h"
#include "LWEncounter.generated.h"

UENUM(BlueprintType)
enum class ELWEncounterTask:uint8 { Aid,Repair,Trade,Defense,Rescue,Cache,Ambush,Escort,Hunt,Signal,Memorial,Hazard,Salvage,Toll,Survey };
UENUM(BlueprintType)
enum class ELWEncounterSetting:uint8 { Camp,Motor,Checkpoint,Medical,Radio,Grave,Supply,Utility };
USTRUCT(BlueprintType)
struct FLWEncounterDefinition {
 GENERATED_BODY()
 UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Id;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) FString Title;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(MultiLine=true)) FString Intro;
 UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(MultiLine=true)) FString Outcome;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) ELWEncounterTask Task=ELWEncounterTask::Aid;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) ELWEncounterSetting Setting=ELWEncounterSetting::Camp;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Cue=TEXT("Speech0");
 UPROPERTY(EditAnywhere,BlueprintReadWrite) FName CostItem;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Cost=0;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) FName RewardItem=TEXT("food");
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 RewardCount=1;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Credits=40;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 XP=40;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 EnemyKind=0;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Enemies=0;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 MinLevel=1;
 // -1 any; 0 dry; 1 rain. Hours: 0 any, 1 day, 2 night.
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Weather=-1;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 Hours=0;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 NearPOI=-1;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) float Weight=1;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) float WorkSeconds=25;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) float Lifetime=900;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) float Cooldown=1800;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) FName Prerequisite;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) bool Enabled=true;
};
USTRUCT()
struct FLWEncounterRecord {
 GENERATED_BODY()
 UPROPERTY() FName Id;
 UPROPERTY() FName Type;
 UPROPERTY() FVector Position=FVector::ZeroVector;
 UPROPERTY() float Yaw=0;
 // 0 discovered opportunity, 1 active, 2 completed, 3 failed/declined.
 UPROPERTY() int32 Stage=0;
 UPROPERTY() int32 Choice=0;
 UPROPERTY() float Progress=0;
 UPROPERTY() float Integrity=100;
 UPROPERTY() double Created=0;
 UPROPERTY() double Expires=0;
 UPROPERTY() bool Seen=false;
 UPROPERTY() bool Rewarded=false;
 UPROPERTY() bool Paid=false;
 UPROPERTY() bool EnemiesSpawned=false;
 UPROPERTY() TArray<float> EnemyHealth;
 UPROPERTY() TArray<FVector> EnemyPositions;
 UPROPERTY() FVector EscortPosition=FVector::ZeroVector;
 UPROPERTY() FVector Goal=FVector::ZeroVector;
 UPROPERTY() FString Result;
};
USTRUCT()
struct FLWEncounterState {
 GENERATED_BODY()
 UPROPERTY() TMap<FName,FLWEncounterRecord> Records;
 UPROPERTY() TMap<FName,double> LastType;
 UPROPERTY() TSet<FName> Completed;
 UPROPERTY() TArray<FName> Recent;
 UPROPERTY() double Elapsed=0;
 UPROPERTY() double NextAttempt=35;
 UPROPERTY() int32 Serial=0;
 UPROPERTY() int32 Reputation=0;
 UPROPERTY() FVector LastPosition=FVector::ZeroVector;
};
UCLASS(BlueprintType)
class LETHALWORLD_API ULWEncounterCatalog:public UDataAsset {
 GENERATED_BODY()
public:
 ULWEncounterCatalog();
 UPROPERTY(EditAnywhere,BlueprintReadWrite,meta=(TitleProperty="Title")) TArray<FLWEncounterDefinition> Events;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) float FrequencyMultiplier=1.8f;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) float MinInterval=90;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) float MaxInterval=180;
 UPROPERTY(EditAnywhere,BlueprintReadWrite) int32 MaxActive=2;
 UFUNCTION(CallInEditor,Category="Encounters") void ResetToNativeDefaults();
 static const ULWEncounterCatalog* Get();
 const FLWEncounterDefinition* Find(FName Id)const;
};
namespace LWEncounters {
 LETHALWORLD_API bool Hostile(const FLWEncounterDefinition& D);
 LETHALWORLD_API bool Eligible(const FLWEncounterDefinition& D,const FLWEncounterState& S,int Level,float Hour,float Rain,bool DangerAllowed);
 LETHALWORLD_API const FLWEncounterDefinition* Select(const ULWEncounterCatalog& C,const FLWEncounterState& S,int Seed,int Level,float Hour,float Rain,bool DangerAllowed);
}
UCLASS()
class LETHALWORLD_API ALWEncounterScene:public ALWWorldObject {
 GENERATED_BODY()
public:
 UPROPERTY() TArray<TObjectPtr<class ALWZombie>> Enemies;
 UPROPERTY() TArray<TObjectPtr<AActor>> Owned;
 UPROPERTY() TObjectPtr<class ALWResident> Witness;
 UPROPERTY() TObjectPtr<class UPointLightComponent> Beacon;
 float Pulse=0,TalkClock=0,Think=0;
 int32 RouteStep=0;
 TArray<FVector2D> EscortRoute;
 void Initialize(class ALWWorld* W,FName Id);
 FLWEncounterRecord* Record()const;
 const FLWEncounterDefinition* Definition()const;
 virtual void Tick(float Dt)override;
 virtual void EndPlay(const EEndPlayReason::Type Reason)override;
 virtual FString Prompt()const override;
 virtual void Use(class ALWCharacter* P)override;
 virtual float TakeDamage(float D,const FDamageEvent& E,AController* I,AActor* C)override;
 void Dialogue(class ALWCharacter* P);
 bool Choose(class ALWCharacter* P,int Choice);
 void SpawnEnemies(int Override=0);
 void Snapshot();
 int LivingEnemies()const;
 void Resolve(class ALWCharacter* P,bool Success,const FString& Result,int Rep=0);
 FString Objective()const;
};
