# Build 0.5 — Ignition

Roadside and POI cars are now driveable sedans with original hollow bodywork and modeled interiors. Their identity, parked location, lock state, damage, ignition bypass and storage persist through saving and chunk streaming.

## Locks and secured loot

Carry lockpicks and press **E** at a locked container or car. **A/D or horizontal mouse movement** adjusts the pick; hold **Space or left mouse** to turn the cylinder. Release torque when the lock binds and try another angle. Binding wears the current pick; breaking it consumes one pick and changes the lock's solution. Canceling retains wear. There are four tiers: Novice, Trained, Expert and Master, with progressively narrower successful angles.

New hostile POIs mix locked and unlocked containers. Higher tiers increase the loot roll quality and add a tier reward: shotgun ammunition, medical kits, a combat rifle, or an LMG. Previously generated containers retain their contents and remain accessible. Start a new survivor or explore new territory to see the new distribution. The starting kit includes eight picks, and merchants and containers replenish them.

## Keys and ignition

Each car has a persistent 128-bit VIN. Its exterior prompt and dashboard show a shortened ID; inspecting a key shows its matching VIN. Carrying the matching key opens the car and authorizes ignition. A key stored in the glovebox does not count as carried. Some gloveboxes contain that car's key: transfer it into your inventory before starting. Keys can be carried, traded, stashed, dropped and recovered like other items.

Without the key, **R** opens the hotwiring panel. The car-specific service plate identifies the battery, ignition and starter pins. Select those wires in order using **1–6** or the onscreen buttons. Wrong connections heat the harness and reset the sequence. Once wired, apply **Space or left mouse** during the green timing window; release outside it. Overheating resets the connections. Success permanently bypasses that car's ignition. Locks and ignition are separate: picking the door does not start the engine.

## Cabin controls

| Action | Input |
|---|---|
| Enter / use dashboard control under crosshair | E |
| Start or stop engine | R |
| Accelerate / reverse / steer | W / S / A–D |
| Brake | Space |
| First-person free look | Mouse |
| Glovebox / search cargo while outside | G |
| Radio: channel 1 / channel 2 / off | T |
| Headlights | H |
| Wipers | C |
| Left / right turn signal | Z / X |
| Exit | F |

Look at the modeled ignition, radio, glovebox, light/wiper switches, indicator stalks or door handle and press **E** as an alternative to shortcuts. Stop before accessing storage or exiting. Close the inventory to inspect the open glovebox lid; use the glovebox control again to shut it. Combat is disabled in the driver seat. Saving in a car restores that seat with its engine off.

This build uses a terrain-following, swept-collision arcade driving model, with steering, acceleration, braking, reverse, wheel motion and impact damage. It does not yet implement a full Chaos drivetrain, fuel consumption, tire damage, repair, car-door animation or passenger seating for companions. One sedan model is shared across cars; identity, security and contents vary. The two radio channels use original synthetic placeholder music at different playback rates.

## Development

- Runtime: `LWVehicle`, `LWVehicleState`, `LWSecurity`, `LWSecurityHUD`.
- Editable audio: `/Game/Audio/DA_AudioCatalog`, including CarEngine, CarRadio, CarIgnition, CarImpact, CarSignal, PickBreak, LockOpen and WireSpark. These use the existing spatial/occlusion pipeline.
- Original source art: `ArtSource/ModelsV5/LethalWorld_V5.blend`; generated with `Tools/make_models_v5.py`.
- Original sound sources: `Tools/make_audio_v5.py`.
- Import: `Tools/build_v5_content.py`; included in `Tools/Build.ps1 -Content`.
- Integrated runtime suite: `-LWV5Smoke -LWAudioSmoke`, using a separate automation save slot.
