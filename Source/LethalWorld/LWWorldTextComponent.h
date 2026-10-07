#pragma once

#include "Components/TextRenderComponent.h"
#include "Subsystems/WorldSubsystem.h"
#include "LWWorldTextComponent.generated.h"

// Keep a sign in its authored plane, but never show mirrored back-face glyphs.
UCLASS()
class LETHALWORLD_API ULWWorldTextComponent : public UTextRenderComponent
{
    GENERATED_BODY()
public:
    ULWWorldTextComponent();
    virtual void OnRegister() override;
    virtual void OnUnregister() override;
    void UpdateFacing(const FVector& Eye);
};

// A bounded round-robin pass replaces one tick function per generated label.
UCLASS()
class LETHALWORLD_API ULWWorldTextSubsystem : public UTickableWorldSubsystem
{
    GENERATED_BODY()
public:
    static constexpr int32 MaxLabelsPerFrame = 128;
    // Queued conversion: world labels become painted, physically mounted panels.
    bool Mount78(ULWWorldTextComponent* Label);
    UPROPERTY() TObjectPtr<class UStaticMesh> SignMesh78;
    UPROPERTY() TObjectPtr<class UMaterialInterface> SignBase78;
    UPROPERTY() TObjectPtr<class UTexture2D> SignAtlas78;
    UPROPERTY() TObjectPtr<class UFont> SignFont78;
    TMap<FString,TWeakObjectPtr<class UMaterialInstanceDynamic>> SignCache78;
    int32 Mounted78=0,Suppressed78=0;
    void Add(ULWWorldTextComponent* Label);
    void Remove(ULWWorldTextComponent* Label);
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
private:
    TArray<TWeakObjectPtr<ULWWorldTextComponent>> Labels;
    int32 Cursor = 0;
};
