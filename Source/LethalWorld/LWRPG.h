#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LWInventory.h"
#include "LWStoryState.h"
#include "LWCanada68State.h"
#include "LWCampaign76State.h"
#include "LWSettlement82State.h"
#include "LWRPG.generated.h"

USTRUCT(BlueprintType)
struct FLWPerk {
 GENERATED_BODY()
 UPROPERTY(EditAnywhere) FName Id;
 UPROPERTY(EditAnywhere) FString Name;
 UPROPERTY(EditAnywhere) int32 Category=0;
 UPROPERTY(EditAnywhere) int32 AttributeRequired=1;
 UPROPERTY(EditAnywhere) int32 LevelRequired=1;
 UPROPERTY(EditAnywhere) int32 MaxRank=3;
 UPROPERTY(EditAnywhere) FName Effect;
 UPROPERTY(EditAnywhere) float Value=.05f;
 UPROPERTY(EditAnywhere) FString Description;
};
USTRUCT(BlueprintType)
struct FLWObjective {
 GENERATED_BODY()
 UPROPERTY(EditAnywhere) FName Event;
 UPROPERTY(EditAnywhere) FName Target;
 UPROPERTY(EditAnywhere) int32 Count=1;
 UPROPERTY(EditAnywhere) FString Text;
};
USTRUCT(BlueprintType)
struct FLWQuest {
 GENERATED_BODY()
 UPROPERTY(EditAnywhere) FName Id;
 UPROPERTY(EditAnywhere) FName Prerequisite;
 UPROPERTY(EditAnywhere) FString Title;
 UPROPERTY(EditAnywhere) FString Description;
 UPROPERTY(EditAnywhere) TArray<FLWObjective> Objectives;
 UPROPERTY(EditAnywhere) int32 XP=150;
 UPROPERTY(EditAnywhere) int32 Credits=100;
};
USTRUCT()
struct FLWQuestState {
 GENERATED_BODY()
 UPROPERTY() FName Id;
 UPROPERTY() int32 Stage=0;
 UPROPERTY() int32 Progress=0;
 UPROPERTY() bool Rewarded=false;
};
USTRUCT()
struct FLWConsignment {
 GENERATED_BODY()
 UPROPERTY() TArray<FLWItemInstance> Items;
 UPROPERTY() FString Name;
 UPROPERTY() int32 Payout=0;
 UPROPERTY() double SaleHour=0;
};
USTRUCT()
struct FLWSettlementRecord {
 GENERATED_BODY()
 UPROPERTY() FString Name;
 UPROPERTY() FVector Center=FVector::ZeroVector;
 UPROPERTY() int32 Size=0;
 UPROPERTY() int32 Reputation=0;
 UPROPERTY() double HostileUntil=0;
 UPROPERTY() double LastEconomyHour=0;
 UPROPERTY() double LastTradeHour=-24;
 UPROPERTY() int32 TradeRep=0;
 UPROPERTY() bool Member=false;
 UPROPERTY() bool Leader=false;
 UPROPERTY() int64 Treasury=0;
 UPROPERTY() TMap<FName,FString> Names;
 UPROPERTY() TArray<FLWConsignment> Listings;
};
USTRUCT()
struct FLWRespawnPoint {
 GENERATED_BODY()
 UPROPERTY() bool Enabled=false;
 UPROPERTY() FVector Position=FVector::ZeroVector;
 UPROPERTY() FName Vehicle;
 UPROPERTY() FName Settlement;
 UPROPERTY() FString Name;
};
USTRUCT()
struct FLWCrewRecord {
 GENERATED_BODY()
 UPROPERTY() FName Id;
 UPROPERTY() FString Name;
 UPROPERTY() int32 Voice=0;
 UPROPERTY() bool Following=false;
 UPROPERTY() int32 Bedroom=0;
 UPROPERTY() FName Station;
 UPROPERTY() FName HomeVehicle66;
 UPROPERTY() int32 HomeBunk66=-1;
};
USTRUCT()
struct FLWRPGState {
 GENERATED_BODY()
 UPROPERTY() TMap<FName,FLWClaim82> Claims82;
 UPROPERTY() FLWCanadaState68 Canada68;
 UPROPERTY() TMap<FName,FName> VoiceProfiles59;
 UPROPERTY() TMap<FName,int32> WeaponKills62;
UPROPERTY() TMap<FName,int32> WeaponHeads62;
UPROPERTY() TMap<FName,int32> WeaponElites62;
UPROPERTY() TMap<FName,int32> WeaponStuns62;
 UPROPERTY() FLWStoryState Story;
 UPROPERTY() FLWCampaign76State Campaign76;
 UPROPERTY() TSet<int32> TradingCards36;
 UPROPERTY() int32 Level=1;
 UPROPERTY() int64 XP=0;
 UPROPERTY() int32 Points=0;
 UPROPERTY() TArray<int32> Attributes={1,1,1,1,1,1,1};
 UPROPERTY() TMap<FName,int32> Perks;
 UPROPERTY() TArray<FLWQuestState> Quests;
 UPROPERTY() TArray<FLWCrewRecord> Crew;
 UPROPERTY() TSet<FName> Discoveries;
 UPROPERTY() TMap<int64,FVector> KnownPlaces;
 UPROPERTY() TMap<int64,FString> PlaceNames;
 UPROPERTY() TSet<FName> Persuaded;
 UPROPERTY() FName TrackedQuest;
 UPROPERTY() TMap<FName,FLWSettlementRecord> Settlements;
 UPROPERTY() FLWRespawnPoint Respawn;
};
UCLASS(BlueprintType)
class LETHALWORLD_API ULWRPGCatalog:public UDataAsset {
 GENERATED_BODY()
public:
 ULWRPGCatalog();
 UPROPERTY(EditAnywhere,Category="Progression") TArray<FLWPerk> Perks;
 UPROPERTY(EditAnywhere,Category="Quests") TArray<FLWQuest> Quests;
 static const ULWRPGCatalog* Get();
};
namespace LWRPG {
 LETHALWORLD_API int64 Threshold(int32 Level);
 LETHALWORLD_API float Stat(const FLWRPGState& State,FName Effect);
 LETHALWORLD_API bool Buy(FLWRPGState& State,FName Perk);
 LETHALWORLD_API int32 AddXP(FLWRPGState& State,int64 Amount);
 LETHALWORLD_API const TCHAR* Category(int32 Index);
}
