#include "LWWorldTextComponent.h"
#include "Engine/World.h"
#include "LWSlotMachine.h"
#include "LWWorld.h"
#include "GameFramework/PlayerController.h"

ULWWorldTextComponent::ULWWorldTextComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
    SetMobility(EComponentMobility::Movable);
    SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ULWWorldTextComponent::OnRegister()
{
    Super::OnRegister();
    // Animated slot reels are appliance displays. POI labels never render as floating glyphs.
    if(GetOwner()&&!Cast<ALWSlotMachine>(GetOwner()))SetVisibility(false);
    if (GetWorld()) if (auto* Updates = GetWorld()->GetSubsystem<ULWWorldTextSubsystem>()) Updates->Add(this);
}

void ULWWorldTextComponent::OnUnregister()
{
    if (GetWorld()) if (auto* Updates = GetWorld()->GetSubsystem<ULWWorldTextSubsystem>()) Updates->Remove(this);
    Super::OnUnregister();
}

void ULWWorldTextComponent::UpdateFacing(const FVector& Eye)
{
    // Use cached bounds for the cheap distance gate. Only a nearby back-facing sign
    // needs glyph bounds recalculation. Respect a shorter authored cull distance.
    const float Range = CachedMaxDrawDistance > 0 ? FMath::Min(CachedMaxDrawDistance,12000.f) : 12000.f;
    if (!IsVisible() || FVector::DistSquared(Eye,Bounds.Origin) > FMath::Square(Range)) return;
    if (FVector::DotProduct(GetForwardVector(), Eye - GetComponentLocation()) >= -2.f) return;
    const FVector Center = CalcBounds(GetComponentTransform()).Origin;
    const FQuat Turn(GetUpVector(), PI);
    // Left/right alignment offsets the origin from the glyph center. Rotate the
    // origin around that center too, preserving the occupied world-space area.
    SetWorldLocationAndRotation(Center + Turn.RotateVector(GetComponentLocation()-Center), Turn*GetComponentQuat());
}

void ULWWorldTextSubsystem::Add(ULWWorldTextComponent* Label) { Labels.AddUnique(Label); }
void ULWWorldTextSubsystem::Remove(ULWWorldTextComponent* Label) { Labels.RemoveSwap(Label); }

bool ULWWorldTextSubsystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
    return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

TStatId ULWWorldTextSubsystem::GetStatId() const
{
    RETURN_QUICK_DECLARE_CYCLE_STAT(LWWorldTextUpdates, STATGROUP_Tickables);
}

void ULWWorldTextSubsystem::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    const auto* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
    if (!PC || !PC->IsLocalController()) return;
    FVector Eye;
    FRotator View;
    PC->GetPlayerViewPoint(Eye, View);
    const int32 Count = FMath::Min(MaxLabelsPerFrame,Labels.Num());
    int Built=0;
    for (int32 I=0; I<Count && !Labels.IsEmpty(); ++I)
    {
        Cursor %= Labels.Num();auto* Label=Labels[Cursor].Get();
        if(!Label){Labels.RemoveAtSwap(Cursor);continue;}
        if(Cast<ALWSlotMachine>(Label->GetOwner())){Label->UpdateFacing(Eye);++Cursor;continue;}
        if(auto* Chunk=Cast<ALWChunk>(Label->GetOwner());Chunk&&!Chunk->Ready68){++Cursor;continue;}
        // No runtime render-target burst when a city chunk is populated.
        if(Built>=1||FVector::DistSquared(Eye,Label->GetComponentLocation())>FMath::Square(16000.f)){++Cursor;continue;}
        Mount78(Label);++Built;Labels.RemoveAtSwap(Cursor);
    }
}
