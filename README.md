# All American Meltdown

First-person wasteland survival across upstate New York and southern Ontario, with companions, settlements, vehicles, and walkable aircraft.

Download the portable Windows build from [GitHub Releases](https://github.com/FennXWeb/all-american-meltdown/releases), or add `FennXWeb/all-american-meltdown` to SMOG. Extract the complete build and run `smog_launch.bat`. Saves and settings live outside the installation at `%LOCALAPPDATA%\AllAmericanMeltdown\Saved`.

See [SMOG release instructions](Docs/SMOG/Release.md), [version 0.84.0 notes](Docs/SMOG/ReleaseNotes-0.84.0.md), and the [regional world and aviation guide](Docs/Update84-Implementation.md). The root `smog_*` files supply launcher metadata and generated artwork. Unreal Engine 5.8 is required for source development; it is not required to play the packaged game.

The sections below are historical development notes. Their older instructions and packaging status describe those updates, not the current release.

# All American Meltdown - Field equipment and terrain

Five backpack sizes now expand carried inventory, chest rigs unlock a primary weapon slot on **5**, and helmet-slot night vision toggles with **N**. POI terrain clearance, tower floor overlap, casino driveway overlap, and downhill vehicle support are corrected. See [capacity, controls and verification](Docs/EquipmentAndTerrain25.md). Editor/source update only; no packaged build.

# All American Meltdown - Arsenal and lighting

Seven new weapons, persistent legendary effects, 30 rarity-specific finishes, and selectable Lumen/hardware ray-traced lighting are implemented. Glass and wet surfaces use updated reflection materials. See [weapon controls, asset tools and lighting details](Docs/ArsenalAndLighting24.md). This is an Editor/source update; no packaged build was created.

# All American Meltdown - Injuries and vehicle dents

Rarity colors now identify weapons in inventory and on the HUD. Living enemies react to limb loss, including hobbling/crawling, impaired attacks and a 3–10 second headless run before death. Ragdolls react to repeat impacts, and vehicle dents deform body geometry and persist with the vehicle. See [details](Docs/InjuriesAndDents23.md). No packaged build was created.

# All American Meltdown - Casino discovery and impacts

Use **FIND CASINO** on the map to route to a resort. Casino markers now have their own gold label, and towns have additional collision-checked resort plots. Blood effects, enemy dismemberment, corpse damage and speed-based vehicle impacts are implemented. Settlers and companions retain recoverable downing. See [details and validation](Docs/CasinoAndImpacts22.md). No packaged build was created.

# All American Meltdown - Casino resort update

The Gilded Republic adds seven floors of gaming, shops, dining, hotel rooms and VIP spaces, with supercar-favored parking. The supercar has a new body, wheels and adjusted cockpit. See [resort details](Docs/CasinoResort21.md). No packaged build was created.

# All American Meltdown - Urban world update

District-based world layouts, residential neighborhoods, dense downtown towers with varied access, sidewalks, street lamps and neon signs. See [world generation details](Docs/UrbanWorld20.md). Open `LethalWorld.uproject` in Unreal Editor; packaging remains your step.

# All American Meltdown - Build 0.18

Companion navigation update: door-aware routes, stairs and elevation checks, obstacle recovery, and automatic opening/closing of usable doors. See [navigation details](Docs/CompanionNavigation.md).
Break-action double barrel shotgun, vehicle fires and wrecks, rebuilt police station/arcade/plaza, five new enemies, bed respawns, ten-companion progression and overflow vehicle convoys. Settlements now have names, sizes, reputation, membership, leadership, stationed crew, passive income and shop consignments. See [update details and controls](Docs/Update18.md) and [validation](Docs/Validation18.md). Open `LethalWorld.uproject` in Unreal Editor. Packaging remains your step.

# All American Meltdown — Build 0.17

Glowing laser beams, corrected gun forearms and unarmed punching; a hollow motorhome with walk-through door and separate seat/storage targets; parked terrain recovery and layered driving audio. See [update details and controls](Docs/Update17.md) and [validation](Docs/Validation17.md). Open `LethalWorld.uproject` in Unreal Editor. No packaged build was created.

# All American Meltdown — Build 0.16

Transparent animated menu branding, expanded survivor appearance with ten hairstyles, redesigned bunker rooms and locker storage, a longer Class A motorhome, six-AM starts and revised day/night lighting, settlement defense, companion map markers, and HUD XP progress. See [update details](Docs/Update16.md) and [validation results](Docs/Validation16.md). Open `LethalWorld.uproject` in Unreal Editor. Packaging is left to you.
# All American Meltdown — Build 0.15

Added a nine-scene subtitled opening, a character creator with saved appearance and starting category points, a bunker start, new title artwork, and a compact HUD. See [opening and character creation](Docs/Opening15.md). Open the existing `LethalWorld.uproject` in Unreal Editor; internal module and save identifiers remain compatible. No new package was created.

# Build 0.14

Added per-POI enemy populations, saved weapon rarities, eight visible weapon attachments, a rebuilt five-seat live-in camper, selectable vehicle seats, and companion waypoint driving. See [equipment and vehicle controls](Docs/Gameplay14.md). Play in the editor; no new package was produced.

# Build 0.13

Corrected pitched house roofs and rebuilt the ranch/cottage room layouts with more realistic footprints. Added 19 original furniture/detail models and improved furnishing, stock placement, and circulation across POIs. See [POI improvement notes](Docs/POIImprovements13.md). Play in the editor for these changes; no new packaged build has been produced.

# Build 0.12

Tavern games now have generated card artwork, textured tabletop play, fanned hands, direct card handling, dealing animations, paced opponents, and card/chip audio. Blackjack adds split hands and surrender; Pitch uses a rotating auction. See [Build 12](Docs/Build12.md) and [card artwork provenance](Docs/CardArtwork12.md). No packaged build was produced.

# Build 0.11

Settlement taverns now include playable wagering tables for Blackjack, partnership Pitch, and Last Card (UNO-style). Music slots support editable randomized playlists. See [Build 11](Docs/Build11.md) for rules, save behavior, and audio setup. [Validation](Docs/Validation11.md): editor compilation, 41 automation tests, 77 tavern/card/music checks, and 1,777 POI/vehicle regression checks passed. No packaged build was produced.

# Lethal World â€” Open Country / Build 0.7

A native Unreal Engine 5.8 C++ first-person survival game with a streamed procedural wasteland, original low-poly art, worn materials, bitmap UI, and degraded camera rendering.

See [Build 0.10](Docs/Build09.md) for rebuilt vehicles, rain effects, the boutique, sleeping, seating, and world setup.

See [Build 0.10](Docs/Build10.md) for 56 roadside encounters, choices, saved consequences, and the road journal.

## Play

Open **`LethalWorld.uproject`** in Unreal Engine 5.8 and play in the editor for the latest changes. The existing executable under `Builds/Windows` and `Play.ps1` still run the older packaged release; no new package is produced automatically.

Press **Enter** to continue; on first launch, choose a seed and world settings before creating your survivor. **New Survivor** opens those settings again. **B** plots a route to Shelter 01; use **E** on its airlock. The bunker stash contains the new weapons, spare magazines, and practice ammunition. Drag equipment into your carried inventory and compatible equipment slots before heading out.

| Action | Input |
| --- | --- |
| Move / look | WASD / mouse |
| Sprint / crouch / jump | Shift / Ctrl / Space |
| Attack / aim | Left / right mouse |
| Primary / secondary / sidearm / melee slot | 1 / 2 / 3 / 4 |
| Reload / fire selector | R / V |
| Inventory / player menu / route home | I / Tab / B |
| Skills / contract journal / crew | K / J / O |
| Use / flashlight | E / F |
| Rotate dragged item / unload selected / drop selected | R / U / Delete |
| Pause or close panel / save / load | Escape / F5 / F9 |

See [the 0.6 guide](Docs/Build06.md) for quick transfer, auto sorting, the shared menu, settlement routines and companion passengers. See [the 0.5 guide](Docs/Build05.md) for locks, keys, hotwiring, driving and cabin controls. See [the 0.4 guide](Docs/Build04.md) for progression, quests, companions, dialogue, settlements and the expanded bunker. Green WAYSTATION markers on the map lead to merchants, wardens, medics and hireable survivors. See [the 0.3 guide](Docs/Build03.md) for destructible POIs and enemies, and [the 0.2 guide](Docs/Build02.md) for inventory, physical ammunition and map controls.

See [the 0.8 guide](Docs/Build08.md) for seating, furniture storage, placement fixes and weather. Validation for this update is recorded in [the 0.8 test report](Docs/Validation08.md). Previous release reports remain in the Docs folder.

## Included systems

- **Vehicle survival:** eleven first-person vehicle profiles with distinct handling, seats, cargo and equipment; police lights and sirens; four lock tiers, lockpicks, VIN-bound keys, hotwiring, glovebox storage, radio, lights, wipers and indicators.

- **Progression:** seven attributes, 49 perks with rank and level gates, two points per level, and persistent combat, survival, crafting, loot, economy and squad benefits.
- **Survivor network:** 12 staged contracts with prerequisite chains, branching subtitled gibberish conversations, friendly waystations, vendors, medical services and persistent recruitable companions. One follower by default; capacity skills unlock up to four.
- **World:** deterministic 128 m chunks, displaced regional hubs, curved highways, sparse rural branches, towns, driveways and terrain blending. Eighteen POI types include rebuilt businesses, homes, apartments, retail, warehouses and civic buildings. Negative and distant coordinates are supported with bounded streaming.
- **Combat:** crowbar, bat, shotgun, revolver, sniper, SMG, combat rifle, and LMG. Original models, mechanical reload motions, distinct audio, spread, recoil, range falloff, and melee stamina. Individual magazines, separate chambers, loose-ammo loading, spent revolver cases, and persistent ammunition state.
- **New encounters:** rifle raiders, fast rabid dogs, and mannequins that stalk while unobserved, then run and attack even while watched once aggressive. Interactive doors, breakable windows, chain-reacting fuel pumps, searchable vehicle cargo, and saved prop state.
- **Infected:** vision, noise investigation, obstacle pathfinding, attacks, stagger, persistent defeat IDs, and seven-body constrained physics ragdolls.
- **Equipment:** grids with rotated and irregular footprints; armor, helmet, weapon, rig, backpack, and tool slots; supplies and consumables; drag-and-drop transfer, compatible ammo loading, unloading, and dropping.
- **Economy:** containment credits, weighted context-specific loot tables, persistent containers, and semi-rare wandering exchange robots with buying and selling.
- **Safehouse:** ten companion bedrooms, private lockers, workshop, medical room, mess hall, communications room, usable furniture and secured shared stash. Death drops carried gear while retaining progression and unlocked companions.
- **Navigation:** full map, minimap, landmarks, waypoint placement, and dynamically updated road routes drawn onto world surfaces.
- **Presentation:** original texture/model/audio source assets, atmospheric lighting and fog, bitmap menus and HUD, grain, scanlines, dither, color quantization, chromatic fringe, and subtle vertex snapping. Higher sensitivity and video-mode settings are included.
- **Acoustics:** spatial attenuation, wall occlusion and low-pass filtering, indoor reverb, shelter-aware ambience, surface footsteps, and menu/exploration/combat music.

## Edit audio and loot in Unreal

Use **Tools â†’ Lethal World Audio Manager** to open `/Game/Audio/DA_AudioCatalog`. Replace a slot's Source by dragging in an audio asset; edit its volume, pitch, loop, and attenuation settings, then save. **Tools â†’ Validate Lethal World Audio** checks the catalog. All 55 default event slots are named and described.

`/Game/Data/DA_LootTable` controls loot context, rarity, weights, stack ranges, guarantees, empty chance, uniqueness, caps, and hazard scaling. `/Game/Data/DA_ItemCatalog` controls item footprints, equipment/ammo compatibility, stack limits, and prices.

## Build and verify

The latest editor update is [0.7 â€” Open Country](Docs/Build07.md): expanded vehicles and handling, rebuilt POIs, irregular roads, mannequin aggression, level-up audio and HUD quest counts. [Validation](Docs/Validation07.md). No packaged 0.7 build is produced.

Development workflow: implement changes and run appropriate compile/test checks only. Packaging is handled by the project owner; do not cook, stage, archive, or package builds unless they explicitly request it.

Requirements: Unreal Engine 5.8, Visual Studio 2022 C++ tools, and the Windows SDK. `Tools/Build.ps1` compiles the editor module; `-Content` imports all assets through 0.4; `-Package` builds the Windows game. Override `-Engine` when Unreal is installed elsewhere.

Run `Tools/Verify.ps1` for Unreal automation and the offscreen gameplay suite. Individual suites are under `LethalWorld.Generation`, `.Inventory`, `.Loot`, `.Navigation`, `.RPG`, and `.Weapons`. `-LWV2Smoke` runs the integrated gameplay test using a separate automation save slot. Logs and screenshots are written under `Saved`.

Original asset generators are under `Tools`. The 23 modular additions have assembly/pivot documentation in `ArtSource/ModelsV2/README.md`. Music and sound effects are original synthesized placeholders and can be replaced through the catalog.

## Scope

This is a playable single-player procedural foundation, not an authored campaign. The inventory has one carried grid, equipment slots, and external storage; worn rigs and backpacks are not nested containers. Merchants retain stock without timed restocking. Living infected, trader patrol position, and relief cooldowns reset when their chunk unloads; defeated IDs, looted container contents, gear bags, and trader stock persist. World size is practically near-infinite rather than mathematically unlimited.

The visual direction and audio features are inspired by crusty survival horror. Models, textures, signage, and sound assets are original; the bitmap font is rasterized from locally installed Consolas. Audio uses Unreal's native [attenuation and occlusion](https://dev.epicgames.com/documentation/unreal-engine/sound-attenuation-in-unreal-engine?lang=en-US).



Update 26: [Discovery, fast travel, severe weather, NPC models and combat](Docs/ExplorationAndCombat26.md). Editor changes only; packaging remains with the project owner.

Update 27: [Ten deep dungeon POIs](Docs/Dungeons27.md), from 18 rooms over two levels to 175 rooms over seven levels, with persistent security controls, staged guards and secured high-value rewards. Editor-only validation; no packaged build.

Update 28: [Unique landmarks, rare airports, stamina perks and settlement interactions](Docs/Landmarks28.md). Twenty one-per-world landmark activities and a three-level airport terminal. No packaged build.


## Combat and underground expansion (update 29)

Direct-loaded missiles and taser batteries, pump-refillable flamethrower canisters, gun bashing (middle mouse), persistent corpse loot, starred legendary enemies, target health bars, three new large enemy types, three underground sites, residential/medical tower variants, and fitted breakable-glass facades. See [controls, behavior, and verification](Docs/CombatAndUnderground29.md). Editor changes only; no packaged build.
