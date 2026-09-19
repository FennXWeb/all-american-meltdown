# All American Meltdown — 0.17

Editor/source update only; no packaging.

## Weapons

Laser sights draw a red emissive beam from the mount to the first hit, with a glowing contact dot. The beam and dot disappear while holstered, reloading, using menus or riding in a vehicle. Gun gloves retain their wrist pivots and reload paths, with sleeves extended back toward the elbows to remove the detached, blunt-ended arm appearance.

Left click punches while unarmed. Punches alternate hands, cost 8 stamina, have a 0.48-second cooldown, and hit once within 145 cm for 12 base damage multiplied by the existing combat bonus. Bunker safety remains enforced. Select an empty melee slot with 4 to put the gun away and use fists; inventory unequipping also works.

## Motorhome

The body is hollow above its 60.5 cm floor instead of containing a full-width solid skirt block. Seats and appliances now stand at floor level. A side door, frame and three steps provide entry: E opens/closes the door, then walk through it. Inside, E on a seat chooses that seat; Space stands in the cabin when parked. Walk back through the open doorway to leave. Existing seat shortcuts and the parked F exit remain available.

Fourteen exterior cargo hatch targets have distinct persistent inventories. The first hatch on the driver's side retains the former shared cargo inventory. Kitchen cupboards, the sink cabinet and under-bed storage are independent; fridge, pantry and wardrobe keep their existing separate records. E/G operates the object under the crosshair. The outside body no longer instantly puts the player into an arbitrary seat. Locked motorhomes retain key/lockpick access.

## Vehicles and audio

Unoccupied stationary vehicles check terrain under the chassis every half second. If embedded, they rise in bounded 15 cm increments; other blocking objects prevent recovery. Recovery never moves the player's occupied vehicle, a chauffeur-driven vehicle or one being boarded.

Driving mixes idle, engine load, overrun, rolling tires, brake friction and tire skid layers. Engine speed follows a smoothed RPM/gear model, throttle and road speed. Diesel, petrol, performance and motorcycle load sources differ. Gear shifts, shutdown and heavy air-brake release add discrete events. Brake friction scales with actual deceleration, while sharp braking and high-speed cornering add skid sound. Cabin listening applies a low-pass filter. Loops stop when the engine is off and the vehicle is stationary or destroyed.

All fourteen new slots are in the existing audio catalog, with deterministic original synthesized placeholder WAVs. Replace sources and edit mix settings in `/Game/Audio/DA_AudioCatalog`. The importer preserves existing assignments, custom tracks and sound replacements. This is a parameter-driven game audio approximation, not a mechanical drivetrain simulation or recorded real vehicle audio.

## Source assets and checks

- `Tools/make_models_v17.py` and `ArtSource/ModelsV17/AllAmericanMeltdown_V17.blend`: hollow shell, hinged door and extended hand/forearm models.
- `Tools/make_audio_v17.py`: original placeholder audio generator.
- `Tools/build_v17_content.py`: mesh/material/audio import.
- `Tools/Verify17.ps1`: automation plus rendered V17 and vehicle compatibility checks, with isolated save slots.

Final verification results are recorded in `Docs/Validation17.md` after execution.
