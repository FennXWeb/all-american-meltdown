# Syracuse reference reconstruction

Syracuse uses an enlarged geographic core with near-real-size landmark footprints. Regional travel compresses outside the city. Coordinates use north as +X and east as +Y. The geographic projection and its inverse are shared by terrain, roads, sites and lake queries; no map requests occur during play.

The core extends nine real kilometres from central Syracuse. Outside it, a continuous, monotonic radial transition rejoins the prior regional projection. This preserves ordering without reversing or folding roads. Old local grid streets are replaced inside the survey boundary, and boundary links connect to the regional network.

## Map data and attribution

Map data © [OpenStreetMap contributors](https://www.openstreetmap.org/copyright), available under the [Open Database License](https://opendatacommons.org/licenses/odbl/1-0/). The downloaded extracts and derived tables are in `ContentSource/Syracuse73` and `Source/LethalWorld/LWSyracuseData73.inl`. Preserve this attribution and the database license when redistributing these files or a derived database.

`Tools/build_syracuse73.py` bakes the extracts into offline street, building and shoreline tables. It also produces `stats.json` and `Layout73.png`. Road junction vertices are preserved. Building rectangles are reconstructed from mapped footprints; this is a game reconstruction, not a survey or exact architectural CAD model. Road elevation and bridge engineering are not supplied by this dataset.

## Architectural references

- [Official Syracuse zoning map](https://www.syr.gov/Departments/Zoning-Administration/Syracuse-Zoning-Map) and [Onondaga County GIS](https://onondaga.gov/planning/gis/).
- [NYS Fairgrounds maps](https://nysfairgrounds.ny.gov/maps) and [official Fair visitor maps](https://nysfair.ny.gov/your-visit/maps/): hall locations, circulation and entrance placement.
- [Palace on James](https://www.palaceonjames.com/) and [Visit Syracuse Palace listing](https://www.visitsyracuse.com/listing/palace-theater/44/): auditorium, stage, exterior, upstairs event space, tin ceiling and commercial kitchen.
- [Oncenter War Memorial venue](https://www.syrvenues.com/p/book/our-spaces/upstate-medical-arena-at-the-oncenter-war-memorial) and [visitor guide](https://www.syrvenues.com/p/visit/azguide): rink, seating, three levels, concourses, locker rooms, memorial displays and upper club.
- [Syracuse University campus map](https://www.syracuse.edu/map/) and [official Dome information](https://cuse.com/sports/2009/2/3/GEN_0203090820): campus placement, field and arena scale, fixed roof and structural crown. The game retains the user-requested Carrier Dome label.
- [National Park Service Niagara Hudson Building](https://home.nps.gov/articles/niagara-hudson-building-ny.htm) and [restoration architect](https://khhpc.com/portfolio-items/national-grid-syracuse-office-complex-a-building-envelope-restoration/): stepped Art Deco facade and Spirit of Light silhouette.
- [Clinton Square](https://www.syr.gov/Living/Our-Community/Parks-Recreation-Youth-Services/Visit-Our-Parks/Clinton-Square): plaza, basin and monument.
- [Onondaga Lake Park](https://onondagacountyparks.com/parks/onondaga-lake-park/): shoreline and park context.
- [Amphitheater seating](https://www.syrvenues.com/p/amphitheater/events--ticketing/general-seating), [builder photographs](https://hb1872.build/project/lakeview-amphitheater/) and [structural engineer](https://jpsllp.com/projects/st-josephs-amphitheater-at-lakeview/): fan-shaped pavilion, trusses, covered seating and lawn.

Unpublished backstage and service-room layouts are reconstructed for gameplay. Palace and War Memorial interiors, and the Dome playing arena, are accessible. Other landmarks and mapped background blocks may use closed or debris-blocked interiors. Background facades do not award discovery XP or continually trigger location alerts.

The final offline extract contains 52,965 street segments, 2,931 street names, 11,433 surveyed building footprints, 16,022 additional playable street-front parcels and 83 fairground halls within the mapped grounds. Runtime clearance checks can reject individual ordinary parcels. Together with the retained regional content, the validated atlas has 67,878 road chords and 28,351 sites.

## New art

`Tools/make_syracuse73.py` builds curved theatre/arena seats, a ticket booth, fluted decorative pilaster and an original winged architectural sculpture. Blender source and FBX files are under `ArtSource/Syracuse73`.

Built-in image generation produced `T_SyracuseAtlas73.png`. Prompt specification: a seamless four-quadrant architectural material atlas for a weathered PS2/PS3-era Syracuse reconstruction: pale limestone with copper Art Deco geometric trim; burgundy theatre velvet with gold floral ornament; pressed tin ceiling; warm red/gold campus sandstone. Flat, orthographic material surfaces, no perspective, lighting gradients, logos or text. Materials select individual quadrants with tiled UVs.

`Tools/import_syracuse73.py` imports the original atlas and five meshes, then creates the architectural and arena materials with instanced-mesh support. `Tools/build_stream_assets68.py` preserves their asynchronous warm-load bundle. No packaged build is produced.

## Validation

Editor Development builds succeed; no packaging was run. `Saved/Syracuse73Verified.log` records all five passing tests: complete atlas placement, routes from Buffalo to the other twenty communities, legacy-save retirement, Syracuse start and landmark geography. The atlas check covers every site, finds zero road-footprint conflicts and confirms that changing the seed does not alter placements.

`Saved/Syracuse73RenderVerified.log` records 26 passing in-engine checks, including construction of the eight built landmark destinations, entrance capsule clearance, floor collision, stairs to upper floors, and the War Memorial club stairs. Final render captures are in `Saved/ScreenshotsV17/Syracuse73_*.png`.

The public plaza at Clinton Square is the new-game spawn. A new game is recommended to see the intended arrival point; existing character saves retain their prior positions. This is a reference-based game reconstruction. Unpublished room plans and infrastructure grades are approximations rather than surveyed replicas.
