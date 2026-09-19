"""Run with Unreal's Python plugin AFTER building the updated native modules.

    UnrealEditor-Cmd.exe LethalWorld.uproject -run=pythonscript \
        -script="<project>/Tools/build_audio_v2.py" -unattended -nop4 -nullrhi

First run Tools/make_audio_v2.py with normal Python + numpy. This script itself
requires no numpy. Imports missing /Game/Audio/S_<slot> assets and creates/seeds
/Game/Audio/DA_AudioCatalog. The original 16 assets and all existing catalog
entries (including custom slots, replacements and mix settings) are preserved.

For an intentional waveform refresh, call build(reimport_generated=True) in the
editor Python console after importing this module, or use --reimport-generated
as a script argument. This affects only the 30 v2 default asset paths, never the
16 legacy assets or the catalog's source assignments. Modified unsaved assets
are refused, so this cannot accidentally save/reimport an artist's open edits.
"""

from __future__ import annotations

from pathlib import Path
import sys
import wave

import unreal as u

CATALOG_PATH = "/Game/Audio/DA_AudioCatalog"
LEGACY_SLOTS = frozenset((
    "Shotgun", "Pump", "Reload", "Swing", "MetalHit", "FleshHit", "StepRoad", "StepEarth",
    "StepIndoor", "Zombie", "Hurt", "Click", "Credit", "Wind", "Drone", "Generator",
))
LOOP_SLOTS = frozenset(("Wind", "Drone", "Generator", "MenuMusic", "ExploreMusic", "CombatMusic"))


def preflight_wave(path: Path) -> None:
    if not path.is_file():
        raise RuntimeError(f"Missing {path}. Run Tools/make_audio_v2.py first; legacy WAVs come from make_surfaces.py.")
    with wave.open(str(path), "rb") as source:
        if source.getnchannels() not in (1, 2) or source.getsampwidth() != 2 or source.getnframes() == 0:
            raise RuntimeError(f"Expected nonempty mono/stereo PCM16 WAV: {path}")


def ensure_saved(asset, lib) -> None:
    if not lib.save_loaded_asset(asset, only_if_is_dirty=False):
        raise RuntimeError(f"Could not save {asset.get_path_name()}")


def build(reimport_generated: bool = False):
    catalog_class = getattr(u, "LWAudioCatalog", None)
    if catalog_class is None:
        raise RuntimeError("LWAudioCatalog is not registered. Build LethalWorld and restart Unreal before running build_audio_v2.py.")
    root = Path(u.Paths.project_dir()).resolve()
    lib = u.EditorAssetLibrary
    asset_tools = u.AssetToolsHelpers.get_asset_tools()
    names = [str(name) for name in catalog_class.get_default_slot_names()]
    if len(names) < 46 or len(set(names)) != len(names) or not LEGACY_SLOTS.issubset(names):
        raise RuntimeError("Native catalog must provide the original 16 plus 30 v2 slots; review the importer when changing that contract.")

    catalog = lib.load_asset(CATALOG_PATH) if lib.does_asset_exist(CATALOG_PATH) else None
    if catalog is not None and not isinstance(catalog, catalog_class):
        raise RuntimeError(f"{CATALOG_PATH} already exists but is not an LWAudioCatalog; nothing was overwritten.")
    # This API reports dirty packages without loading or touching unrelated assets.
    dirty_packages = {package.get_name() for package in u.EditorLoadingAndSavingUtils.get_dirty_content_packages()}
    if CATALOG_PATH in dirty_packages:
        raise RuntimeError("Audio catalog has unsaved edits. Save or revert it before running the importer.")

    planned = []
    preserved = 0
    for name in names:
        destination = f"/Game/Audio/S_{name}"
        exists = lib.does_asset_exist(destination)
        if exists:
            sound = lib.load_asset(destination)
            if not isinstance(sound, u.SoundBase):
                raise RuntimeError(f"{destination} exists but is not a SoundBase; nothing at that path will be overwritten.")
            if not reimport_generated or name in LEGACY_SLOTS:
                preserved += 1
                continue
            if not isinstance(sound, u.SoundWave):
                raise RuntimeError(f"{destination} is an authored SoundCue/MetaSound, not a generated wave; refusing replacement.")
        if destination in dirty_packages:
            raise RuntimeError(f"{destination} has unsaved edits. Save or revert it before reimporting.")
        source = root / "ArtSource" / "Audio" / f"S_{name}.wav"
        preflight_wave(source)
        planned.append((name, source, destination, exists))

    # Complete all preflight checks before creating or importing anything.
    lib.make_directory("/Game/Audio")
    for name, source, destination, existed in planned:
        task = u.AssetImportTask()
        task.set_editor_property("filename", str(source))
        task.set_editor_property("destination_path", "/Game/Audio")
        task.set_editor_property("destination_name", f"S_{name}")
        task.set_editor_property("automated", True)
        task.set_editor_property("replace_existing", existed)
        task.set_editor_property("replace_existing_settings", False)
        task.set_editor_property("save", False)
        asset_tools.import_asset_tasks([task])
        if not task.get_editor_property("imported_object_paths"):
            raise RuntimeError(f"Unreal did not report a successful audio import: {source}")
        sound = lib.load_asset(destination)
        if not isinstance(sound, u.SoundWave):
            raise RuntimeError(f"Import did not create a SoundWave: {source}")
        if not existed:
            sound.set_editor_property("looping", name in LOOP_SLOTS)
        ensure_saved(sound, lib)
        u.log(f"LW_AUDIO_IMPORTED {name}")

    if catalog is None:
        factory = u.DataAssetFactory()
        factory.set_editor_property("data_asset_class", catalog_class)
        catalog = asset_tools.create_asset("DA_AudioCatalog", "/Game/Audio", catalog_class, factory)
        if catalog is None:
            raise RuntimeError("Failed to create audio catalog.")
        created = True
    else:
        created = False
    added = catalog.add_missing_default_slots()
    # Use an integer return in C++: Unreal Python hides bool returns and discards
    # out-parameters on false, which would lose the useful validation diagnostics.
    error_count, errors, warnings = catalog.validate_catalog()
    for message in warnings:
        u.log_warning(f"LW_AUDIO: {message}")
    for message in errors:
        u.log_error(f"LW_AUDIO: {message}")
    if error_count:
        raise RuntimeError(f"Audio catalog validation failed with {len(errors)} errors; catalog changes remain unsaved for review.")
    if created or added:
        ensure_saved(catalog, lib)
    u.log(f"LW_AUDIO_V2_COMPLETE: {len(planned)} imported, {preserved} existing sounds preserved, "
          f"{added} slots added, {len(catalog.get_editor_property('slots'))} total slots, {len(warnings)} warnings.")
    return catalog


if __name__ == "__main__":
    build(reimport_generated="--reimport-generated" in sys.argv)
