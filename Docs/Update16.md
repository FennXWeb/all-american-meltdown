# All American Meltdown — 0.16

Editor/source update; no packaged build.

## Menus and survivor

The generated transparent logo is in `ArtSource/Branding16/T_AllAmericanMeltdownAlpha.png`. The main menu animates embers and a subtle heat shimmer using the original alpha. The pause menu uses a still cutout. The opening's alternate-history label has been removed.

The creator has male/female silhouettes, height, weight and shoulder-width adjustments, six skin tones, three face proportions, four hair colors, four outfits and ten original hair meshes: buzz cut, textured crop, side part, long, bob, ponytail, curls, mohawk, braid and bun. These are deliberately low-poly models for the game's existing visual style. Appearance is saved as additive identity fields; collision size and gameplay stats remain independent of cosmetic proportions. First-person legs use the chosen proportions and clothing.

## Bunker

All ten bedrooms now have a bed, bedside storage and lamp, wardrobe, work surface, chair, books, rug and wall art. The four service spaces have separate mess, workshop/comms, infirmary and storage layouts. Secure storage is the first locker in the new storage room; its original inventory is retained. Thirteen additional lockers provide separate storage. Legacy bedroom desk/crate and utility cabinet record IDs remain accessible through the redesigned furniture. Bookshelves, bedside cabinets and pantry units also have usable inventories. Storage records are restored on new game and load without reseeding bunker supplies.

The inner hatch casing meets the inside wall face in a dedicated wall recess. The hallway box is gone.

## World and combat

New games start at 06:00. The sun rises from the horizon at 06:00 and reaches daytime illumination through the morning; cloud cover still reduces light. A textured moon follows the opposite arc and supplies only faint light at night. Existing saves retain their time of day.

The HUD now includes a small XP progress bar, measured between the previous and next cumulative level thresholds. Active companions appear as cyan crosses on the minimap and full map; off-map companions pin to the map edge.

Settlement residents defend against nearby visible enemies. Player attacks mark that settlement hostile, including tavern residents; other towns are unaffected. Hostility is stored with world state and survives streaming/saving. Hostile residents refuse conversation and trade. The existing downed/recovery behavior for residents is retained.

## Motorhome

An original approximately twelve-metre Class A shell replaces the previous coach. It has a flat front, wide windshield, three axles, side window openings, sweeping black/metal graphics, luggage hatches, mirror arms, roof AC units and awning cassettes. The geometry is inspired by the user's reference proportions without copied branding.

One driver and four rear passenger seats remain. Dashboard controls, windshield rain and wipers, lights, seat cameras and the walking aisle are relocated to the longer shell. Rear kitchen, sink, refrigerator, pantry, wardrobe and bed preserve their interactions and inventory keys. The wider/longer chassis and wheelbase affect steering and obstacle clearance. Suspension height accounts for the offset axle midpoint and samples terrain beneath the chassis overhangs, preventing the longer body from catching uneven ground. Obstacle collision remains enabled; this clearance correction does not lift the vehicle over props.

## Authoring

- `Tools/make_models_v16.py`: reproducible Blender geometry and manifest.
- `ArtSource/ModelsV16/AllAmericanMeltdown_V16.blend`: editable source scene.
- `Tools/build_v16_content.py`: Unreal asset import with strict mesh bounds/material validation.
- `ArtSource/Branding16/prompt.txt`: exact image-generation prompt and animation implementation.
- `-LWV16Smoke`: isolated rendered regression, using `AllAmericanMeltdown_AutomationV16` save slot.

Validation results are recorded separately after execution.
