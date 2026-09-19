# Urban world generation

The existing seed and world-density settings now drive planned neighborhoods as well as the regional road backbone. No packaged build is produced.

## Layout

- Regional highways and curved connecting roads still join neighboring regions, leaving undeveloped countryside between population centers.
- Towns have commercial centers, separate residential lanes and housing blocks. Highway-side development includes fuel, motels and larger retail destinations; industrial roads favor depots and warehouses.
- Selected populated regions develop compact downtown street blocks. Apartment estates form a residential edge around the core.
- Building footprints are checked against local and neighboring roads and other oriented footprints. Site IDs and layout remain deterministic for a seed.

## Towers

The new tower POI has 12–22 floors. Its seeded access roll selects condemned (30%), partially accessible (50%, 2–5 floors) or fully accessible (20%). These are candidate probabilities; surviving world placement can change the observed proportions.

Accessible floors contain office suites, working doors, seating, cabinets and shelves with persistent loot, floor numbers and switchback stairs. Condemned entrances are physically sealed with concrete/debris. Partial towers stop below a collapse slab. Facades, columns, canopies and roof equipment remain present above inaccessible floors.

## Lighting

Street lamps and framed neon shop signs stream with their parcels. Street illumination fades on at dusk and off after dawn, and nearby lights activate within 65m of the player. Tower interior lights use a 45m draw limit. Light housings, poles and neon tubes are assembled as instanced geometry. New materials are created additively by `Tools/build_urban20_content.py`; existing artist assets and spawn-table overrides are retained.

## Files and testing

- `LWGeneration.h`: district planning and parcel geometry.
- `LWUrban.cpp`: tower geometry, furniture and lighting.
- `LWUrbanTests.cpp`: three-seed district/access/road-clearance coverage.
- `LWUrbanSmoke.inl`: in-engine tower floor/stair collision checks and rendered views.

Run the editor automation group `LethalWorld.Generation` and `LethalWorld.V7.RegionDiversity`. The rendered smoke suite uses `-LWV17Smoke -LWUrbanSmoke` and an isolated automation save slot.

Existing saves are not deleted or migrated to a new seed. Because the generated layout changed, a new world is the cleanest way to evaluate the new geography; stored loose objects from an older layout retain their saved positions.

## Validation (September 7, 2026)

- Editor Development build succeeded (`Saved/BuildUrban20f.stdout.log`). No packaging.
- All 46 editor automation tests passed (`Saved/Urban20_Automation.log`).
- The urban test sampled 243 regions across three seeds: 42 city regions, 30 with at least three surviving towers, 51 condemned towers, 74 partial towers, 19 fully accessible towers, and all 21 POI types represented.
- Offscreen gameplay suite passed 370 checks with zero failures (`Saved/Urban20_Rendered.log`): tower assets, accessible floors, stair treads and overhead clearance, storage/door counts, night lighting and live city streaming.
- Rendered previews are in `Saved/ScreenshotsV17/Tower20_*.png` and `Saved/ScreenshotsV17/Urban20_Downtown.png`.

These are sampled geometry and runtime checks, not an exhaustive playthrough of every generated seed or floor.
