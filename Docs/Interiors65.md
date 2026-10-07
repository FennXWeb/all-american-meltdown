# Interiors revision 65

All 68 procedural POI types now pass through a construction-aware interior furnishing system. Story sites and settlement buildings use it as well. It combines shared structural repairs, a rebuilt shopping mall, replacements for selected older props, and room-specific furnishing. Outdoor parking, runways, courtyards, circulation routes and intentionally ruined areas remain open.

## Building changes

- **Shopping mall:** twelve integrated stores with glazed fronts, separate stockrooms and fitted ceilings; a continuous skylit gallery with open kiosks, information desk, seating and planters; a food hall, kitchen, restrooms, and operations/security rooms. The shops share the mall's structure instead of sitting inside it as separate roofed buildings.
- **Airport:** continuous east floor edges, full-width front/rear glazing, facade sills and spandrels, finished acoustic ceilings, and service partitions that meet the ceiling. Paired waiting banks flank the public concourse on both passenger decks.
- **Casino:** closed shop and hotel corridor end partitions, extra playable slot banks and table-game rows. Its seven floors and stair flights remain functional. Furnishing budgets apply per floor so the lower casino cannot consume the hotel floors' allocation. New game tables use separate IDs to preserve older resort records.
- **Towers:** continuous corner posts, enclosed bathrooms with fitted doors, and complete workstations in place of desktop-only floor props. Stair access and intentionally inaccessible floors remain protected.
- **Expansion buildings:** corrected a shared four-centimetre wall-to-roof seam. Indoor partitions in unique landmarks now meet the ceiling.
- **Bank:** secure deposit room and fixtures align with the bank floor.
- **Homes and diner:** fitted domestic ceilings close the roof void; the diner cooking line is behind the staff partition, with a coffee counter facing the dining room.
- **Police, arcade and plaza shops:** finished acoustic ceiling surfaces beneath the exterior roof sheets.
- **Underground sites:** furnishing runs after cave rock placement and before lowering the complete assembly underground.

## New assets

`ArtSource/Interiors65` contains **61 custom meshes**, editable Blender source, FBX exports, a bounds/triangle manifest, a generated material atlas, and a generated atlas of **16 original wall prints**. Unreal assets are in `/Game/Art/Interiors65`: 61 meshes, 32 materials and two textures. Each mesh has three LODs.

The kit includes upholstered sofas, armchairs and restaurant booths; desks, chairs and computer equipment; cabinetry, stocked shelves and lockers; dining, cafe and coffee tables; beds; kitchen and bathroom appliances; service carts; plants, mats and rugs; framed artwork, noticeboards, clocks, mirrors, radiators and fire extinguishers; ceiling panels, vents, cable trays, ducts, pipes, fans and light fixtures. Geometry includes shaped cushions, handles, drawers, wheels and other modeled details. Selected older furniture and wall/floor finishes are replaced during POI construction.

## Placement and interaction

The construction scope records each site's floors, walls, ceilings, props and interactive objects in local coordinates. Additional furniture comes from residential, medical, industrial, hospitality, retail or civic groups, refined by nearby room contents. Floor props require support under their corners and centre. Wall and ceiling fixtures require backing across their bounds.

Placement rejects intersecting bounds and reserves door swings, doorless openings, main entrances, important circulation paths, dungeon combat routes, stair edges, existing NPC positions and future story objectives/spawn positions. Desks and dining tables receive suitable chairs and tabletop objects when space permits. Cabinets and shelving prefer wall alignment; smaller groupings have seeded variation. Candidate cells are visited in deterministic shuffled order so finite budgets populate the entire deck instead of filling one edge first.

Storage uses existing persistent loot records, and chairs/beds use existing furniture interactions. Added containers have deterministic keys without renaming older containers. The kit preloads asynchronously; noninteractive decor uses instanced meshes. Added storage/seating actors have no idle tick and use distance culling. Player-built bunker furniture is not replaced by this POI construction scope.

## Validation and scope

The editor target compiles successfully. `LWInteriors65Smoke` builds all 68 POI types and nine story sites in an isolated save namespace, validates placement rejection, checks finished-floor coverage and captures representative interiors. It also checks support/headroom on all 144 casino stair treads and 64 airport stair treads, plus the airport's three east floor edges. Results and per-type construction counts are retained in `Saved/Interiors65SmokeComplete.log` and `Interiors65Audit.csv`.

Final run: **114 checks passed, zero failures**, 105.5 seconds after test setup. All 208 stair treads passed, and no new interior material reported missing instanced-mesh usage flags. Nine representative interior screenshots are in `Saved/ScreenshotsV17/Interior65_*.png`. The editor build log is `Saved/BuildInteriors65Reviewed.log`. The user's graphics settings were restored from the pre-test backup.

This is procedural coverage plus targeted layout work, not a claim that every possible seed, rotation and playthrough has been visually inspected. Representative interiors are reviewed in-engine; the smoke audit does not prove all routes for every NPC capsule size. The largest POIs still construct synchronously, so this update is not a complete solution to world-streaming hitches.

No packaged build is produced. Asset generation/import scripts are `Tools/build_interiors65.py` and `Tools/import_interiors65.py`.
