#pragma once
#include "CoreMinimal.h"
#include "LWInventory.h"
#include "LWStoryState.generated.h"

// Append-only chapter checkpoint data; stored inside the existing RPG save.
USTRUCT()
struct FLWStoryState {
 GENERATED_BODY()
 UPROPERTY() int32 LayoutVersion52=0;
 UPROPERTY() bool Enabled=false;
 UPROPERTY() int32 Stage=0;
 UPROPERTY() TSet<int32> Paid;
 UPROPERTY() TSet<FName> Flags;
 UPROPERTY() TArray<FLWItemInstance> Confiscated;
 UPROPERTY() FGuid ConfiscatedWeapon;
 UPROPERTY() bool GearHeld=false;
 UPROPERTY() bool SceneFinished=false;
 UPROPERTY() int32 BossWave=0;
};

namespace LWStory {
 constexpr int32 Complete=29;
 struct FMission {const TCHAR* Title;const TCHAR* Objective;int32 Site;const TCHAR* Action;int32 Enemies;};
 LETHALWORLD_API const TArray<FMission>& Missions();
 // Fixed authored reservations are independent of streaming and random POI density.
 inline FVector2D Site(int32 I){const FVector2D P[]={{18000,16000},{112000,-58000},{-78000,151000},{241000,85000},{385000,-143000},{174000,326000},{503000,248000},{684000,-42000},{867000,207000}};return P[FMath::Clamp(I,0,8)];}
 inline const TCHAR* SiteName(int32 I){const TCHAR* N[]={TEXT("Mile Nine Rest Stop"),TEXT("Mercy Crossing"),TEXT("St. Agnes Field Clinic"),TEXT("Cinder Tollhouse"),TEXT("Relay Six"),TEXT("Dry Creek Transfer Yard"),TEXT("The Kennels"),TEXT("Ash Crown Foundry"),TEXT("Fort Resolute")};return N[FMath::Clamp(I,0,8)];}
 inline bool Reserved(FVector2D P,float Pad=0){for(int I=0;I<9;I++){auto D=P-Site(I);if(FMath::Abs(D.X)<4800+Pad&&FMath::Abs(D.Y)<4800+Pad)return true;}return false;}
 inline uint32 EnemyId(int Stage,int Index){return 0xEF310000u+Stage*64+Index;}
}
