#pragma once
#include "LWInteractable.h"
#include "LWGeneration.h"
#include "LWTradingCards36.generated.h"
struct FLWRPGState;
class UTexture2D;class ALWChunk;class ALWWorld;
namespace LWCollect36 {
 struct FCard {int Index;const TCHAR* Name;const TCHAR* Series;int Rarity;const TCHAR* Effect;float Value;const TCHAR* Lore;};
 const TArray<FCard>& Deck();
 float Bonus(const FLWRPGState& State,FName Effect);
 bool Collect(FLWRPGState& State,int Index);
 int Select(uint32 Seed,int Quality=0);
 const TCHAR* RarityName(int Rarity);
 FLinearColor RarityColor(int Rarity);
 FString Benefit(const FCard& Card);
 class UTexture2D* Artwork(int Index);
 void Spawn(class ALWChunk* Chunk,class ALWWorld* World,const LWGen::FSite& Site);
}
UCLASS()
class ALWTradingCard36:public ALWInteractable {
 GENERATED_BODY()
public:
 ALWTradingCard36();
 UPROPERTY() int32 CardIndex=0;
 UPROPERTY() TObjectPtr<class UStaticMeshComponent> Picture;
 bool ArtReady=false;
 void Setup(int Index);
 virtual FString Prompt()const override;
 virtual void Use(class ALWCharacter* Player)override;
 virtual void Tick(float Dt)override;
};
