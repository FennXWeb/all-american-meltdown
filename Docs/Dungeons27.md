# Deep dungeons — update 27

Ten hostile facilities are added to the procedural world. All use the existing discovery system: approach the site to reveal its map marker and earn discovery XP. Dungeon reservations have road connections and undergo road/parcel overlap checks. They do not replace existing saved containers or ordinary POIs.

| Facility | Size | Floors | Rooms | Guards, including final sentries | Design |
|---|---|---:|---:|---:|---|
| Redline Pumpworks | Normal | 2 | 18 | 18 | Pump machinery, filter galleries, operator rooms and chemical stores; restore water controls. |
| Saint Mercy Quarantine | Normal | 3 | 27 | 26 | Wards, surgery, morgue and pharmacy; restore decontamination. |
| Blackwater Remand | Normal | 2 | 24 | 24 | Cell blocks, visitation, intake and security; override prison security. |
| Federal Bullion Depository | Large | 3 | 48 | 63 | Counting rooms, audit archives, deposit lockers and transport offices; authenticate the strongroom. |
| Dead Air Broadcast Center | Large | 4 | 36 | 43 | Studios, newsrooms, tape archives and transmitter decks; restore the emergency signal. |
| Rusthaven Steelworks | Large | 3 | 60 | 78 | Casting halls, machinery, tool stores and furnace control; isolate the furnace feed. |
| Eden Research Annex | Large | 4 | 64 | 83 | Specimen labs, clean rooms, containment, observation and cold storage; release the prototype vault. |
| Patriot Missile Command | Massive | 5 | 100 | 129 | Launch control, service bays, barracks, communications and blast control; cancel the launch interlock. |
| Union Central Interchange | Massive | 5 | 125 | 164 | Platforms, ticket halls, freight rooms, signal control and evacuation holding; switch traction power. |
| Executive Continuity Arcology | Massive | 7 | 175 | 228 | Reception, executive residences, archives, command and private refuge; revoke the executive seal. |

## Route and encounter design

Every level has a seeded connected room graph: a spanning route, branching side rooms and sparse loops. Security controls sit in distant rooms; the middle control uses a different room from the final control even in two-level sites. All floors connect through a separate return-flight stair core. Steps rise 17.86cm with 30cm treads. Main room arches are 3.8m wide and 4.1m high, allowing large enemies through. Heavy props occupy corner bays, leaving cross-shaped circulation between exits. Room numbers, level signs, lit landings, pipework and an entrance directory aid navigation.

Each facility has distinct furnishings and room names using existing detailed meshes, plus newly assembled structural modules, equipment mounts, service pipework, rail beds, bars, shutters, facade pilasters and rooftop silhouettes. These are generated in-engine; no new bitmap textures or imported mesh assets are required.

Guard positions are explicitly placed in room clearances. Raiders, infected, dogs and stronger final sentries vary by facility. A local director activates at most 24 living enemies, up to three per update, subject to the global population budget. Other decks deactivate and retain remaining health while the site stays loaded. Defeated IDs persist. These counts describe the whole dungeon, not simultaneous active NPCs.

## Objectives and reward

Use **E** at the entrance directory for control levels. Find and activate three security consoles, then defeat the final sentries to release the end reserve. Circuit progress and remaining final guards appear on the HUD while inside. The directory, consoles and reserve have individual interactions.

| Size | Credits | Base XP | Guaranteed weapons |
|---|---:|---:|---|
| Normal | 1,800 | 500 | Two Rare weapons |
| Large | 4,500 | 1,200 | One Legendary and two Epic weapons |
| Massive | 10,000 | 2,500 | One Legendary and three Epic weapons |

The reserve also includes matching ammunition, filled detachable magazines where applicable, medical supplies, lockpicks, a laser and mounted flashlight. Legendary modifiers and skin rolls use the existing weapon system. Quantities split into valid stacks. Additional ordinary caches are distributed among room furnishings, with themed loot and varied locks.

Circuit flags, killed guard IDs, reserve contents and the one-time payout use existing save records. Reopening or reloading cannot reroll the reserve or repeat its credits/XP. Leaving before finishing keeps controls and defeated enemies; surviving enemy health follows existing chunk-lifetime behavior.

## Implementation and checks

- `LWDungeonLayout.h`: definitions, graph generation, objective placement and reward scales.
- `LWDungeonBuilding.cpp`: structure, stairs, room furnishing, signs, lights and interactive devices.
- `LWDungeon.cpp`: activation budget, persistent objectives and gated rewards.
- `LWGeneration.h`: checked roadside reservations; `LWPOITypes.h`: discovery/map integration.
- `Tools/VerifyDungeon27.ps1`: complete automation suite plus all-ten-site rendered checks.

Validation completed: Editor compilation passed (`Saved/Build27d.log`); all 56 automation tests and 432 rendered dungeon checks passed with zero failures. Tests cover all ten generated types, room connectivity, road/parcel separation, every room floor and stair tread, player passage clearance including low step-over rails, enemy activation, secured rewards, valid inventory stacks and one-time payments. Reviewed entry and reserve captures under `Saved/ScreenshotsV17/Dungeon27_*.png`. No game was cooked, staged or packaged.
