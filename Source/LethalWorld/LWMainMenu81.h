#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWOpening.h"
#include "LWMainMenu81.generated.h"
class ULWSaveGame;
struct FLWRPGState;
namespace LWMenu81 {
 FString Mission(const FLWRPGState& State);
 FString Location(const ULWSaveGame& Save);
 struct FRelease { FString Version,Date,Title;TArray<FString> Notes; };
 const TArray<FRelease>& Releases();
}

UCLASS()
class ALWMenuPortrait81 : public AActor {
 GENERATED_BODY()
public:
 ALWMenuPortrait81();
 void Build(const FLWIdentity& Identity);
 void Render(double Time,bool ReducedMotion);
 UPROPERTY() TObjectPtr<class USceneComponent> PoseRoot;
 UPROPERTY() TObjectPtr<class USceneCaptureComponent2D> Capture;
 UPROPERTY() TObjectPtr<class UTextureRenderTarget2D> Target;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Parts;
 double NextFrame=0;int32 CaptureCount=0;
};

// One asynchronous read when entering the title screen. Never applies a save or
// starts world streaming just to display the survivor's identity.
UCLASS()
class ULWMainMenu81 : public UObject {
 GENERATED_BODY()
public:
 void Refresh(const FString& ExactSlot=FString());
 void Accept(ULWSaveGame* Save);
 void EnsurePortrait(UWorld* World);
 void StopPortrait();
 UPROPERTY() FLWIdentity Identity;
 UPROPERTY() TObjectPtr<ALWMenuPortrait81> Portrait;
 FString Slot,Place,Mission,Error;
 int64 Credits=0,CanadianDollars=0;
 int32 Level=1,Day=1;
 bool Loading=false,Ready=false,Requested=false;
 uint32 Revision=0;
};
