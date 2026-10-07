# Upstate New York world

The American map now uses a fixed authored atlas. Chunk streaming remains; random seeds no longer relocate cities, roads, or buildings. Seeds still control loot and encounters. Existing saved progress is not deleted, but a **new game is recommended**: saved world-object positions and discovered locations from the former procedural map refer to a different layout.

## Geography

North is +X, east is +Y. The projection is centred on downtown Buffalo, with regional distances compressed approximately 10:1. Roads, furniture, people and landmark buildings retain normal centimetre scale. Buffalo to Syracuse is roughly 22 km in game before road detours. The four main centres are Buffalo, Niagara Falls, Rochester and Syracuse. Seventeen additional communities include Orchard Park, Williamsville, Tonawanda, Lockport, Batavia, Le Roy, Brockport, Geneseo, Canandaigua, Victor, Geneva, Waterloo, Seneca Falls, Auburn, Skaneateles, Baldwinsville and Liverpool.

The Thruway alignment passes south of Rochester; Niagara, Rochester and Finger Lakes connectors link the centres. This is a gameplay interpretation, not a GIS survey or street-for-street replica. Existing story reservations, the bunker and northern customs gameplay remain accessible. Other named landmarks are not replicas yet: existing airport, prison, casino and mall assets stand in for those destinations.

The map supports a region-wide zoom. City names form the basemap; individual POI discovery still works normally.

## Landmark construction

- **Highmark Stadium (type 68)**: based on the new Orchard Park venue appropriate to the 2030 setting. Regulation 109.73 × 48.77 m field, elliptical seating terraces, radial aisles, field tunnels, concourses, open canopy, scoreboards, concessions, service rooms and marked parking. The building footprint approximates the published 15-acre ground footprint; the developed gameplay parcel is 620 × 580 m rather than the entire 242-acre stadium property.
- **Destiny USA (type 69)**: three full-size retail floors and a glazed Carousel Court rotunda, approximately 230,000 m² gross playable floor area, open central galleries, bridges, stairs, stockrooms, seating, shop fixtures, skylight and parking. The 460 × 180 m retail envelope approximates the published 2.4-million-square-foot destination. Store layout and footprint are an interpretation, not an exact commercial floor-plan reproduction.

Both builders reuse existing meshes and materials. Work is queued through the chunk construction pipeline, and the owning chunk remains retained across the large building footprint. They use stable IDs for interactable stock and existing loot/enemy systems.

## Editing

`LWNewYork69.cpp` owns the town coordinates, route alignments, landmark reservations, fixed parcels and spatial index. `LWNewYorkBuildings69.cpp` builds the two destinations. Appending destinations preserves existing IDs; do not reorder established landmark entries in a shipped save layout. `Tools/build_stream_assets68.py` regenerates the asynchronous asset bundles after changing builder assets.

## References

- [New Highmark Stadium facts](https://static.clubs.nfl.com/image/upload/bills/ead06w4bskebakjtgppa)
- [Erie County stadium environmental survey](https://www3.erie.gov/environment/sites/www3.erie.gov.environment/files/2024-06/21-historic-survey-web.pdf)
- [Destiny USA press guide](https://www.destinyusa.com/uploads/Destiny%20USA%20Press%20Guide-%20June.pdf)
- [NYSDOT roadway inventory viewer](https://www.dot.ny.gov/risviewer)

Editor validation: `LethalWorld.Update69.Atlas`, `LethalWorld.Update69.Routes`, and `-LWV17Smoke -LWUI46Smoke -LWNY69Smoke`. No packaged build is required.

Final validation: editor target compiled successfully (`Saved/Build69f.log`); atlas and all-town route tests passed (`Saved/NY69Automation3.log`) with 1,116 sites and zero footprint conflicts. In-game landmark checks passed 8/8, including standing headroom throughout the mall stair flights (`Saved/NY69SmokeVerified.log`). Screenshots are in `Saved/ScreenshotsV17/NY69_*.png`. User graphics settings were restored after testing. No packaged build was produced.

Quick navigation: `locate highmark` or `locate destiny` sets a waypoint using the fixed atlas. The locate command now searches this atlas directly instead of scanning thousands of generated regions.
