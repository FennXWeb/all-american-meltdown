#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWOpening.generated.h"
USTRUCT()
struct FLWIdentity {
 GENERATED_BODY()
 UPROPERTY() FString Name=TEXT("SURVIVOR");
 UPROPERTY() int32 Skin=2;
 UPROPERTY() int32 Hair=1;
 UPROPERTY() int32 HairColor=0;
 UPROPERTY() int32 Outfit=0;
 UPROPERTY() int32 Frame=1;
 UPROPERTY() int32 Face=0;
 UPROPERTY() int32 Body=0;
 UPROPERTY() float Height=.5f;
 UPROPERTY() float Weight=.5f;
 UPROPERTY() float Shoulders=.5f;
 UPROPERTY() int32 StyleVersion35=0;
 UPROPERTY() TArray<float> Shapes35;
 UPROPERTY() int32 Top35=0;
 UPROPERTY() int32 Bottom35=0;
 UPROPERTY() int32 Shoes35=0;
 UPROPERTY() int32 Headwear35=0;
 UPROPERTY() int32 Gloves35=0;
 UPROPERTY() int32 Beard35=0;
 UPROPERTY() int32 Eyewear35=0;
 UPROPERTY() int32 TopColor35=0;
 UPROPERTY() int32 BottomColor35=1;
 UPROPERTY() int32 ShoesColor35=0;
 UPROPERTY() int32 BeardColor35=0;
 UPROPERTY() int32 EyeColor35=0;
 UPROPERTY() TArray<int32> Tattoos35;
 UPROPERTY() TArray<float> TattooSize35;
 UPROPERTY() float TattooOpacity35=.85f;
 UPROPERTY() TArray<FVector2D> TattooPosition35;
 UPROPERTY() TArray<float> TattooRotation35;

};
namespace LWOpening {
 struct FBeat {const TCHAR* Title;const TCHAR* Cue;const TCHAR* Lines[3];};
 LETHALWORLD_API const TArray<FBeat>& Beats();
 LETHALWORLD_API FLinearColor Skin(int I);
 LETHALWORLD_API FLinearColor Cloth(int I);
 LETHALWORLD_API bool ValidAllocation(const TArray<int32>& A);
}
UCLASS()
class LETHALWORLD_API ALWOpeningScene:public AActor {
 GENERATED_BODY()
 public:
 ALWOpeningScene();
 UPROPERTY() TObjectPtr<class UCameraComponent> Camera;
 UPROPERTY() TObjectPtr<class ALWWorld> World;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Pieces;
 UPROPERTY() TArray<TObjectPtr<class UStaticMeshComponent>> Animated;
 UPROPERTY() TArray<TObjectPtr<class UPointLightComponent>> Lamps;
 UPROPERTY() TObjectPtr<class UAudioComponent> Audio;
 FVector From,To,Focus;
 int32 Shot=-1;
 void Clear();void Stage(int32 Index);void Pose(float Progress);void Portrait(const FLWIdentity& Identity,float Yaw);
 class UStaticMeshComponent* Piece(FName Mesh,FName Material,FVector At,FVector Size,FRotator Rotation=FRotator::ZeroRotator);
 void Light(FVector At,FLinearColor Color,float Power,float Radius);
 virtual void EndPlay(const EEndPlayReason::Type Reason)override;
};
