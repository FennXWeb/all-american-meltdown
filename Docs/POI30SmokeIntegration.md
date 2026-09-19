# POI30 rendered smoke integration (unrun)

Owner edits: new `LWPOI30Smoke.inl` only for harness; no GameMode or V17 dispatch files modified. Coordinate these three independent narrow insertions with the vehicle fixture owner; retain both branches and run fixtures separately.

1. In `LWGameMode.h`, beside the other private Build...Smoke declarations:

```cpp
void BuildPOI30Smoke(class ALWCharacter& Player);
```

2. In `LWGameMode.cpp`, in the smoke include block after `FLWV2SmokeState` and before `LWV17Smoke.inl`:

```cpp
#include "LWPOI30Smoke.inl"
```

3. At the start of `BuildV17Smoke` in `LWV17Smoke.inl`, alongside the vehicle agent's independent dispatch branch:

```cpp
if(FParse::Param(FCommandLine::Get(),TEXT("LWPOI30Smoke"))){BuildPOI30Smoke(Initial);return;}
```

Run the existing rendered harness with BOTH `-LWV17Smoke -LWPOI30Smoke`, using its normal isolated automation save. Do not combine with the vehicle fixture selector. No StartPlay, Tick, screenshot-directory or watchdog changes needed. Do not use NullRHI. Cold shader compilation may exceed the existing 210-second overall watchdog; warm shaders before the validation run.

Expected output: 20 nonempty PNGs under `Saved/ScreenshotsV17`: `POI30_58_Exterior/Interior` through `POI30_63_Exterior/Interior`, `POI30_1_Exterior`, `POI30_4_Exterior`, `POI30_13_Exterior`, `POI30_Survivor_Trade`, `POI30_Survivor_Recruit`, `POI30_Survivor_Missions`, `POI30_Guardian_EndRoom`, `POI30_Guardian_ReserveUnlocked`. Logs include POI30_LAYOUT, POI30_GUARDIAN and POI30_DONE plus standard harness checks. A nonzero failure count or a missing screenshot is failure.

Coverage: actual geometry dispatch for six new layouts plus existing motel/diner/depot; seeded production survivor selection, three distinct generated names, no generic hostile actors, storage presence and short new entrance label, floor beneath six interior viewpoints, real trader stock opening, paid recruitment and duplicate-hire rejection, real courier contract-menu availability. Guardian coverage builds dungeon24 through production code and spawns its lead warden through the director, validates three stars and 8246 max HP / 2.05x damage, checks live-guardian reward denial and synthetic defeated-state release/payment idempotence.

Limits: other dungeon guards are marked defeated in the isolated smoke save to make the end guardian observable; this does not test fighting, actual death drops, or weapon balance. Couriers' mission menus are tested; mission acceptance/completion is not. Views use frozen movement at an elevated isolated fixture, not a walk-through/navigation test. Natural generation coverage remains in `LWPOIExpansionTests.cpp`. All fixtures are uncompiled and unrun; screenshot files will only be created when the owner runs the rendered harness.

Physical new entrance signs now use LWSites::Label, not Name/address. Shared `LWPOISurvivors` handles optional casts for new types and existing 0/1/3/4/10/11/12/13/14/16. Existing settlements, friendly authored hubs, boutique, dungeons, towers, and other special sites are excluded. A local friendly copy suppresses the existing generic enemy call for normal selected sites without changing saved parcel identity. NPC keys retain their poi30 site/role format. Proper names derive from site/role using first names, surnames and middle initials, independent of loading order; no settlement roster is manufactured just to name roadside residents. Common names can recur in a large world, but the fixed three-name cast is gone and each site's roles have different first names. Existing authored casts within a site's footprint take precedence.

Dedicated spawn rows 58–63 supplied by the integration owner take precedence automatically. The absent-row fallback remains; no SpawnTable or gas-can changes made here.


## Rendered retry after first runtime failure
The first run (`Saved/World30_POI30.log`) passed the mall enclosure/entrance checks but built zero survivors, then aborted at the first interaction prerequisite. The retry builds each POI at a distinct remote address before lifting it, avoiding the loaded terrain at the former shared ground-level construction site. Ground collision is a suspected cause rather than a confirmed one; new POI30_BUILD and POI30_SURVIVOR_SPAWN/SKIP/BLOCKED logs distinguish selection, authored cast, hired/existing identity and placement failures. Production spawning tries six deterministic nearby points using AdjustIfPossibleButDontSpawnIfColliding, never forced overlap. Prerequisite failures now record errors and skip only their action, allowing the full gallery to render.

Mall retry also faces all twelve shop openings inward, orients side-wall signs toward the concourse, adds a front directory/information kiosk and welcome seating/planters outside the central aisle, and adds lower pendant illumination. The fixture sweeps every storefront opening and the welcome aisle. The information kiosk is decorative, preserving existing shop/prop identity ordering.

After setup, a two-second map step marks actual LWGen::Gather sites known with LWSites::Name, opens bMap and captures Map30_Icons. The next step closes panels. This extra Map30-prefixed screenshot leaves the POI30 count at twenty (twenty-one total including the map).

Retry changes are source-only and require the owner's compile/rendered rerun. No retry success is claimed here.


Final spawn correction: normal builders now defer survivor creation until geometry completion; the helper publishes pending slab collision, traces a walkable floor, rejects roofs/elevated obstructions and tests the actual resident capsule before DontSpawnIfColliding. No forced spawn or ignored geometry. The remote rendered fixture provides a flat supporting terrain slab beneath its exterior apron, then lifts the stage and actors together. This supersedes the earlier adjust-if-possible retry description; six deterministic samples remain.
