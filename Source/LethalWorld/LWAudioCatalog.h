#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Sound/SoundCue.h"
#include "UObject/SoftObjectPtr.h"
#include "LWAudioCatalog.generated.h"

class UAudioComponent;
class USoundAttenuation;
class USoundBase;

/** Internal runtime-only cue. It has no editor graph, even in an editor -game process. */
UCLASS(Transient, NotBlueprintable)
class ULWAudioPlaybackCue : public USoundCue
{
    GENERATED_BODY()

public:
    virtual void PostInitProperties() override;
};

UENUM(BlueprintType)
enum class ELWAudioAttenuation : uint8
{
    Inherit UMETA(DisplayName="Caller default (honor Loud)"),
    Spatial UMETA(DisplayName="World / normal range"),
    Loud UMETA(DisplayName="World / weapon range"),
    None UMETA(DisplayName="2D / no attenuation"),
    Custom UMETA(DisplayName="Custom attenuation asset")
};

/** One editable event. Assign Source or multiple SoundWave, SoundCue or MetaSound assets in Tracks. */
USTRUCT(BlueprintType)
struct LETHALWORLD_API FLWAudioSlot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
    TSoftObjectPtr<USoundBase> Source;

    // A nonempty track list takes priority over Source for every event; Source remains the fallback.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio", meta=(ToolTip="Available for every event: SFX, voices, ambience, vehicles and music. Each playback randomly selects one distinct, loadable asset; repeats are allowed. Source is used if none can load."))
    TArray<TSoftObjectPtr<USoundBase>> Tracks;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio", meta=(ClampMin="0.0", ClampMax="4.0"))
    float Volume = 1.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio", meta=(ClampMin="0.125", ClampMax="4.0"))
    float Pitch = 1.f;

    /** Geometry obstruction, vehicle cabin filtering and measured room reverberation.
     * Music/UI are always exempt. Disable for an authored effect with its own processing. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
    bool bEnvironmentalProcessing = true;

    // PlaySlot applies this to SoundWaves without changing the shared source asset.
    // SoundCue/MetaSound sources must implement their own matching loop behavior.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio", meta=(DisplayName="Loop"))
    bool bLoop = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio", meta=(DisplayName="Music", ToolTip="Music plays in 2D, including while paused. Retain the returned component to stop or crossfade it."))
    bool bMusic = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio")
    ELWAudioAttenuation Attenuation = ELWAudioAttenuation::Inherit;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio", meta=(EditCondition="Attenuation == ELWAudioAttenuation::Custom", EditConditionHides))
    TSoftObjectPtr<USoundAttenuation> CustomAttenuation;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio", meta=(MultiLine=true))
    FString Description;
};

/** Keep this in a UPROPERTY if retaining it beyond the current game-thread call. */
USTRUCT(BlueprintType)
struct LETHALWORLD_API FLWResolvedAudioSlot
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category="Audio")
    TObjectPtr<USoundBase> Sound = nullptr;

    UPROPERTY(BlueprintReadOnly, Category="Audio")
    FLWAudioSlot Settings;

    UPROPERTY(BlueprintReadOnly, Category="Audio")
    bool bUsedFallback = false;
};

/** Editable event registry at /Game/Audio/DA_AudioCatalog. No editor dependencies. */
UCLASS(BlueprintType, meta=(DisplayName="Lethal World Audio Catalog"))
class LETHALWORLD_API ULWAudioCatalog : public UDataAsset
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Audio Catalog", meta=(ToolTip="Keep event keys stable. Expand a value and assign Source or add multiple Tracks for any sound event. Volume zero mutes. Source and legacy assets provide playlist fallbacks."))
    TMap<FName, FLWAudioSlot> Slots;

    static const TCHAR* GetCatalogObjectPath();
    static FString GetLegacyObjectPath(FName SlotName);
    static FLWAudioSlot GetDefaultSlot(FName SlotName);

    UFUNCTION(BlueprintPure, Category="Lethal World|Audio")
    static TArray<FName> GetDefaultSlotNames();

    /** Synchronous game-thread lookup; retain in a UPROPERTY to avoid reloads after GC. */
    UFUNCTION(BlueprintCallable, Category="Lethal World|Audio")
    static ULWAudioCatalog* GetDefaultCatalog();

    /** Resolves the source only. Playback should apply Settings, or use PlaySlot. */
    UFUNCTION(BlueprintCallable, Category="Lethal World|Audio")
    bool ResolveSlot(FName SlotName, FLWResolvedAudioSlot& OutSlot, bool bAllowFallback = true) const;

    /** Also works when the catalog itself is absent; falls back to /Game/Audio/S_<slot>. */
    UFUNCTION(BlueprintCallable, Category="Lethal World|Audio")
    static bool ResolveDefaultSlot(FName SlotName, FLWResolvedAudioSlot& OutSlot, bool bAllowFallback = true);

    /** Caller owns stopping loops/music; component auto-destroys on completion or Stop.
     * Pass the world's existing normal/loud attenuation assets to preserve occlusion.
     * bListenerRelative bypasses positional processing before playback for local weapon reports.
     * All calls and source loading must occur on the game thread.
     */
    UFUNCTION(BlueprintCallable, Category="Lethal World|Audio", meta=(WorldContext="WorldContextObject", AdvancedDisplay="VolumeMultiplier,PitchMultiplier,bLoud,DefaultAttenuation,LoudAttenuation,bListenerRelative"))
    static UAudioComponent* PlaySlot(const UObject* WorldContextObject, FName SlotName, FVector Location,
        float VolumeMultiplier = 1.f, float PitchMultiplier = 1.f, bool bLoud = false,
        USoundAttenuation* DefaultAttenuation = nullptr, USoundAttenuation* LoudAttenuation = nullptr, bool bListenerRelative = false);

    /** Non-destructive seeding: existing entries and artist choices are never replaced. */
    UFUNCTION(BlueprintCallable, Category="Audio Catalog")
    int32 AddMissingDefaultSlots();

    UFUNCTION(CallInEditor, Category="Audio Catalog", meta=(DisplayName="Add Missing Default Slots"))
    void AddMissingSlots();

    /** Returns the error count; also reports sources, fallback, key, range and loop issues. */
    UFUNCTION(BlueprintCallable, Category="Lethal World|Audio")
    int32 ValidateCatalog(TArray<FString>& OutErrors, TArray<FString>& OutWarnings) const;

#if WITH_EDITOR
    virtual EDataValidationResult IsDataValid(FDataValidationContext& Context) const override;
#endif
};
