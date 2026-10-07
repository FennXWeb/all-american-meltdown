# Update 57 — Military vehicles, aircraft and new encounters

## Vehicles

- **APC:** eight wheels, eight seats (driver, gunner, six passengers), armored damage resistance, traversing cannon, 24-round ready rack. Uses `ammo_30mm` from vehicle cargo.
- **Armored truck:** two seats, reinforced body, larger cargo inventory, diesel audio and instruments.
- **Armed pickup:** driver, gunner and passenger, bed-mounted machine gun, 100-round feed. Uses `ammo_762` from cargo. Its forward depression limit protects the cab.
- **Utility helicopter:** four seats, animated main/tail rotors, cockpit, skids, fuel use, spool-up, hover damping, vertical flight, collision/landing damage and unpowered descent. Manual pilot; companions ride as passengers. Ground convoy AI does not take helicopters as road vehicles.

Gunners use the first passenger seat. Companions automatically take it when available and independently search for visible hostiles. Rotation speed, line of sight, aim limits, heat, reload time, ammo and friendly obstruction checks gate firing. Player gunner: left mouse fires, R reloads from cargo. Menu clicks stop turret fire. Turret ammunition is saved in the vehicle record.

Helicopter controls: W/S forward/back, A/D yaw, Space ascend, C descend with the default bindings, R ignition. These use the existing remappable movement/crouch actions. Release controls to stabilize. Land before exiting. The HUD shows altitude and rotor speed. Existing keys, hotwiring, fuel, storage and damage systems remain in use.

## Locations

- Military base: perimeter/checkpoint, barracks, command offices, ordnance stores, motor pool and helicopter pad. APC, armed pickup and helicopter placements.
- Airport: extends the existing airport type 32 with rotor operations, a helicopter pad and supplies.
- Bank: teller hall, ATMs, seating, secure deposit room, hinged vault door, high-tier locked storage and an armored truck.
- Gun range: reception, ammunition storage, separated shooting lanes, targets and gun racks.
- Sporting-goods store: camping displays, hunting racks, checkout, stockroom and supplies.

Append-only POI IDs 64–67 preserve existing type IDs. Commercial/industrial selection includes the new shops and range. Large bank/sports parcels are also reserved before road placement, preventing their rejection by undersized urban blocks. Military bases use reserved major parcels. New names use the existing deterministic site naming and address system.

## Enemies

- Giant hornet: airborne orbit, committed sting dives, obstacle avoidance and animated wings.
- Bear: ground navigation, timed charges and telegraphed swipes with recovery windows.
- Rogue AI: fast strafing, short acceleration bursts, variable direction changes, navigation around obstacles and inaccurate burst fire.

Enemy IDs 12–14 extend the enum without changing existing saves. New enemies use the existing health, legendary, injury, corpse-looting and damage systems; they do not count against the giant-boss population limit. Spawn tables expose separate hornet, bear and rogue weights.

## Art and audio sources

- `Tools/make_expansion57.py` authors the mesh contours and exports 41 assets from `ArtSource/ModelsV57/Expansion57.blend`. Hulls, anatomy and aerofoils use custom mesh surfaces. Articulated turrets, rotors and creature limbs are separate assets. Models have three distance LODs.
- `ArtSource/TexturesV57/T_Expansion57.png` is the generated four-surface atlas. Unreal samples top-left UV origin; Blender uses bottom-left.
- `Tools/audio57.py` produced 42 ElevenLabs effects, including 12 vehicle engine layers and five voice variations for each new enemy. Generation used **1,036 credits** according to the saved ledger. Keys are not stored in the project.
- `Tools/import57.py` imports materials, meshes and audio. `Tools/finish57.py` generates LODs and updates the new spawn/loot rows. All new sounds are replaceable in `/Game/Audio/DA_AudioCatalog`.

Texture prompt: Square material atlas divided into four equal quadrants: worn olive military paint; dense brown grizzly fur; ochre and charcoal hornet chitin; graphite machined robot armor. Flat orthographic albedo, no text or borders, tactile 2008-era post-apocalyptic materials. Generated with the built-in image-generation tool.

## Validation

Editor compilation, strict FBX bounds/material validation, and an isolated `-LWV17Smoke -LWUI46Smoke -LWExpansion57Smoke` gameplay scenario. Captures are written to `Saved/ScreenshotsV17/Expansion57_*`. The scenario covers vehicle meshes/cockpits/audio, companion firing, ammunition conservation, UI capture, flight/landing, enemy setup, generated POI availability, and POI layouts/pad clearance.

Final results: editor build succeeded (`Saved/Build57g.log`); all 65 gameplay checks passed (`Saved/Smoke57c.log`); 16 distinct regression tests passed across `Saved/Tests57.log` and `Saved/Tests57b.log`. The first regression run exposed two stale test assumptions, updated to cover 14 road vehicle types with helicopters excluded, and distant playable regions south of the closed border. The existing northern-boundary test also passes. Cockpits, turret clearance, creature silhouettes and POI interiors were reviewed in engine screenshots.

No packaged build is generated.
