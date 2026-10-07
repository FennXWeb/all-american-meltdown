#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Async/Future.h"
#include "LWAviationSky84.generated.h"
struct FLWFarTerrain84 {TArray<FVector> V,N;TArray<FVector2D> UV;TArray<FColor> C;TArray<int32> T;};
UCLASS()
class ALWAviationSky84:public AActor {
 GENERATED_BODY()
public:
 ALWAviationSky84();void Update(class ALWWorld* W,class ALWCharacter* P,float Dt);
 static void Ensure(class ALWWorld* W,class ALWCharacter* P,float Dt);
 UPROPERTY() TObjectPtr<class UProceduralMeshComponent> Horizon;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Clouds;
 TFuture<FLWFarTerrain84> Pending;FVector2D Center=FVector2D(1e12),Requested=FVector2D::ZeroVector;
};
