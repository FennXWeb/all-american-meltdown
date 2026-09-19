#pragma once
#include "CoreMinimal.h"
#include "LWBunker45State.generated.h"
USTRUCT()
struct FLWPlaced45 {
 GENERATED_BODY()
 UPROPERTY() FName Model;
 UPROPERTY() FName Use;
 UPROPERTY() FTransform Transform=FTransform::Identity;
 UPROPERTY() int32 Cost=0;
 UPROPERTY() bool Removed=false;
};
USTRUCT()
struct FLWBunker45State {
 GENERATED_BODY()
 UPROPERTY() int32 BedroomFloors=0;
 UPROPERTY() bool Garage=false;
 UPROPERTY() TSet<FName> Utilities;
 UPROPERTY() TMap<FName,FLWPlaced45> Furniture;
 UPROPERTY() TMap<FName,FName> Assignments;
 UPROPERTY() double WorkSeconds=0;
 UPROPERTY() int32 WorkCycles=0;
 int32 Bedrooms()const{return 10+10*FMath::Clamp(BedroomFloors,0,9);}
};
