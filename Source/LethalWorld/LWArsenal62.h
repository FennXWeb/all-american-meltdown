#pragma once
#include "CoreMinimal.h"
#include "LWInventory.h"
class ALWCharacter;
namespace LWArsenal62 {
struct FAttachment {FName Id,Mount;FString Name,Pros,Cons;float Recoil=1,Spread=1,Damage=1,Rate=1;};
const TArray<FAttachment>& Attachments();
const FAttachment* Find(FName Id);
FString MysticName(int W);int Theme(int W);
FString CamoName(int C,int W);FString Challenge(const ALWCharacter* P,FName Weapon,int C);
bool Unlocked(const ALWCharacter* P,FName Weapon,int C);
float Stat(const FLWItemInstance* G,int Index);
void Migrate(TArray<FLWItemInstance>& Items);
void Credit(ALWCharacter* P,FName Weapon,bool Head,bool Elite,bool Stun=false);
const TCHAR* BaseMesh(int W);
TArray<TPair<FName,FVector>> MovingMeshes(int W);
}
