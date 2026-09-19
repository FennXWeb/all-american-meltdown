#pragma once
#include "LWInventory.h"

class ALWCharacter;
class ALWWorldObject;
namespace LWFuel {
 constexpr int32 CanCapacity=20; // Rounds stores whole litres; cans never stack.
 LETHALWORLD_API FLWItemDefinition GasCanDefinition();
 LETHALWORLD_API FLWItemInstance MakeGasCan(int32 Litres=CanCapacity);
 // Returns litres added. Caller owns feedback and saving, allowing pump ammo refill too.
 LETHALWORLD_API int32 RefillGasCans(ALWCharacter* Player,ALWWorldObject* Pump);
}
