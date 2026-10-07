#pragma once
#include "CoreMinimal.h"
class ALWChunk;class ALWWorld;class ALWCharacter;
namespace LWBorder51 {
 constexpr double North=1280000.; // +X is map north; 12.8 km from the original bunker region.
 constexpr double Warning=North-250.;
 constexpr double Strip=North-12800.;
 inline bool Beyond(double X){return X>=North;}
 inline bool Restricted(double X){return X>Warning;}
 void Build(ALWChunk* C,ALWWorld* W);
 void Enforce(ALWWorld* W,ALWCharacter* P);
}
