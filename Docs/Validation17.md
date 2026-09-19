# Build 0.17 validation — 2026-09-07

No packaged build or shipping cook was created.

| Check | Result | Evidence |
|---|---|---|
| Unreal 5.8.1 Development Editor compile | Succeeded | `Saved/Build17Final.stdout.log` |
| Asset import | Four meshes validated; laser material created; 14 new sounds/slots imported | `Saved/Content17.log` |
| LethalWorld automation | 45 passed, zero failures | `Saved/Automation17.log` |
| V17 rendered gameplay | 36 passed, zero failures | `Saved/Regression17_V17.log` |
| Vehicle/equipment compatibility | 266 passed, zero failures | `Saved/Regression17_V14.log` |

The gameplay runs used offscreen 1280×720 rendering and isolated automation save slots. Existing player saves were not used. The audio importer preserved 67 existing sounds and all existing catalog assignments. Six pre-existing warnings concern intro music attenuation metadata; no new slot failed import.

V17 checks cover a visible beam clipped by a nearby wall, extended support-arm assets, empty-melee-slot unarmed selection, punch damage/stamina/cooldown, visible fists, and restoring grip transforms after re-equipping. Motorhome checks cover five floor-level seat targets, fourteen cargo hatches, tracing/opening the door, walking into and out of the cabin, tracing and selecting a specific seat, independent cabinet/hatch inventories and legacy cargo preservation, save serialization, RPM/load and brake layers, and stopping parked audio. Recovery checks confirm the vehicle rises out of terrain once clear, refuses to push through a nearby player, and does not move when occupied.

The vehicle suite also passes camper fixture use, cabin walking, chauffeur movement, controlled stop, all vehicle seat layouts, waypoint arrival and stopping at a barricade.

Initial verification found a missing door trace collider, corrected with a box attached to the hinged leaf. Combat fixtures were adjusted to update the camera before tracing. The recovery fixture originally stood inside the embedded car; it now explicitly checks that this blocks recovery, steps clear, and checks the nudge. Its height comparison uses Unreal's double-precision position type. Temporary diagnostic logging was removed.

## Visual evidence

Reviewed images are retained in `Docs/ScreenshotsV17`:

- `LaserArms17.png`: emissive beam/contact dot and the forearm extending out of the camera frame.
- `Punch17.png`: visible unarmed hands and target enemy.
- `Entry17.png`: side doorway, fitted door and exterior steps.
- `Cabin17.png`: hollow interior with full-height kitchen cabinets, fridge, wardrobe and bed standing above the wooden floor.

This was an automated editor regression pass with screenshot inspection, not an extended manual playthrough. Driving sounds are original synthesized placeholders controlled by the new mix system; production recordings can replace them in the audio catalog. Interior walking uses the existing motorhome cabin controller with doorway entry/exit transitions. No shipping build was tested.
