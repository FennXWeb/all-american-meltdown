#pragma once
#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LWSpawnTable.generated.h"
USTRUCT(BlueprintType)
struct FLWSpawnRow {
 GENERATED_BODY()
 UPROPERTY(EditAnywhere) int32 POIType=0;
 UPROPERTY(EditAnywhere) int32 MinCount=1;
 UPROPERTY(EditAnywhere) int32 MaxCount=4;
 UPROPERTY(EditAnywhere) float ZombieWeight=70;
 UPROPERTY(EditAnywhere) float RaiderWeight=20;
 UPROPERTY(EditAnywhere) float DogWeight=10;
 UPROPERTY(EditAnywhere) float MooseWeight=0;
 UPROPERTY(EditAnywhere) float TitanWeight=4;
 UPROPERTY(EditAnywhere) float DeathclawWeight=2;
 UPROPERTY(EditAnywhere) float ScorpionWeight=2;
 UPROPERTY(EditAnywhere) float KarenWeight=4;
 UPROPERTY(EditAnywhere,meta=(ClampMin=0,ClampMax=1)) float IndoorChance=.7f;
};
UCLASS(BlueprintType)
class LETHALWORLD_API ULWSpawnTable:public UDataAsset {
 GENERATED_BODY()
 public:
 ULWSpawnTable();
 UPROPERTY(EditAnywhere) TArray<FLWSpawnRow> POIs;
 UPROPERTY(EditAnywhere) float WildernessChance=.16f;
 UPROPERTY(EditAnywhere) int32 MaxAlive=100;
};
