#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LWInteractable.generated.h"
UCLASS()
class LETHALWORLD_API ALWInteractable : public AActor
{
    GENERATED_BODY()
public:
    ALWInteractable();
    UPROPERTY() TObjectPtr<class UStaticMeshComponent> Mesh;
    UPROPERTY() TObjectPtr<class UAudioComponent> Hum;
    virtual void EndPlay(const EEndPlayReason::Type Reason) override;
    float ReadyAt=0;
    void Configure(class ALWWorld* World);
    virtual FString Prompt() const;
    virtual void Use(class ALWCharacter* Player);
};
