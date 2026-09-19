# Audio catalog integration handoff

This change owns only `Source/LethalWorld/LWAudioCatalog.h/.cpp`, this editor
module directory, and `Tools/make_audio_v2.py` / `Tools/build_audio_v2.py`.
Unreal, UHT, compilation, gameplay integration and content import were not run.
The scripts have been tested outside Unreal; main should perform the integration
and engine checks below. No existing game source, configuration or source audio
was changed by this work.

## Register the module (main)

Add this entry alongside the existing runtime module in `LethalWorld.uproject`:

```json
{ "Name": "LethalWorldEditor", "Type": "Editor", "LoadingPhase": "Default" }
```

Add `ExtraModuleNames.Add("LethalWorldEditor");` to
`Source/LethalWorldEditor.Target.cs`. Do not add it to the game target. The new
`LethalWorldEditor.Build.cs` already declares its dependencies and includes the
existing runtime module's root header directory. The runtime catalog requires
only the existing Core/CoreUObject/Engine dependencies; no runtime dependency on
UnrealEd, ToolMenus, SoundCueEditor or this editor module is needed.

Keep the existing PythonScriptPlugin and EditorScriptingUtilities editor plugins
enabled. Keep the existing `/Game/Audio` DirectoriesToAlwaysCook entry. The
catalog contains ordinary cooked soft references to replacement audio assets.

## Generate and import (main)

1. Run `python Tools/make_audio_v2.py` using Python with numpy. It writes **30 new
   WAVs** to `ArtSource/Audio`, preserving any files already there. The original
   16 WAVs are outside the generator's name list. Use `--overwrite` only when
   intentionally regenerating the 30 placeholders.
2. Compile the editor target with the new native files and module registration;
   restart Unreal so Python sees `unreal.LWAudioCatalog`.
3. Run `Tools/build_audio_v2.py` in Unreal's Python environment. For example:

   ```powershell
   & 'G:\Epic\UE_5.8\Engine\Binaries\Win64\UnrealEditor-Cmd.exe' `
       'X:\LethalWorld\LethalWorld.uproject' -run=pythonscript `
       '-script=X:\LethalWorld\Tools\build_audio_v2.py' -unattended -nop4 -nullrhi
   ```

The importer imports missing sounds and creates/seeds
`/Game/Audio/DA_AudioCatalog`. Its default rerun preserves existing sound assets,
catalog source assignments, mix settings and custom slots; only missing standard
slots are added. It preflights file presence/types before mutations, refuses
dirty target assets, checks each import/save result, and reports validation
errors. It does not save unrelated packages. A failure partway through imports
can leave earlier individual imports saved; rerunning resumes missing work.

For a deliberate refresh of the generated 30 SoundWaves, run the function
`build(reimport_generated=True)` after importing the script as a module in Unreal
Python (or pass `--reimport-generated` as a script argument). It preserves catalog
assignments and the original 16 assets. Existing SoundCue/MetaSound assets at
default paths are never replaced by the refresh.

Do not run the old `build_content.py` audio import after artists start replacing
audio: that existing script unconditionally replaces audio at default paths.
Run the old full content build first when one is needed, then this importer.

## Gameplay integration (main)

Include `LWAudioCatalog.h` and replace the implementation of `ALWWorld::Sound`
with the following call. It preserves the existing function signature, including
per-event volume, pitch, loudness and the world's occlusion settings:

```cpp
return ULWAudioCatalog::PlaySlot(this, N, P, Volume, Pitch, Loud,
    Attenuation, LoudAttenuation);
```

`PlaySlot` reads `/Game/Audio/DA_AudioCatalog.DA_AudioCatalog`. If the catalog,
entry or source is missing/unresolved, it tries `/Game/Audio/S_<Name>.S_<Name>`.
Unknown safe names also receive this fallback. A missing sound returns `nullptr`;
volume zero mutes a slot. An empty Source requests fallback, rather than muting.
Volume/pitch from the slot multiply the caller's values. Inputs are checked for
nonfinite values; slot volume clamps to 0–4 and final pitch to 0.125–4.

Keep a `UPROPERTY() TObjectPtr<ULWAudioCatalog> AudioCatalog` on the world and
assign `ULWAudioCatalog::GetDefaultCatalog()` in BeginPlay to retain the loaded
catalog. The helper's cache is weak so it never permanently roots an editor
asset. Lookup/loading is synchronous and must run on the game thread. Optional
preloading for the existing `Sounds` map and smoke checks:

