# Roads and graphics, revision 33

## Player-facing controls

Settings > Advanced Graphics has two pages. Quality controls cover view distance, textures, shadows, effects, foliage, post processing, global illumination, reflections, shading, anti-aliasing quality, landscape quality, and overall presets. Display/effects controls expose render scale (50–100%), frame cap (unlimited through 240 FPS), VSync, FXAA/TAA/TSR or no AA, motion blur, bloom, ambient occlusion, anisotropic filtering, sharpening, and the existing performance/Lumen/hardware-ray-tracing modes. Hardware mode is only offered when supported. Balanced defaults are available. Changes apply immediately and persist; display resolution/window mode retain the existing 15-second confirmation and rollback.

Old system-level view distance, shadow resolution, texture pool and frame-cap overrides were removed so scalability settings can work. Explicit render scale/effects selections override the old fixed project values. Camera decay remains separately controllable.

## Road generation and rendering

The deterministic regional road graph remains the shared source for terrain, POI placement, mapping and vehicle navigation. Regional curves now use 24 sampled segments with parcel junctions attached to the actual sampled centerline. Highways vary between four and six lanes (1680/2440cm paved widths), consistent along each corridor; streets and narrow access lanes retain their own widths.

Each chunk clips roadway capsules to its exact XY bounds. Convex subtraction removes overlaps before triangulation, so each pavement/shoulder layer has one surface at intersections and curved joins. Geometry belongs to the chunk it covers instead of the chunk containing a long segment's midpoint. This avoids missing road spans when neighbors stream out and overlapping pavement from adjacent chunks. Top surfaces have collision; road shoulders follow the same clipped construction.

Junctions are detected at real centerline intersections, including T junctions and segment-interior crossings. Ordinary curve seams do not become intersections. Close crossings are treated as one protected area. Lane and edge paint stops before junctions. Major near-orthogonal junctions receive signals; local and irregular junctions receive stop signs. Signal cycles alternate 28 seconds of green, four seconds of amber and two seconds of all-red clearance per axis. Opposing approaches share a phase. Phase offsets derive from seed and position, and continue consistently when chunks reload.

Roadside content includes stop bars, zebra crossings, octagonal stop plates, mast-mounted signal heads with three emissive lenses, speed and route signs, curve warnings, guardrails with reflectors, streetlights, utility poles, sidewalks, storm drains, and occasional roadside wrecks. Placement avoids junction mouths, other road surfaces and building footprints. These are runtime mesh assemblies and instanced geometry, not separate heavyweight blueprint actors for every prop.

Companion drivers and convoy cars brake for signals and perform a 1.5-second stop at stop-controlled approaches. Player-driven vehicles remain under player control. Signals do not introduce a new civilian traffic population. Irregular intersections use all-way stops rather than assigning unsafe perpendicular signal phases.

## POI clearance

The complete gathered road set is checked against all final POIs, including access roads added by adjacent regions after initial parcel selection. Road clearance includes a 100cm margin, with access endpoints 400cm beyond the reserved building footprint so wide driveways remain valid. Settlement buildings use the final neighboring-road checks, and perimeter fences leave gaps wherever any roadway crosses. The casino's declared plot now covers its entire parking forecourt (18800cm deep), which previously extended beyond its reserved parcel. Existing seed layouts can change when regenerated; live chunks need to reload before geometry updates appear.

## Source

- `LWGraphics33.cpp/.h`: settings persistence, renderer application, labels.
- `LWRoads33.cpp/.h`: lane profiles, junction identification, signal timing, driver stopping.
- `LWRoadBuilding33.cpp`: clipped/merged road geometry and roadside assemblies.
- `LWGeneration.h`: sampled routes and final neighboring-road checks.
- `Tools/import_roads33.py`: eight sign/signal materials under `/Game/Materials/M_*33`.
- `LWRoad33Tests.cpp`: signal safety, topology, lane-count and multi-seed clearance tests.
- `LWRoad33Smoke.inl`: graphics screens, live setting changes, two-chunk junction fixture and signal material checks.

## Validation

- Editor Development builds passed; final vehicle-ID build recorded in `Saved/BuildRoads33f.log`.
- Eight materials imported and saved (`Saved/Road33_Import.log`).
- Full regression suite: **72 passed, zero failed, zero warning results** (`Saved/Road33FinalReport/index.json`, `Saved/Road33_AutomationFinal.log`). This includes all POI categories, rare resorts, dungeon reservations, long/negative-coordinate routes, and signal conflicts.
- Offscreen runtime: **40 checks passed, zero failures** (`Saved/Road33_RuntimeFinal.log`). Checked live graphics changes and restoration, menu labels, two-chunk road surfaces, exactly four signal heads at a boundary junction, and valid lens materials.
- The existing interior-crossing test now chooses an approach point within the sampled segment, since denser samples shortened it; it still checks the exact crossing, on-road travel and shortest route length.
- Reviewed the graphics pages, junction overview and forward-facing illuminated signal captures in `Saved/ScreenshotsV17/Road33_*.png`.

No packaged build was produced.
