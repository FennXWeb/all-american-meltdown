#pragma once
#include "CoreMinimal.h"
#include "LWSettlement82State.generated.h"

USTRUCT()
struct FLWConstruction82 {
 GENERATED_BODY()
 UPROPERTY() FName Id;
 UPROPERTY() FName Catalog;
 UPROPERTY() FTransform Transform=FTransform::Identity;
 UPROPERTY() FName Support;
 UPROPERTY() int32 Outside=0;
 UPROPERTY() int32 Inside=1;
 UPROPERTY() int32 OutsideColor=0;
 UPROPERTY() int32 InsideColor=0;
 UPROPERTY() int32 Paid=0;
};
USTRUCT()
struct FLWSettler82 {
 GENERATED_BODY()
 UPROPERTY() FName Id;
 UPROPERTY() FString Name;
 UPROPERTY() int32 Voice=0;
 UPROPERTY() FName Station;
 UPROPERTY() FName Bed;
};
USTRUCT()
struct FLWClaim82 {
 GENERATED_BODY()
 UPROPERTY() FName Id;
 UPROPERTY() FString Name;
 UPROPERTY() FVector Center=FVector::ZeroVector;
 UPROPERTY() float Radius=6000;
 UPROPERTY() TArray<FLWConstruction82> Pieces;
 UPROPERTY() TArray<FLWSettler82> Residents;
 // A value-only, unlimited construction stockpile; never an inventory alias.
 UPROPERTY() int32 Scrap=0;
 UPROPERTY() int64 Treasury=0;
 UPROPERTY() bool Broadcast=true;
 UPROPERTY() double RecruitmentSeconds=0;
 UPROPERTY() double WorkSeconds=0;
 UPROPERTY() int32 WorkCycle=0;
 UPROPERTY() int32 RecruitSerial=0;
 UPROPERTY() TArray<FName> Contracts;
};
