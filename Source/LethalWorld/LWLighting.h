#pragma once
#include "CoreMinimal.h"
namespace LWLighting {
void Load();void Cycle();int Mode();FString Label();bool HardwareAvailable();void Apply(int Requested,bool Save=false);
}
