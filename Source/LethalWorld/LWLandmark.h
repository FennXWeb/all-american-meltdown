#pragma once
#include "CoreMinimal.h"
#include "LWWorldObject.h"
#include "LWGeneration.h"
#include "LWLandmark.generated.h"
UCLASS()
class LETHALWORLD_API ALWLandmark : public AActor {
 GENERATED_BODY()
public:
 ALWLandmark();
 UPROPERTY() TObjectPtr<class ALWWorld> World;
 UPROPERTY() TObjectPtr<class ALWChunk> Chunk;
 LWGen::FSite Site;
 UPROPERTY() TArray<TObjectPtr<class ALWZombie>> Guards;
 virtual void Tick(float Dt)override;
 FName Key(const TCHAR* Suffix)const;
 int Stage()const;int GuardCount()const;uint32 GuardId(int I)const;
 bool Activate(int Index,int Choice,class ALWCharacter* P);
 void MakeReward();
};
UCLASS()
class LETHALWORLD_API ALWLandmarkDevice : public ALWWorldObject {
 GENERATED_BODY()
public:
 UPROPERTY() TObjectPtr<ALWLandmark> Landmark;
 int Index=0,Choice=0;
 virtual FString Prompt()const override;
 virtual void Use(class ALWCharacter* P)override;
};
