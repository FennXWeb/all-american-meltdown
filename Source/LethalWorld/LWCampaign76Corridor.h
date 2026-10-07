#pragma once
#include "CoreMinimal.h"
namespace LWGen {struct FSite;struct FRoad;}
namespace LWCampaign76 {
 struct FStop {FName Id;FString Name;int32 Type;FVector2D Position;};
 const TArray<FStop>& Corridor();
 void AddCorridor(TArray<LWGen::FSite>& Sites,TArray<LWGen::FRoad>& Roads);
 void FinishApproach78(const TArray<LWGen::FSite>& Sites,TArray<LWGen::FRoad>& Roads);
}
