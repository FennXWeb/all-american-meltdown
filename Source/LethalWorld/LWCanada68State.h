#pragma once
#include "CoreMinimal.h"
#include "LWInventory.h"
#include "LWCanada68State.generated.h"

USTRUCT()
struct FLWCustomsItem68 {
 GENERATED_BODY()
 UPROPERTY() FLWItemInstance Item;
 // Empty source is the survivor inventory; other IDs are vehicle compartments.
 UPROPERTY() FName Source;
};
USTRUCT()
struct FLWCanadaState68 {
 GENERATED_BODY()
 UPROPERTY() int64 CanadianDollars=0;
 UPROPERTY() bool Passport=false;
 UPROPERTY() bool Cleared=false;
 UPROPERTY() bool Entered=false;
 UPROPERTY() TArray<FLWCustomsItem68> Escrow;
};
