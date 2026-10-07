#pragma once
#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"
#include "LWOpening.h"
#include "LWHair61.generated.h"

// Cosmetic spring response, bounded and evaluated only near the local camera.
UCLASS()
class LETHALWORLD_API ULWHair61 : public UProceduralMeshComponent {
 GENERATED_BODY()
public:
 ULWHair61(const FObjectInitializer& ObjectInitializer);
 void Initialize(class UStaticMeshComponent* Source,const FLWIdentity& Identity,bool Visible,bool Morph);
 virtual void TickComponent(float Dt,ELevelTick Type,FActorComponentTickFunction* Function) override;
 static bool Supports(const class UStaticMeshComponent* Source);
 bool ValidateDeformation61() const;
 FVector Displacement61=FVector::ZeroVector;
private:
 struct FSection61 {TArray<FVector> Rest,Position,Normals;TArray<FVector2D> UV;TArray<FProcMeshTangent> Tangents;TArray<float> Compliance;};
 TArray<FSection61> Sections61;
 FVector Velocity61=FVector::ZeroVector,PreviousPosition61=FVector::ZeroVector,PreviousVelocity61=FVector::ZeroVector;
 FQuat PreviousRotation61=FQuat::Identity;
 bool HasHistory61=false;
 float Clock61=0,Accumulated61=0;
};
