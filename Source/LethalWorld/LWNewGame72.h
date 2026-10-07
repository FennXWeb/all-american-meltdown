#pragma once
#include "CoreMinimal.h"
#include "LWRPG.h"
namespace LWNewGame72 {
FVector2D StartXY();
bool LegacySite(FVector Position);
// Legacy campaign fields remain serializable so confiscated possessions can be recovered.
void RetireStory(FLWRPGState& RPG,TArray<FLWItemInstance>& Inventory,TArray<FLWItemInstance>& Stash,TArray<FLWItemInstance>& Overflow);
}
