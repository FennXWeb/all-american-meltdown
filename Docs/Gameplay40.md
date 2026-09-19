# Gameplay revision 40

## Controls

- Hold **Q** to lean left; hold **C** to lean right. Releasing returns upright. Leaning moves the camera and weapon, with a small view roll and a swept sphere that limits wall penetration. Sprinting, falling, sitting and opening UI reset leaning.
- Press **H** on foot to use one inventory medkit. No kit is consumed at full health. Menus/dialogue block healing. Healing interrupts a reload using the existing cancellation rules and applies existing healing perks.
- In vehicles, **H** still controls headlights and **C** still controls wipers; **E** remains interact.

## Hitch fixes

Routine interaction, inventory, quest, weapon/reload and world-state updates request a batched autosave. Requests no longer immediately copy and serialize the entire world and write it to disk. A snapshot is taken after three quiet seconds, deferred during reload/attack/damage/combat, with a thirty-second dirty-age limit. Immutable serialized bytes are written on a worker thread. Explicit saves, death and quitting retain flush boundaries, and explicit save/load waits for any older writer to avoid overwriting newer progress.

The audio catalog's sources, tracks and attenuation assets are preloaded asynchronously and retained for the world lifetime, together with common blood/weapon effect assets. Custom catalog slots are included.

Blood spray uses one instanced component with lightweight ballistic droplets instead of eight new physics bodies per hit. A bounded weak list replaces the full-world actor scan used to count effects. Corpse ragdolls, severed limbs, loot and existing dismemberment rules remain active.

Vehicle residency and bunker-storage maintenance runs twice per second. Combat music scans four times per second and performs visibility checks for at most eight nearby engaged candidates, rather than tracing every enemy every frame.

## Music

Nearby, alive, hostile enemies must be engaged with the player, on the same floor and visible to start combat music. Recent actual combat with a living target can sustain it. A seven-second hold avoids switching songs every time an enemy briefly loses sight. Transitions fade out over 0.8 seconds and in over 1.1 seconds. The existing per-slot multi-track catalog remains the source of music selection.

## Validation

- Editor target: `Saved/Build40Final.log`, succeeded. No packaging.
- Existing automation: `Saved/Tests40.log`, 84 successful tests, exit code 0.
- Rendered isolated gameplay run: `Saved/Gameplay40Smoke.log`, 21 checks, zero failures.
- Covered medkit availability/consumption/UI blocking; both lean directions, wall clearance and UI reset; save batching/reload deferral/ordered save-load; music engagement/floor filtering/hold expiry; blood instances and enemy death.
- 1,000 save requests took 0.003 ms in that run and requested one subsequent snapshot. This measures request overhead, not overall frame time.

The game-thread snapshot/serialization, mission checkpoint capture and procedural chunk construction still have nonzero cost. These changes remove identified event-triggered hitch sources, but are not a guarantee of zero hitches across large saves, all locations or hardware. `LW_AutosaveSnapshot40` is an Unreal Insights CPU scope for further profiling.
