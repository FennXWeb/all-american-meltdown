# Destiny USA reconstruction

This replaces the rectangular placeholder at the existing Syracuse landmark. The
site ID and Syracuse location are unchanged; its reserved parcel is enlarged to
accommodate the mapped building, pedestrian entrances and parking approaches.

## Reference basis

- [Official directory PDF](https://www.destinyusa.com/uploads/documents/Directory.pdf):
  Commons, levels 1–3, branching corridors, tenant-block outlines, Canyon,
  Carousel Court, escalator banks and Hiawatha pedestrian bridge. The available
  diagram is marked v0114. Store names and contents in the game are fictional.
- [OpenStreetMap building way 108262911](https://www.openstreetmap.org/way/108262911):
  74-point exterior outline, approximately 628 × 362 metres along geographic axes.
  [Bridge way 249375762](https://www.openstreetmap.org/way/249375762) and mapped
  access roads/parking aisles retain their relative geographic positions.
- [HKK Architects project photographs](https://hkkarchitects.com/portfolio-item/destiny-usa/):
  Canyon timber trees and fabric petals, metal ceiling portals, exposed framing,
  glazing, balcony treatment and the original mall's interior finishes.
- [Pyramid's aerial photograph](https://www.pyramidmg.com/property/destiny-usa/):
  roof masses, facade treatment, entry glazing and the bridge relationship.

The outside footprint is map-derived. **The interior directory is a schematic,
not a measured construction drawing.** It is registered to the exterior using
piecewise affine corner correspondences. Heights, structural spacing, shop
subdivisions, service areas and furniture are interpretations of the public
references. This is not a survey-exact replica or a claim that the 2014 tenant
plan is the current as-built layout. More recent measured plans would allow those
details to be matched precisely.

## Implementation

`LWDestinyData71.inl` stores offline-triangulated floor/roof panels, openings,
retail blocks, entrances, stair locations and mapped access roads. The game does
not download maps, read PDFs, run geometry unions or triangulate these plans while
streaming. `LWDestiny71.cpp` builds the material-batched shell and instanced
furnishings through the existing incremental construction queue. Furniture stock
has persistent IDs and existing loot interactions; seating is usable.

The original shopping wing and its Commons parking undercroft, upper cinema,
multi-level expansion, Canyon, carousel projection and glazed bridge are distinct
parts of the building, rather than repeated rectangular floors.

## Regenerating the data

The checked-in generated C++ data is sufficient to build the game. To regenerate:

1. Download the official directory above into `Saved/References71/Directory.pdf`.
2. Download the OSM XML API response for
   `https://www.openstreetmap.org/api/0.6/map?bbox=-76.178,43.067,-76.164,43.075`
   into `Saved/References71/site.osm`.
3. Install Python dependencies: PyMuPDF, Pillow, NumPy and Shapely 2.1 or newer.
   The script also searches `Saved/Tools71` for workspace-local dependencies.
4. Run `python Tools/build_destiny71.py` from the project root.
5. Review `Saved/References71/ReconstructionPlan.png` and run
   `LethalWorld.Update71` plus the Update69 atlas/route regression tests.
6. The unpackaged `-LWV17Smoke -LWUI46Smoke -LWDestiny71Smoke` game run checks
   construction, stair clearance and interaction population, then captures views.

Reference photographs are research inputs, not redistributed game textures.

Reference snapshot SHA-256 values (21 September 2026):

- Directory.pdf: `61059c88e164a68ffa9b04acad7471a6920c39dd2b84a9cc17292e321b8ad0cb`
- site.osm: `59e10b4c534096c75b5fbd53b48840cf0d72d514b9f3ac3a4e75204fd6538b70`

Saved stock from the retired `ny69_<site>_...` containers is moved into the new
store containers, preserving item IDs, contents, grid size and locks. Migration
markers prevent duplication on subsequent visits.

## Verification

- Unreal 5.8 Development Editor build succeeded (`Saved/Build71g.log`).
- Update69 Atlas, Update69 Routes and Update71 DestinyLayout: all passed
  (`Saved/Destiny71AutomationFinal.log`).
- Unpackaged walkthrough: 6/6 checks passed, including collision/headroom on both
  sides of all 11 escalator banks and legacy container migration
  (`Saved/Destiny71SmokeFinalG.log`).
- Five rendered views captured in `Saved/ScreenshotsV17/Destiny71_0.png` through
  `Destiny71_4.png` and inspected during development.
- User video settings restored after testing. No packaged build produced.

## Map attribution

Map-derived outline, bridge and road data © OpenStreetMap contributors, available
under the [Open Database License](https://opendatacommons.org/licenses/odbl/1-0/).
See [OpenStreetMap copyright and attribution](https://www.openstreetmap.org/copyright).
The generated data and regeneration script retain the source identification.