```cpp
for (FName Name : ULWAudioCatalog::GetDefaultSlotNames())
{
    FLWResolvedAudioSlot Resolved;
    if (ULWAudioCatalog::ResolveDefaultSlot(Name, Resolved))
        Sounds.Add(Name, Resolved.Sound);
}
```

`ResolveSlot` / `ResolveDefaultSlot` resolve the raw source plus `Settings` and
`bUsedFallback`. Set `bAllowFallback=false` when testing assignments strictly.
If playing these raw sources directly, the caller must apply the settings.
`PlaySlot` does that automatically, so existing direct Wind/Drone startup calls
should also be routed through it, retaining their existing .7 / .32 caller gains.

Attenuation choices are caller default (honors Loud), normal world range, weapon
range, 2D, and a custom SoundAttenuation asset. Pass the world's existing normal
and loud assets for its spatial choices. Music always uses 2D playback, which can
play while paused. `Music` is a classification/2D flag, not a separate mix bus.

For SoundWaves, `PlaySlot` applies Loop with a transient WavePlayer cue only when
the source's loop setting differs; it never changes the shared source asset.
SoundCue/MetaSound sources retain their authored graph loop behavior and
validation warns about mismatches. The returned UAudioComponent keeps transient
playback objects alive. Store components for Wind/Drone/music and stop or fade
out old tracks on mode changes and world teardown. Components auto-destroy on
completion/Stop. There is no implicit music switching or concurrent-track limit.

## Authoring

Tools → **Lethal World Audio Manager** opens the catalog's standard asset editor,
creating it and adding missing slots if necessary. Expand **Slots**, find a key,
then drag a SoundWave (or SoundCue/MetaSound Source) from the Content Browser onto
its **Source** field. Change volume, pitch, loop, music, attenuation and the
description there, then Save. Reopening the menu never replaces edited entries.
Keep event keys stable; the details view has an **Add Missing Default Slots**
button to repair accidental key deletion without resetting other edits.

Tools → **Validate Lethal World Audio** reports errors/warnings in a dialog and
the Output Log. Standard asset validation also uses `IsDataValid`. Checks cover
missing/unresolved sounds and fallbacks, required keys, invalid name characters,
finite volume/pitch ranges, custom attenuation, music placement and graph loop
mismatches. `validate_catalog()` in Python returns `(error_count, errors,
warnings)`; an integer return deliberately preserves diagnostics on failure.

## Slots and verification

Existing 16: Shotgun, Pump, Reload, Swing, MetalHit, FleshHit, StepRoad, StepEarth,
StepIndoor, Zombie, Hurt, Click, Credit, Wind, Drone, Generator.

New 30: RevolverFire/Open/Eject/Insert/Close; SniperFire/Bolt/MagOut/MagIn;
SMGFire/MagOut/MagIn/Charge; RifleFire/MagOut/MagIn/Charge;
LMGFire/Cover/Belt/Close; LootPickup, InventoryMove, TraderVoice, Trade,
BunkerDoor, DeathDrop; MenuMusic, ExploreMusic, CombatMusic. Each weapon fire is
one shot, suitable for event-driven automatic fire. The three stereo music loops
are respectively 24, 30 and 16 seconds; other sounds are mono. Everything is
original deterministic numpy synthesis at 32 kHz, PCM16, with headroom.

Completed outside Unreal:

- Python syntax and native/generator slot contract checks (16 + 30 = 46).
- All 30 signals synthesized; no duplicate PCM content, nonfinite samples,
  clipping or failed loop-boundary checks.
- WAV write/read round trip, deterministic resynthesis, stereo music, preserving
  existing files by default and explicit overwrite behavior.
- Offline Unreal API stub checks for initial import, idempotence, legacy and
  artist-edit preservation, missing slot repair, reimport scope, dirty assets,
  wrong-type collisions and validation diagnostics.
- Local UE 5.8 header/source inspection of the native and Python APIs used.

Still required in Unreal by main: UHT/editor and game builds; import/save/reload
the catalog; verify drag/drop and menu behavior; audition the placeholders and
mix; exercise fallback with/without a catalog; loop a replacement non-looping
SoundWave; test music transitions/Stop and packaged cooking of replacement
sources. No engine or compile success is claimed by the offline checks.
