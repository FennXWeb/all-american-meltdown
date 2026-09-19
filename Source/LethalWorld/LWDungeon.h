#pragma once
#include "CoreMinimal.h"
#include "LWWorldObject.h"
#include "LWGeneration.h"
#include "LWDungeon.generated.h"
struct FLWDungeonGuard {FVector Position;int Kind=0,Floor=0;uint32 Id=0;float SavedHealth=-1;bool Warden=false,LegendaryGuardian=false;TWeakObjectPtr<class ALWZombie> Actor;};
UCLASS()
class LETHALWORLD_API ALWDungeon : public AActor {
 GENERATED_BODY()
public:
 ALWDungeon();
 UPROPERTY() TObjectPtr<class ALWWorld> World;
 UPROPERTY() TObjectPtr<class ALWChunk> Chunk;
 LWGen::FSite Site;TArray<FLWDungeonGuard> Guards;bool WasInside=false;
 virtual void Tick(float Dt)override;
 virtual void EndPlay(const EEndPlayReason::Type Reason)override;
 void Setup(ALWWorld* W,ALWChunk* C,const LWGen::FSite& S);
 FName Key(const TCHAR* Suffix)const;
 FVector At(FVector Local)const;
 int Relays()const;int WardensLeft()const;bool Ready()const;
 bool Activate(int Index,class ALWCharacter* P);bool Claim(class ALWCharacter* P);
 void MakeReward();
};
UCLASS()
class LETHALWORLD_API ALWDungeonDevice : public ALWWorldObject {
 GENERATED_BODY()
public:
 UPROPERTY() TObjectPtr<ALWDungeon> Dungeon;
 int Index=0; // 0..2 circuit, 3 final chest, 4 entrance directory / story
 virtual FString Prompt()const override;
 virtual void Use(class ALWCharacter* P)override;
};
