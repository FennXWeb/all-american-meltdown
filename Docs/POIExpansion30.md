# POI expansion integration

## Ownership and IDs
Only POI/generation/dungeon files and new expansion files were edited. No character, HUD, discovery, resident, vehicle, World header, or site-identity helper edits.

IDs are append-only: 58 shopping mall, 59 prison, 60 church, 61 car dealership, 62 hardware store, 63 theme park. LWPlaces::Count is 64; GeneralCount remains 19 because the legacy compact parcel selection is unchanged. New entrance signs call LWSites::Label(S) from the separately owned LWSiteIdentity.h.

## Layouts
- Mall: 180m x 150m campus, twelve specialty stores, two anchor halls (department store and seated cinema), axial concourse, food court, seating/planters and rear delivery/workshop rooms.
- Prison: walled perimeter/intake, 24 furnished cells in two wings, exercise court, mess hall and infirmary/warden storage.
- Church: central aisle and sixteen pew banks, high sanctuary, altar/cross, colored window panels, bell tower and separate rear relief vestry.
- Dealership: three display cars, finance desks, three service buildings with cars/workbenches and a striped outdoor inventory lot. Uses existing vehicle spawning/persistence.
- Hardware: six labeled stock aisles, checkout lanes, rear stock/workshop partition and outdoor lumber stacks.
- Park: ticket booths, six midway concession/game buildings, picnic corridor, static carousel, modeled Ferris wheel with rim/spokes/gondolas and ride engineering building. Rides are scenery, not operational vehicles.

## Generation and persistence
A separate 30000/30001/30002 hash family attempts one additional roadside site in 55% of non-origin, non-unique regions. Placement can fail when no safe parcel fits. Roads, original site IDs, original PRNG sequences and dungeon enemy IDs remain unchanged. Large additions stay inside their owning region, reject local and neighboring road/parcel collisions, and receive checked access roads. Site IDs own deterministic prop and NPC keys. Existing type IDs and save structures are unchanged; existing worlds can gain these additional sites on regeneration.

Shared LWPOISurvivors uses a separate 30020 hash to select a 28% survivor-stop chance at expanded POIs and eligible existing normal POIs, with merchant/recruit/warden roles providing existing trading, hiring and contract dialogue. Hired or already-spawned residents are not duplicated. Local generic hostiles are suppressed at these stops; this is not an invulnerable safe zone. Existing cooked enemy tables without rows 58–63 fall back to prison/police or depot combat rows using the original new-site ID and transform.

Every dungeon's first final-room warden is forced to three legendary stars with an additional 1.6x health multiplier (on top of existing 1.55x warden and 4.75x legendary health). Existing star damage/resistance and corpse loot apply. The final reserve remains gated by all controls and wardens. End sentries use central positions away from corner furniture, retaining their original persistent IDs and killed-state behavior. Previously defeated wardens remain defeated.

## Validation and remaining integration
No compile, package, editor launch or runtime tests were performed, as requested. Source-level validation checked all 17 literal mesh references against existing assets. Added LethalWorld.Generation.ExpandedPOIReservations automation coverage for natural appearance of all six types, entrances, level foundations, and local/neighbor road and parcel collisions; it has not been run. Run it with existing generation/dungeon tests after agents finish integration, then visually inspect all six layouts and guardian encounters in editor.

Map/discovery owner: atlas IDs already agree; new geometry consumes your LWSites::Name helper. No additional character/HUD hooks are needed for NPC interactions. Dedicated designer spawn rows 58–63 can replace the compatible fallback later. Optional custom site-specific missions require RPG catalog work; current couriers offer the existing contract catalog. Runtime geometry, vehicle clearance and placement frequency still need editor validation.

## Integration follow-up, September 9, 2026
The earlier validation section describes the delegated source-only stage. Integration subsequently added dedicated spawn rows for 58–63 and imported them. All 67 project automation tests passed, including expanded parcel coverage. All 133 rendered POI checks passed (`Saved/World30_POI30.log`), covering actual survivor dialogue and guardian gating. Additional sites require more than one existing filtered site in their region to preserve sparse countryside. Survivor placement now runs after collision geometry completion. See Update30.md for the integrated results.
