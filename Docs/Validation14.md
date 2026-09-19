# Build 0.14 validation

Validated September 6, 2026 in Unreal Engine 5.8.1. No packaging, cooking, staging, or BuildCookRun was performed.

- Editor C++ target: succeeded (`Saved/Build14g.stdout.log`).
- Asset generation: 15 Blender-authored meshes; final import validated mesh bounds and material assignments (`Saved/Models14Final.log`, `Saved/Content14Final.log`). Original source scene: `ArtSource/ModelsV14/LethalWorld_V14.blend`.
- Automation: 44 tests passed, including rarity/attachment ownership, atomic detach, transfer/sort preservation, compatibility rules, spawn defaults and vehicle seats (`Saved/Automation14.log`).
- Final new gameplay suite: 266 checks, zero failures (`Saved/Regression14Release.log`, `LW_V14_DONE`). Covers installed visual components, optic view, saved payloads, camper fixtures and separate cargo, parked cabin walking and bed trace, crew driving/stop, all seats across 11 vehicle types, weighted interior spawns, killed-ID persistence, actual waypoint arrival and impassable barricades.
- Existing compatibility suite: 2,209 checks, zero failures (`Saved/Regression14Compatibility.log`, `LW_V9_DONE`). Covers vehicle interiors, POI furniture/storage, weather/wipers, sleeping/sitting and prior world behavior.
- Screenshots inspected: camper exterior, parked living area, scoped view. Final screenshots are retained in `Docs/ScreenshotsV14`.

The final roof and HUD polish was followed by both rendered gameplay suites. Automation was run earlier against the same equipment implementation; the full multi-version `Tools/Verify.ps1` sequence was not rerun. Smoke tests use isolated automation save slots.

## Practical boundaries

Companion driving uses the generated road graph, lookahead steering, obstacle sweeps, passing clearance checks, and bounded reverse recovery. It does not guarantee traversal of every random blockage or inaccessible off-road waypoint. When recovery cannot proceed, it stops and dismisses the driver. Gameplay validation includes a clear route and a fully blocked route; it is not an exhaustive test of every map seed or traffic arrangement.

RV cabin walking and residential interactions require parking. Moving vehicles require a seat. The sink currently uses the existing hydration mechanic; this build does not add a fresh-water tank simulation. Mounts and compatibility are defined in `Source/LethalWorld/LWWeaponMods.h`; enemy population tuning is exposed through `/Game/Data/DA_EnemySpawns`.
