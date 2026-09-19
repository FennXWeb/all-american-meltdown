#pragma once
#include "CoreMinimal.h"
namespace LWChapter52 {
 FVector Objective(FName Action);
 FVector Recovery(int Stage);
 TArray<FVector> Posts(int Site,int Stage);
 bool Destructible(FName Action);
 FName Prop(FName Action);
}
