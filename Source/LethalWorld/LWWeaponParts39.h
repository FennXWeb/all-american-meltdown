#pragma once
#include "LWInventory.h"
namespace LWParts39 {
struct FDefinition {
 FName Id;FString Name;FName Group;FName Mesh;FName Weapon;
 FVector Size=FVector(10);FVector Muzzle=FVector::ZeroVector;
 float Damage=0,Handling=0,Speed=0;int32 Price=40;
};
LETHALWORLD_API const TArray<FDefinition>& Definitions();
LETHALWORLD_API const FDefinition* Find(FName Id);
LETHALWORLD_API TArray<FLWItemDefinition> Items();
LETHALWORLD_API FName WeaponId(int32 Index);
LETHALWORLD_API FLWAssemblyPart39 Part(const FLWItemInstance& I);
LETHALWORLD_API FLWItemInstance Item(const FLWAssemblyPart39& P);
LETHALWORLD_API TArray<FLWAssemblyPart39> Recipe(const FLWItemInstance& G);
LETHALWORLD_API bool Validate(const FLWItemInstance& G,FString& Error);
LETHALWORLD_API void Derive(FLWItemInstance& G);
LETHALWORLD_API bool Dismantle(TArray<FLWItemInstance>& Inventory,FGuid Gun,FString& Error);
LETHALWORLD_API bool Store(TArray<FLWItemInstance>& Inventory,FLWItemInstance Gun,FString& Error);
LETHALWORLD_API bool ReleaseMagazine(TArray<FLWItemInstance>& Inventory,FLWItemInstance& Gun,FString& Error);
LETHALWORLD_API uint32 Fingerprint(const TArray<FLWItemInstance>& Items);
LETHALWORLD_API TArray<FTransform> Barrels(const FLWItemInstance& G);
LETHALWORLD_API bool Has(const FLWItemInstance* G,FName Id);
LETHALWORLD_API float Bonus(const FLWItemInstance* G,int32 Stat);
LETHALWORLD_API FName Mesh(FName Definition);
}
