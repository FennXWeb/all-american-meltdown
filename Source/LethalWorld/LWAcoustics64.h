#pragma once
#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "Sound/SoundAttenuation.h"
#include "LWAcoustics64.generated.h"

class UAudioComponent;
class UReverbEffect;
class ALWCharacter;
struct FHitResult;

namespace LWAcoustics64
{
    struct FTransmission { float Gain=1, Cutoff=20000; };
    // Coverage is the fraction of obstructed paths; thickness is measured in cm.
    FTransmission Transmission(float Coverage, float Thickness, float Cabin, bool InteriorSource);
    int RoomPreset(bool Roof, int Walls, float CubicMetres, bool Cabin);
}

/** Listener acoustics for procedural geometry. No volumes need to be hand placed. */
UCLASS()
class LETHALWORLD_API ULWAcoustics64 : public UTickableWorldSubsystem
{
    GENERATED_BODY()
public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual void Tick(float DeltaTime) override;
    virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(ULWAcoustics64, STATGROUP_Tickables); }
    virtual bool DoesSupportWorldType(EWorldType::Type Type) const override;
    void Track(UAudioComponent* Audio, FName Slot, const FSoundAttenuationSettings& Base, bool LocalWeapon=false);
    int GetRoomPreset() const { return CurrentRoom; }
    int GetTrackedCount() const { return Sources.Num(); }
    // Shared by runtime and geometry regression tests. Ignores the listener and emitter.
    LWAcoustics64::FTransmission Probe(FVector Listener, FVector Source, AActor* ListenerActor, AActor* SourceActor, float Cabin, bool InteriorSource);
private:
    struct FSource
    {
        TWeakObjectPtr<UAudioComponent> Audio;
        FSoundAttenuationSettings Base;
        FName Slot;
        bool Local=false;
        float Cutoff=20000;
    };
    TArray<FSource> Sources;
    UPROPERTY(Transient) TArray<TObjectPtr<UReverbEffect>> Rooms;
    FVector Ear=FVector::ZeroVector;
    TWeakObjectPtr<ALWCharacter> Listener;
    float SourceClock=0, RoomClock=0;
    int Cursor=0, CurrentRoom=-1, CandidateRoom=-1, StableRoom=0;
    uint64 BudgetFrame=0;
    int TraceBudget=48;
    void RefreshListener();
    void MeasureRoom();
    void Process(FSource& Source, bool Initial);
    bool Trace(FHitResult& Hit, FVector Start, FVector End, AActor* Ignore, AActor* Other=nullptr);
};
