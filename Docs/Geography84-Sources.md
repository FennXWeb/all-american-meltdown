# Geography and airport sources — update 84

Retrieved September 26, 2026. Runtime map generation uses the baked local data; it makes no network requests.

## Sources and attribution

- **© OpenStreetMap contributors**, [Open Database Licence](https://www.openstreetmap.org/copyright). Town streets and the Hancock building/taxiway outlines come from OSM API 0.6 map extracts. Preserve this attribution when distributing the derived map/database. Raw extracts and derived data are in `ContentSource/Geography84`; the reproducible conversion scripts are in `Tools`.
- **Natural Earth**, public-domain 1:10m roads, lake polygons, river centerlines and country boundaries, from [Natural Earth Vector](https://github.com/nvkelso/natural-earth-vector/tree/master/geojson). This supplies regional coverage outside the more detailed OSM town extracts. These generalized datasets are not a parcel survey.
- [Syracuse Hancock airfield specifications](https://syrairport.org/about-us/airfield-specifications/): active runways 10–28 (9,003 × 150 feet) and 15–33 (7,500 × 150 feet), taxiway dimensions and terminal/concourse context.
- [Syracuse Hancock master plan](https://syrairport.org/2021-syr-master-plan-update/), including the published existing airport layout and terminal alternatives: building/concourse arrangement and airfield reference. Official plan imagery is reference material, not redistributed game textures.

## Coordinate conventions

North is game +X, east is +Y. The regional projection uses Buffalo as its origin and compresses regional distances approximately 10:1. The existing enlarged Syracuse projection is preserved, so local landmarks and roads remain near their established playable scale. The transition outside Syracuse is intentionally nonlinear. Hancock's terminal and runway geometry use a local metric projection around 43.1133, −76.1112; runway lengths are retained at real scale.

Lake Ontario, Lake Erie, Oneida, Cayuga and Seneca follow lake polygons. The existing detailed Onondaga Lake shoreline is retained. The St. Lawrence follows its surveyed course with a playable width. Canada is classified against geographic country polygons rather than a horizontal map cutoff. Checkpoints occupy the Peace Bridge, Lewiston–Queenston and Thousand Islands crossing areas. Toronto remains on the northwestern side of Lake Ontario.

The original Syracuse landmark and campaign IDs are retained. `LWGeographyMigration84.cpp` migrates old northern campaign and Toronto positions once, including nearby player vehicles, containers and player-built settlement pieces. Inventory and currency are preserved.

## Reproduction

1. `Tools/fetch_geography84.py` downloads missing raw source files only.
2. `Tools/bake_geography84.py` generates the geographic header and derived review data; requires Shapely.
3. `Tools/bake_airport84.py` unions the five terminal footprints and triangulates floors around stair openings; requires Shapely.
4. `LethalWorld.Update84.Geography` exports the final in-game parcel list to `Saved/Geography84_Parcels.csv` for inspection.

The importer preserves short junction segments and joins disconnected town extracts to regional roads with short, dry-land approaches. These visible approach roads are authored connections; they are not surveyed street alignments. Intersecting landmark reservations are bypassed together so a later detour cannot cut into an earlier building.

Town zoning, shops, homes, community routines, interiors and regional airport buildings are authored game content placed against those sources. They are not claims of an exact reconstruction of every real building or room. Other airports use compact functional runways around their geographic locations. The Griffiss playable airfield is offset 1.35 game kilometres east to keep its runway and terminal reservation clear of compressed Rome and the established story facilities.
