#pragma once
#include "CoreMinimal.h"
#include "Async/Future.h"
#include "LWMissionRecovery37.generated.h"

struct FLWCheckpointJob43 {
 FName Key;FString Title;int32 Stage=0,StartStage=0;
 TFuture<TArray<uint8>> Bytes;
};

// Payloads are normal save snapshots without this map, preventing recursive snapshots.
USTRUCT()
struct FLWMissionRecovery37 {
 GENERATED_BODY()
 UPROPERTY() FString Title;
 UPROPERTY() int32 StartStage=-1;
 UPROPERTY() int32 CheckpointStage=-1;
 UPROPERTY() TArray<uint8> Start;
 UPROPERTY() TArray<uint8> Checkpoint;
};
