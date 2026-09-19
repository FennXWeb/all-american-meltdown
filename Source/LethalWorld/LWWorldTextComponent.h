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
    void Add(ULWWorldTextComponent* Label);
    void Remove(ULWWorldTextComponent* Label);
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override;
    virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;
private:
    TArray<TWeakObjectPtr<ULWWorldTextComponent>> Labels;
    int32 Cursor = 0;
};
