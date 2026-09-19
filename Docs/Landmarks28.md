# Landmarks, airports and stamina — update 28

No packaged build is produced by this update. Use the Unreal Editor target.

## Settlement interactions
Residents block the visibility trace with their character capsule. Interaction no longer depends on simple collision in their segmented art meshes. Civilian passers-by retain their intentional one-line-only behavior; hostile or incapacitated residents still cannot converse.

## Stamina
Endurance has two new perks: Conditioning (five ranks, +10 maximum stamina each) and Deep Reserve (three ranks, +25 each, Endurance 5 and level 10 minimum). Each later rank needs another three character levels. Both stack with Iron Lungs. The skill screen paginates categories with more than seven perks. Existing RPG data assets acquire missing new perk and landmark definitions at runtime without replacing authored entries.

## Rarity and world ownership
Casinos now attempt placement in 4% of eligible settlement regions, with no guaranteed casino at the origin and no second parcel-spawn path. Collision checks may reduce actual frequency further.

Airports attempt placement in 0.3% of non-town regions, excluding landmark reservations. Their 300 x 220 metre footprint includes a three-level terminal, west stairs, check-in, baggage, security, gate seating, cafes, duty-free storage, operations, crew rooms, apron, parked aircraft, freight hangar, tower exterior and runway. The tower is an exterior landmark, not a traversable fourth building. The whole airport remains streamed while the player is anywhere inside its footprint.

Each of the twenty unique landmark types has one mathematically assigned region per seed. They cannot be selected by ordinary parcel tables. Reservations take priority over ordinary plots; regional roads detour around their footprints. Placement is independent of chunk load order. Most are several kilometres from the initial shelter. Map discovery still applies.

Existing saved containers and quest progress are preserved. Because regional road/parcel placement has changed, a new world gives the cleanest layout; previously saved vehicles or respawn locations may refer to an old parcel location.

## Unique places
| Landmark | Activity |
|---|---|
| Liberty Last Broadcast | Restore and transmit the final bulletin |
| Mercy Seed Vault | Supply water and restart the seed chamber |
| Atlas Observatory | Align a three-channel observation sequence |
| Presidential Night Train | Recover records across the evacuation train |
| Hoover Memorial Spillway | Restore the turbine control sequence |
| Sunken Treasury | Drain the reserve and release its damaged seal |
| Chapel of the Ashes | Read and perform the memorial bell sequence |
| The Final Picture Show | Restart the projector, survive attracted enemies |
| Coldwater Lighthouse | Supply scrap and relight the beacon controls |
| The Glass Conservatory | Release and defeat a containment specimen |
| Ironjaw Arena | Survive three progressively harder waves |
| Apollo Rocket Garden | Decode military telemetry in the museum |
| Weather Crown | Choose clear skies or a storm for three game hours |
| The Silent Courthouse | Publish evidence for reputation or suppress it for credits |
| Museum of Prosperity | Recover the curator's hidden collection |
| Titan Grave Excavation | Activate a transponder and defeat the titan |
| Black Box Ridge | Recover a crashed courier's manifest and strongbox |
| Neon Mirage Motor Court | Solve the missing guest's room sequence |
| Switchback Radio Telescope | Align three receiver channels |
| Redwood Emergency Ark | Repair the shelter's life-support relay |

Every landmark has physical objective devices, a tracked three-stage journal quest, a guaranteed legendary weapon and supplies in a persistent reserve, 600 base XP, and 1,500 credits (3,000 for suppressing courthouse evidence). Clues display the sequence needed by multi-channel controls. Supply puzzles consume their required items. The reserve cannot be accessed before completing the activity. Completion, puzzle progress, killed challenge enemies, outcome choices and looted inventories survive streaming and saves; rewards do not refill.

## Validation
Editor compilation succeeded (`Saved/Build28d.log`). All 59 automation tests passed, including 80 landmark reservations across four seeds, road/parcel clearance, rarity sampling and stamina rank gates. The rendered scenario checks normal resident focus/dialogue, all twenty activities, challenge enemy spawning, one-time rewards, controller recreation, three airport decks and every stair tread. Run `Tools/VerifyLandmarks28.ps1` to repeat the checks. No packaging command is used.

Final rendered run: **235 checks passed, zero failures**, recorded in `Saved/Landmarks28_FinalRuntime.log`. The airport apron and selected landmark screenshots are in `Saved/ScreenshotsV17/`.
