#include "LWAudioCatalog.h"

#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundAttenuation.h"
#include "Sound/SoundBase.h"
#include "Sound/SoundCue.h"
#include "Sound/SoundNodeWavePlayer.h"
#include "Sound/SoundWave.h"
#include "UObject/Class.h"
#include "UObject/Package.h"
#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

namespace LWAudio
{
    struct FDefaultSlot
    {
        const TCHAR* Name;
        const TCHAR* Description;
        ELWAudioAttenuation Attenuation = ELWAudioAttenuation::Inherit;
        bool bLoop = false;
        bool bMusic = false;
    };

    // The editor and Python importer both seed from this single canonical list.
    static const FDefaultSlot Defaults[] =
    {
        {TEXT("Slide54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Spatial},
        {TEXT("Cloth54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Spatial},
        {TEXT("Climb54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Spatial},
        {TEXT("Land54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Spatial},
        {TEXT("TitanStep54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Loud},
        {TEXT("BehemothStep54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Loud},
        {TEXT("ColossusStep54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Loud},
        {TEXT("GiantSlam54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Loud},
        {TEXT("DeathclawStep54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Loud},
        {TEXT("DeathclawCharge54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Spatial},
        {TEXT("ScorpionStep54"),TEXT("Traversal or creature contact event; supports replacement tracks."),ELWAudioAttenuation::Loud},

        {TEXT("MissileFire24"),TEXT("Missile launch report; replace in the audio catalog.")},
        {TEXT("MinigunFire24"),TEXT("Rotary cannon report; replace in the audio catalog.")},
        {TEXT("SawedOffFire24"),TEXT("Short shotgun report; replace in the audio catalog.")},
        {TEXT("DeagleFire24"),TEXT(".50 AE pistol report; replace in the audio catalog.")},
        {TEXT("M4Fire24"),TEXT("M4 carbine report; replace in the audio catalog.")},
        {TEXT("TaserFire24"),TEXT("Electrical dart report; replace in the audio catalog.")},
        {TEXT("FlameFire24"),TEXT("Flame burst report; replace in the audio catalog.")},
        {TEXT("DoubleBarrelFire"),TEXT("Double barrel shotgun report.")},
        {TEXT("DoubleBarrelOpen"),TEXT("Break action latch and barrels opening.")},
        {TEXT("DoubleBarrelEject"),TEXT("Spent shell ejection.")},
        {TEXT("DoubleBarrelInsert"),TEXT("Shell insertion into breech.")},
        {TEXT("DoubleBarrelClose"),TEXT("Barrels closing and locking.")},
        {TEXT("MooseRoar"),TEXT("Mutated moose call.")},
        {TEXT("TitanRoar"),TEXT("Titan growl.")},
        {TEXT("DeathclawRoar"),TEXT("Deathclaw roar.")},
        {TEXT("ScorpionHiss"),TEXT("Scorpion hiss.")},
        {TEXT("KarenShriek"),TEXT("Scooter zombie shriek.")},
        {TEXT("LevelUp"),TEXT("Level advancement chime."),ELWAudioAttenuation::None},
        {TEXT("PoliceSiren"),TEXT("Emergency vehicle siren."),ELWAudioAttenuation::Loud,true},
        {TEXT("PunchSwing"),TEXT("Unarmed punch air movement.")},
        {TEXT("PunchHit"),TEXT("Unarmed contact impact.")},
        {TEXT("CarDoor"),TEXT("Motorhome door hinge and latch.")},
        {TEXT("CarGearShift"),TEXT("Transmission gear engagement.")},
        {TEXT("CarEngineStop"),TEXT("Engine shutdown tail.")},
        {TEXT("CarAirBrake"),TEXT("Heavy vehicle air-brake release.")},
        {TEXT("CarLoadDiesel"),TEXT("Low diesel engine under load; seamless loop."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarLoadPetrol"),TEXT("Petrol engine under load; seamless loop."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarLoadSport"),TEXT("Performance engine under load; seamless loop."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarLoadBike"),TEXT("Motorcycle engine under load; seamless loop."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarOverrun"),TEXT("Engine braking and exhaust overrun; seamless loop."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarTires"),TEXT("Road and tire rolling noise; seamless loop."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarBrake"),TEXT("Braking friction proportional to deceleration; seamless loop."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarSkid"),TEXT("Hard braking and cornering tire skid; seamless loop."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarEngine"),TEXT("Vehicle idle and driving loop with RPM pitch."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarRadio"),TEXT("Cabin civil-band radio loop."),ELWAudioAttenuation::Spatial,true},
        {TEXT("CarIgnition"),TEXT("Starter motor and ignition catch.")},
        {TEXT("CarImpact"),TEXT("Vehicle chassis collision impact.")},
        {TEXT("CarSignal"),TEXT("Turn signal relay click.")},
        {TEXT("PickBreak"),TEXT("Lockpick snapping under excess torque.")},
        {TEXT("LockOpen"),TEXT("Lock cylinder opens successfully.")},
        {TEXT("WireSpark"),TEXT("Hotwire short circuit and electrical arc.")},
        {TEXT("Speech0"),TEXT("Low-register fictional speech syllables.")},
        {TEXT("Speech1"),TEXT("Mid-register fictional speech syllables.")},
        {TEXT("Speech2"),TEXT("High-register fictional speech syllables.")},
        {TEXT("DoorHinge"),TEXT("POI door hinge and latch.")},
        {TEXT("GlassBreak"),TEXT("Window fracture and falling shards.")},
        {TEXT("FuelExplosion"),TEXT("Volatile fuel pump explosion."),ELWAudioAttenuation::Loud},
        {TEXT("DogGrowl"),TEXT("Rabid dog growl and bark.")},
        {TEXT("RaiderVoice"),TEXT("Masked raider radio vocalization.")},
        {TEXT("MannequinMove"),TEXT("Dry mannequin joint scrape.")},
        {TEXT("Shotgun"), TEXT("Original shotgun blast."), ELWAudioAttenuation::Loud},
        {TEXT("Pump"), TEXT("Original shotgun pump action.")},
        {TEXT("Reload"), TEXT("Original shotgun shell insertion.")},
        {TEXT("Swing"), TEXT("Original melee swing.")},
        {TEXT("MetalHit"), TEXT("Original metal impact.")},
        {TEXT("FleshHit"), TEXT("Original flesh impact.")},
        {TEXT("StepRoad"), TEXT("Original footstep on road.")},
        {TEXT("StepEarth"), TEXT("Original footstep on earth.")},
        {TEXT("StepIndoor"), TEXT("Original indoor footstep.")},
        {TEXT("Zombie"), TEXT("Original zombie vocalization.")},
        {TEXT("Hurt"), TEXT("Original player hurt reaction.")},
        {TEXT("Click"), TEXT("Original interface / empty weapon click.")},
        {TEXT("Credit"), TEXT("Original credit reward.")},
        {TEXT("Rain"), TEXT("Weather rain loop."), ELWAudioAttenuation::None, true},
        {TEXT("Thunder"), TEXT("Weather thunder."), ELWAudioAttenuation::None},
        {TEXT("Wind"), TEXT("Original looping wind bed."), ELWAudioAttenuation::None, true},
        {TEXT("Drone"), TEXT("Original looping ambient drone."), ELWAudioAttenuation::None, true},
        {TEXT("Generator"), TEXT("Original positional generator loop."), ELWAudioAttenuation::Spatial, true},
        {TEXT("RevolverFire"), TEXT("Heavy handgun crack, short bass tail."), ELWAudioAttenuation::Loud},
        {TEXT("RevolverOpen"), TEXT("Cylinder latch release and swing-out.")},
        {TEXT("RevolverEject"), TEXT("Extractor push and multiple falling brass cases.")},
        {TEXT("RevolverInsert"), TEXT("Single brass round seated in a cylinder.")},
        {TEXT("RevolverClose"), TEXT("Cylinder swung shut and locked.")},
        {TEXT("SniperFire"), TEXT("Long rifle report with a distant rolling tail."), ELWAudioAttenuation::Loud},
        {TEXT("SniperBolt"), TEXT("Bolt lift, long travel and steel lock.")},
        {TEXT("SniperMagOut"), TEXT("Precision rifle magazine release and withdrawal.")},
        {TEXT("SniperMagIn"), TEXT("Precision rifle magazine seating and catch.")},
        {TEXT("SMGFire"), TEXT("Short, bright automatic weapon pop; one shot per trigger event."), ELWAudioAttenuation::Loud},
        {TEXT("SMGMagOut"), TEXT("Light magazine catch and quick extraction.")},
        {TEXT("SMGMagIn"), TEXT("Light magazine insertion and slap.")},
        {TEXT("SMGCharge"), TEXT("Compact charging handle pull and spring return.")},
        {TEXT("RifleFire"), TEXT("Dry rifle crack with a midrange punch; single shot."), ELWAudioAttenuation::Loud},
        {TEXT("RifleMagOut"), TEXT("Polymer magazine release and textured slide.")},
        {TEXT("RifleMagIn"), TEXT("Polymer magazine seat and palm tap.")},
        {TEXT("RifleCharge"), TEXT("Rifle charging handle and heavy bolt release.")},
        {TEXT("LMGFire"), TEXT("Heavy automatic report with receiver chatter; single shot."), ELWAudioAttenuation::Loud},
        {TEXT("LMGCover"), TEXT("Feed-cover latch and hinge opening.")},
        {TEXT("LMGBelt"), TEXT("Belt links and brass settling on the feed tray.")},
        {TEXT("LMGClose"), TEXT("Feed cover slam and locking latch.")},
        {TEXT("LootPickup"), TEXT("Cloth movement, object pickup and small confirmation chime."), ELWAudioAttenuation::None},
        {TEXT("InventoryMove"), TEXT("Soft inventory tile tick and cloth movement."), ELWAudioAttenuation::None},
        {TEXT("TraderVoice"), TEXT("Original synthetic, nonverbal two-syllable radio greeting.")},
        {TEXT("Trade"), TEXT("Mechanical transaction click and ascending confirmation tones."), ELWAudioAttenuation::None},
        {TEXT("BunkerDoor"), TEXT("Heavy latch, motor, hinge scrape and final metal thud."), ELWAudioAttenuation::Spatial},
        {TEXT("DeathDrop"), TEXT("Equipment bundle falls with a dull impact and scattered objects."), ELWAudioAttenuation::Spatial},
        {TEXT("MenuMusic"), TEXT("Original slow minor-key title theme; seamless stereo loop."), ELWAudioAttenuation::None, true, true},
        {TEXT("ExploreMusic"), TEXT("Original sparse exploration motif; seamless stereo loop."), ELWAudioAttenuation::None, true, true},
        {TEXT("CombatMusic"), TEXT("Original urgent percussion and bass ostinato; seamless stereo loop."), ELWAudioAttenuation::None, true, true}
    };

    bool IsSafeSlotName(FName Name)
    {
        if (Name.IsNone()) return false;
        for (TCHAR Character : Name.ToString())
        {
            if (!((Character >= 'A' && Character <= 'Z') || (Character >= 'a' && Character <= 'z') ||
                  (Character >= '0' && Character <= '9') || Character == '_')) return false;
        }
        return true;
    }

    template <typename T>
    T* LoadQuietly(const FString& Path)
    {
        return Path.IsEmpty() ? nullptr : LoadObject<T>(nullptr, *Path, nullptr, LOAD_NoWarn | LOAD_Quiet);
    }

    void Sanitize(FLWAudioSlot& Settings)
    {
        Settings.Volume = FMath::IsFinite(Settings.Volume) ? FMath::Clamp(Settings.Volume, 0.f, 4.f) : 1.f;
        Settings.Pitch = FMath::IsFinite(Settings.Pitch) ? FMath::Clamp(Settings.Pitch, .125f, 4.f) : 1.f;
    }

    bool Resolve(FName Name, const FLWAudioSlot* Entry, FLWResolvedAudioSlot& Out, bool bAllowFallback)
    {
        Out = FLWResolvedAudioSlot();
        if (!IsSafeSlotName(Name)) return false;
        Out.Settings = Entry ? *Entry : ULWAudioCatalog::GetDefaultSlot(Name);
        Sanitize(Out.Settings);
        if (Entry && !Entry->Tracks.IsEmpty())
        {
            TArray<FSoftObjectPath> Candidates;
            for (const auto& Track : Entry->Tracks) if (!Track.IsNull()) Candidates.AddUnique(Track.ToSoftObjectPath());
            // Shuffle first, then load until one succeeds; no need to load an entire playlist.
            while (!Candidates.IsEmpty())
            {
                const int32 Index = FMath::RandRange(0, Candidates.Num()-1);
                Out.Sound = LoadQuietly<USoundBase>(Candidates[Index].ToString());
                if (Out.Sound) return true;
                Candidates.RemoveAtSwap(Index);
            }
        }
        if (Entry && !Entry->Source.IsNull())
            Out.Sound = LoadQuietly<USoundBase>(Entry->Source.ToSoftObjectPath().ToString());
        if (!Out.Sound && bAllowFallback)
        {
            Out.Sound = LoadQuietly<USoundBase>(ULWAudioCatalog::GetLegacyObjectPath(Name));
            Out.bUsedFallback = Out.Sound != nullptr;
        }
        return Out.Sound != nullptr;
    }

    USoundBase* PlaybackSound(const FLWResolvedAudioSlot& Resolved)
    {
        USoundWave* Wave = Cast<USoundWave>(Resolved.Sound.Get());
        if (!Wave || (!Resolved.Settings.bLoop && !Wave->IsLooping())) return Resolved.Sound.Get();

        // A per-play WavePlayer overrides looping without modifying/reimporting the artist's asset.
        // UAudioComponent holds the cue, which in turn keeps its player and wave alive.
        USoundCue* Cue = NewObject<ULWAudioPlaybackCue>(GetTransientPackage(), NAME_None, RF_Transient);
        USoundNodeWavePlayer* Player = NewObject<USoundNodeWavePlayer>(Cue, NAME_None, RF_Transient);
        Player->SetSoundWave(Wave);
        Player->bLooping = Resolved.Settings.bLoop;
        Cue->FirstNode = Player;
        Cue->VolumeMultiplier = 1.f; // Engine's default cue gain is 0.75.
        Cue->PitchMultiplier = 1.f;
        Cue->SoundClassObject = Wave->SoundClassObject;
        Cue->AttenuationSettings = Wave->AttenuationSettings;
        // Keep ambience alive across mute and distance culling so it can become audible again.
        Cue->VirtualizationMode = Resolved.Settings.bLoop ? EVirtualizationMode::PlayWhenSilent : Wave->VirtualizationMode;
        Cue->ConcurrencySet = Wave->ConcurrencySet;
        Cue->bOverrideConcurrency = Wave->bOverrideConcurrency;
        Cue->ConcurrencyOverrides = Wave->ConcurrencyOverrides;
        Cue->Duration = Player->GetDuration(); // CacheAggregateValues only updates duration in editor.
        Cue->CacheAggregateValues();
        return Cue;
    }
}

void ULWAudioPlaybackCue::PostInitProperties()
{
    // USoundCue's editor hook creates a graph through the SoundCueEditor singleton,
    // which may be absent in commandlets / editor -game. Playback needs only FirstNode.
    USoundBase::PostInitProperties();
}

const TCHAR* ULWAudioCatalog::GetCatalogObjectPath()
{
    return TEXT("/Game/Audio/DA_AudioCatalog.DA_AudioCatalog");
}

FString ULWAudioCatalog::GetLegacyObjectPath(FName SlotName)
{
    if(SlotName==TEXT("Slide54"))SlotName=TEXT("StepEarth");
    if(SlotName==TEXT("Cloth54"))SlotName=TEXT("InventoryMove");
    if(SlotName==TEXT("Climb54"))SlotName=TEXT("StepIndoor");
    if(SlotName==TEXT("Land54"))SlotName=TEXT("StepEarth");
    if(SlotName==TEXT("TitanStep54"))SlotName=TEXT("CarImpact");
    if(SlotName==TEXT("BehemothStep54"))SlotName=TEXT("CarImpact");
    if(SlotName==TEXT("ColossusStep54"))SlotName=TEXT("CarImpact");
    if(SlotName==TEXT("GiantSlam54"))SlotName=TEXT("CarImpact");
    if(SlotName==TEXT("DeathclawStep54"))SlotName=TEXT("StepRoad");
    if(SlotName==TEXT("DeathclawCharge54"))SlotName=TEXT("DeathclawRoar");
    if(SlotName==TEXT("ScorpionStep54"))SlotName=TEXT("MetalHit");
    return LWAudio::IsSafeSlotName(SlotName)
        ? FString::Printf(TEXT("/Game/Audio/S_%s.S_%s"), *SlotName.ToString(), *SlotName.ToString()) : FString();
}

FLWAudioSlot ULWAudioCatalog::GetDefaultSlot(FName SlotName)
{
    FLWAudioSlot Result;
    if(SlotName==TEXT("Slide54")){Result.Volume=0.75f;Result.Pitch=0.7f;}
    if(SlotName==TEXT("Cloth54")){Result.Volume=0.6f;Result.Pitch=0.85f;}
    if(SlotName==TEXT("Climb54")){Result.Volume=0.8f;Result.Pitch=0.75f;}
    if(SlotName==TEXT("Land54")){Result.Volume=1.0f;Result.Pitch=0.7f;}
    if(SlotName==TEXT("TitanStep54")){Result.Volume=0.8f;Result.Pitch=0.7f;}
    if(SlotName==TEXT("BehemothStep54")){Result.Volume=1.0f;Result.Pitch=0.5f;}
    if(SlotName==TEXT("ColossusStep54")){Result.Volume=1.2f;Result.Pitch=0.35f;}
    if(SlotName==TEXT("GiantSlam54")){Result.Volume=1.4f;Result.Pitch=0.45f;}
    if(SlotName==TEXT("DeathclawStep54")){Result.Volume=0.9f;Result.Pitch=0.7f;}
    if(SlotName==TEXT("DeathclawCharge54")){Result.Volume=1.0f;Result.Pitch=0.9f;}
    if(SlotName==TEXT("ScorpionStep54")){Result.Volume=0.3f;Result.Pitch=1.4f;}
    Result.Source = TSoftObjectPtr<USoundBase>(FSoftObjectPath(GetLegacyObjectPath(SlotName)));
    for (const LWAudio::FDefaultSlot& Entry : LWAudio::Defaults)
    {
        if (SlotName == FName(Entry.Name))
        {
            Result.Attenuation = Entry.Attenuation;
            Result.bLoop = Entry.bLoop;
            Result.bMusic = Entry.bMusic;
            Result.Description = Entry.Description;
            if (Entry.bMusic) Result.Volume = .45f;
            break;
        }
    }
    return Result;
}

TArray<FName> ULWAudioCatalog::GetDefaultSlotNames()
{
    TArray<FName> Names;
    for (const LWAudio::FDefaultSlot& Entry : LWAudio::Defaults) Names.Add(FName(Entry.Name));
    return Names;
}

ULWAudioCatalog* ULWAudioCatalog::GetDefaultCatalog()
{
    static TWeakObjectPtr<ULWAudioCatalog> Cached;
    if (!Cached.IsValid()) Cached = LWAudio::LoadQuietly<ULWAudioCatalog>(GetCatalogObjectPath());
    return Cached.Get();
}

bool ULWAudioCatalog::ResolveSlot(FName SlotName, FLWResolvedAudioSlot& OutSlot, bool bAllowFallback) const
{
    return LWAudio::Resolve(SlotName, Slots.Find(SlotName), OutSlot, bAllowFallback);
}

bool ULWAudioCatalog::ResolveDefaultSlot(FName SlotName, FLWResolvedAudioSlot& OutSlot, bool bAllowFallback)
{
    if (const ULWAudioCatalog* Catalog = GetDefaultCatalog()) return Catalog->ResolveSlot(SlotName, OutSlot, bAllowFallback);
    return LWAudio::Resolve(SlotName, nullptr, OutSlot, bAllowFallback);
}

UAudioComponent* ULWAudioCatalog::PlaySlot(const UObject* WorldContextObject, FName SlotName, FVector Location,
    float VolumeMultiplier, float PitchMultiplier, bool bLoud, USoundAttenuation* DefaultAttenuation, USoundAttenuation* LoudAttenuation, bool bListenerRelative)
{
    FLWResolvedAudioSlot Resolved;
    if (!WorldContextObject || !ResolveDefaultSlot(SlotName, Resolved)) return nullptr;
    const FLWAudioSlot& Settings = Resolved.Settings;
    const float Volume = Settings.Volume * (FMath::IsFinite(VolumeMultiplier) ? FMath::Max(0.f, VolumeMultiplier) : 1.f);
    const float Pitch = FMath::Clamp(Settings.Pitch * (FMath::IsFinite(PitchMultiplier) ? PitchMultiplier : 1.f), .125f, 4.f);
    if (Volume <= 0.f) return nullptr;
    USoundBase* Sound = LWAudio::PlaybackSound(Resolved);
    if (bListenerRelative || Settings.bMusic || Settings.Attenuation == ELWAudioAttenuation::None)
        return UGameplayStatics::SpawnSound2D(WorldContextObject, Sound, Volume, Pitch);

    USoundAttenuation* Attenuation = DefaultAttenuation;
    if (Settings.Attenuation == ELWAudioAttenuation::Loud || (Settings.Attenuation == ELWAudioAttenuation::Inherit && bLoud))
        Attenuation = LoudAttenuation ? LoudAttenuation : DefaultAttenuation;
    else if (Settings.Attenuation == ELWAudioAttenuation::Custom)
    {
        if (USoundAttenuation* Custom = Settings.CustomAttenuation.LoadSynchronous()) Attenuation = Custom;
    }
    return UGameplayStatics::SpawnSoundAtLocation(WorldContextObject, Sound, Location, FRotator::ZeroRotator, Volume, Pitch, 0.f, Attenuation);
}

int32 ULWAudioCatalog::AddMissingDefaultSlots()
{
    int32 Added = 0;
    for (FName Name : GetDefaultSlotNames())
    {
        if (!Slots.Contains(Name))
        {
#if WITH_EDITOR
            if (Added == 0) Modify();
#endif
            Slots.Add(Name, GetDefaultSlot(Name));
            ++Added;
        }
    }
    return Added;
}

void ULWAudioCatalog::AddMissingSlots()
{
    AddMissingDefaultSlots();
}

int32 ULWAudioCatalog::ValidateCatalog(TArray<FString>& OutErrors, TArray<FString>& OutWarnings) const
{
    OutErrors.Reset();
    OutWarnings.Reset();
    for (FName Name : GetDefaultSlotNames())
    {
        if (!Slots.Contains(Name))
        {
            OutWarnings.Add(FString::Printf(TEXT("%s: required event is missing; use Add Missing Default Slots. Runtime will try its legacy asset."), *Name.ToString()));
            if (!LWAudio::LoadQuietly<USoundBase>(GetLegacyObjectPath(Name)))
                OutErrors.Add(FString::Printf(TEXT("%s: neither a catalog entry nor a legacy asset exists."), *Name.ToString()));
        }
    }
    for (const TPair<FName, FLWAudioSlot>& Pair : Slots)
    {
        const FString Prefix = Pair.Key.ToString() + TEXT(": ");
        const FLWAudioSlot& Slot = Pair.Value;
        if (!LWAudio::IsSafeSlotName(Pair.Key)) OutErrors.Add(Prefix + TEXT("key must use letters, digits or underscores and cannot be None."));
        if (!FMath::IsFinite(Slot.Volume) || Slot.Volume < 0.f || Slot.Volume > 4.f)
            OutErrors.Add(Prefix + TEXT("volume must be finite and between 0 and 4."));
        if (!FMath::IsFinite(Slot.Pitch) || Slot.Pitch < .125f || Slot.Pitch > 4.f)
            OutErrors.Add(Prefix + TEXT("pitch must be finite and between 0.125 and 4."));
        if (!StaticEnum<ELWAudioAttenuation>()->IsValidEnumValue(static_cast<int64>(Slot.Attenuation)))
            OutErrors.Add(Prefix + TEXT("unknown attenuation choice."));
        if (Slot.Attenuation == ELWAudioAttenuation::Custom &&
            !LWAudio::LoadQuietly<USoundAttenuation>(Slot.CustomAttenuation.ToSoftObjectPath().ToString()))
            OutErrors.Add(Prefix + TEXT("custom attenuation asset is missing or not a SoundAttenuation."));
        if (Slot.bMusic && Slot.Attenuation != ELWAudioAttenuation::None)
            OutWarnings.Add(Prefix + TEXT("music always plays in 2D; choose no attenuation to match."));
        if (Slot.Description.TrimStartAndEnd().IsEmpty()) OutWarnings.Add(Prefix + TEXT("add a description for the authoring team."));
        USoundBase* Sound = LWAudio::LoadQuietly<USoundBase>(Slot.Source.ToSoftObjectPath().ToString());
        TSet<FSoftObjectPath> TrackPaths;
        for (int32 I=0; I<Slot.Tracks.Num(); ++I)
        {
            const auto Path = Slot.Tracks[I].ToSoftObjectPath();
            USoundBase* Track = LWAudio::LoadQuietly<USoundBase>(Path.ToString());
            if (!Track) OutWarnings.Add(Prefix + FString::Printf(TEXT("Tracks[%d] is empty or missing and will be skipped."), I));
            else
            {
                if (!Sound) Sound=Track;
                if (!Cast<USoundWave>(Track) && Track->IsLooping()!=Slot.bLoop)
                    OutWarnings.Add(Prefix + FString::Printf(TEXT("Tracks[%d]: author this cue/MetaSound's looping to match Loop."), I));
            }
            if (TrackPaths.Contains(Path)) OutWarnings.Add(Prefix + TEXT("Duplicate playlist entry; it receives no additional selection weight."));
            TrackPaths.Add(Path);
        }
        if (!Sound)
        {
            Sound = LWAudio::LoadQuietly<USoundBase>(GetLegacyObjectPath(Pair.Key));
            if (Sound) OutWarnings.Add(Prefix + TEXT("source is empty or unresolved; runtime is using the legacy fallback."));
            else OutErrors.Add(Prefix + TEXT("no playable source or legacy fallback; import or assign a SoundBase asset."));
        }
        if (Sound && !Cast<USoundWave>(Sound) && Sound->IsLooping() != Slot.bLoop)
            OutWarnings.Add(Prefix + TEXT("source loop behavior differs from Loop; author the SoundCue/MetaSound graph to match. PlaySlot can override SoundWave loops only."));
    }
    return OutErrors.Num();
}

#if WITH_EDITOR
EDataValidationResult ULWAudioCatalog::IsDataValid(FDataValidationContext& Context) const
{
    const EDataValidationResult ParentResult = Super::IsDataValid(Context);
    TArray<FString> Errors, Warnings;
    const bool bValid = ValidateCatalog(Errors, Warnings) == 0;
    for (const FString& Error : Errors) Context.AddError(FText::FromString(Error));
    for (const FString& Warning : Warnings) Context.AddWarning(FText::FromString(Warning));
    return bValid && ParentResult != EDataValidationResult::Invalid ? EDataValidationResult::Valid : EDataValidationResult::Invalid;
}
#endif
