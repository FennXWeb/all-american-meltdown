# Update 80 — menu input, saves and coach repairs

## Changes

- Canvas menus use temporary mouse capture, forwarding the first press while keeping the cursor visible and unlocked. Gameplay retains permanent capture. UI attack suppression remains in place.
- Save confirmations exclusively own mouse hitboxes and keyboard focus. Saving opens in vehicles, reports unavailable states, and queues a manual destination behind an active autosave or checkpoint write.
- The fire-mode action switches to wiper control while occupying a vehicle. This covers the default V binding, remapped keys and controller LB + X. It remains blocked in menus.
- Diesel RV instruments are mounted ahead of a new dashboard fascia. The lower cockpit has a sealed firewall. Radio, lights, wipers, indicators and ignition controls are visible and targetable again.
- Both coaches have corrected glazing coverage, the missing driver-side pane and its own blind, entry lintel/jamb infill, and telescoping slide-out returns. Existing blind IDs, vehicle identities and storage inventories are preserved.
- Combustion engine layers are louder, with stronger rolling, braking, wind and wiper sounds. Occupants hear their own vehicle without rear-engine distance loss or stacked muffling. External sounds retain distance and occlusion processing. Electric vehicle layers retain their catalog gain and pitch when mixed.

## Assets and maintenance

- Build corrected meshes: `Tools/build_rv80.py` in Blender.
- Import: `Tools/import_rv80.py` in Unreal Editor Python.
- Eight meshes live under `/Game/Art/Meshes/SM_RV80_*`; original RV66 and electric assets are retained. New meshes reuse existing mapped materials and are included in streaming preloads. The radio, rocker switches, ignition and indicator stalks replace the old exposed control blocks.
- `LWUpdate80Smoke.inl` runs with `-game -LWV17Smoke -LWUI46Smoke -LWUpdate80Smoke`. It uses isolated automation save slots, real viewport mouse presses, save readback, wiper input, and rendered coach captures.

## Validation

- Unreal 5.8 Development Editor build succeeded (`Saved/BuildUpdate80Final.log`). Existing project deprecation warnings remain.
- Eight meshes imported with validated bounds/materials (`Saved/ImportRV80Final.log`).
- Rendered regression: **66 checks passed, zero failures**, covering first-press menu input, overwrite modal hit-testing, paused and queued manual saves with disk readback, vehicle saves, keyboard/rebound/controller wipers, live instruments, glazing coverage, slide seals, engine continuity, and cabin versus exterior attenuation (`Saved/Update80FinalSmoke.log`).
- Final cockpit, window/slide and save confirmation captures inspected under `Saved/ScreenshotsV17/Update80_*`.
- User settings restored after testing. No packaged build produced.
