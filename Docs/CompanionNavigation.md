# Companion navigation

## Final stacked-floor fix and verification

The final editor-only build succeeded (`Saved/BuildNavStackCorner.log`). The rendered suite then completed **39/39 checks passing**, including stacked-floor routing, descent, fallback, blocked LOS music, different-floor music, and friendly obstruction (`Saved/NavStackCornerTrace.log`, `AAM_V17_DONE failures=0 checks=39`). The stacked case retained its original 18-second limit and finished at X=20898.216, Y=19999.986, Z=5490.047.

The trace exposed a movement issue beyond search timing: accepting a waypoint within 40 cm could cut the next corner before the capsule had cleared a stair wall. That invalid shortcut caused a stop and unnecessary replan of an otherwise valid route. Waypoint advancement now checks the walkable segment from the actual capsule position to the following waypoint before skipping the corner. With the fix, the stacked run retained its initial 711-node route throughout movement with no logged blocked or stalled interval and reached the landing before the unchanged deadline. Walking speed and the test assertion were not relaxed.

Per-second navigation diagnostics require `-LWCompanionNavDebug`; they are off by default. An additional 48-fps confirmation run was stopped at the user's request after the full 39/39 pass; it is not counted as validation. Sources are finalized. No packaging was performed.

## World30 runtime follow-up

`Saved/World30_CompanionNav.log` completed with 38 of 39 checks passing. All music transitions, peaceful-resident targeting, friendly obstruction, hostile damage, ordinary stairs, descent, fallback, doors and obstacle recovery passed. The stacked-floor route failed its unchanged 18-second assertion.

Source review found that the four-second search cutoff discarded the frontier when no partial route improved on the origin. Directly below a goal, reaching stairs may first require moving farther away, so restarting can repeatedly explore the same region. The follow-up fix retains that frontier until the search completes, exhausts its open set, or reaches its existing node cap; useful partial routes can still advance after four seconds. The existing per-frame collision budget and 6,500-node cap remain. Vertical fixtures now log final positions and clear velocity after relocating the actor. This follow-up has not yet been compiled or rerun; the passing runtime results above apply to the preceding binary.

The follow-up also replaces tick-order search admission with a persistent FIFO of weak resident requests. A late-ticking search retains priority into the next frame rather than losing every 3 ms slice to earlier actors. Served searches rejoin behind pending requests on their next call; inactive requests expire after two frames. The 96-node frame allowance now charges actual processed nodes rather than reserving 32 per caller even when time prevented that work. A standalone scheduling model checked equal service for ten callers in fixed and reverse tick order under one-service-per-frame saturation, plus continued service after a caller stops requesting. Source delimiter checks passed; this model does not substitute for the pending Unreal rebuild and stacked-floor rerun. No headers changed in this follow-up.

## September 9, 2026 source update (not compiled or run)

Follow-up review fixed a vertical-route search restart: the planner now remembers the requested goal separately from its projected/fallback destination. A formation point over a stairwell or outside an upper landing can therefore fall back to the player's floor without resetting its incremental search every frame. Floor-height keys, sampled walkable edges and endpoint-height checks remain responsible for stair traversal; the music-only 1.8 m threshold is not used to restrict navigation. Peaceful/downed resident exclusion is also enforced inside the shared engagement predicate, in addition to companion target selection and cached-target checks.

Two new body includes extend `BuildCompanionNavSmoke`: `LWCompanionVerticalSmoke.inl` runs after the existing ascending-stair test, and `LWCompanionAISmoke.inl` runs after navigation cleanup. No game-mode insertion is needed: `LWGameMode.cpp` already includes `LWCompanionNavSmoke.inl`, and the existing `-LWV17Smoke -LWCompanionNavSmoke -LWAudioSmoke` flags select the expanded suite. Do not include these body fragments at global scope.

The vertical cases require routing from directly below an upper destination, descending stairs, and climbing to the player when the formation point is over a void. The AI/music cases observe actual `MusicState` transitions through visible engagement, short occlusion, sustained wall occlusion, restored LOS, a different elevation with clear LOS, same-floor return, and unrelated alerts. Damage assertions cover cached peaceful targets, peaceful candidates with alert/aggression flags, a friendly blocking the shot, and a successful hostile shot after that friendly moves aside. An alerted peaceful resident must also leave music in exploration. These fixtures have been source-reviewed but have not been compiled or executed pending coordination with other agents.

Navigation now keeps a separate blocked-path timer so collision rejection cannot erase recovery progress on the next tick. Progress is measured over 0.35-second windows to avoid false stalls at high frame rates. Searches can restart for moving or vertically relocated destinations, and door lookahead stops at the current waypoint to avoid reacting to an obstruction beyond a turn. Recent manual door closures are respected during planning; already-open manual doors remain usable.

Following companions scan threats on a staggered interval, retain a preferred target, prioritize enemies near the player, and verify the shot hits that enemy before applying damage. While close to the player they hold firing distance or retreat from enemies within 5 m using the walking planner. Beyond 8.5 m from the player, or while approaching a player vehicle, normal following takes priority. Settlement defenders also reject unrelated alerts. New logic lives in `LWCompanionThreat.cpp` and `LWThreatAwareness.h`.

Combat music now requires a living, alerted enemy focused near the player, within 35 m, with visual contact and feet within 1.8 m vertically. Walls and floor separation therefore exclude unrelated threats. Hostile, standing residents qualify; friendly residents and passive mannequins do not. A 1.5-second hold prevents rapid switching during brief occlusion and clears immediately in menus, the safehouse, or on player death. The floor threshold intentionally excludes threats substantially above or below the player, even across an open atrium.

Added smoke assertions for visible engagement, unrelated noise, another floor, wall occlusion, passive/aggressive mannequins, and dead enemies. Existing door, stairs, obstruction and following fixtures remain. This update received source inspection and delimiter checks only. No compilation, runtime smoke execution, cooking or packaging was performed; the older validation below does not validate these changes. Vehicle/convoy sources, `LWSocial.cpp`, the shared character header and HUD were not edited.

Companions and settlement residents now use a dedicated, incremental walking planner instead of the enemies' flat obstacle search. It samples real collision floors, tracks different elevations, tests headroom and step height, and routes around furniture, walls and unsafe drops. Cached waypoints are reused until the destination moves, the route is blocked, or progress stalls. Long searches can advance along a safe partial route and continue from there.

Closed, usable POI doors are treated as passage options while planning. A nearby companion opens the door and waits for its swing before crossing. Doors opened by companions close afterward, with a doorway/swing-area check for the player, other NPCs and downed crew. The original open state is respected: companions leave already-open doors open. Locked doors are not unlocked or bypassed. Manual door use takes priority for a short interval, and open/closed state uses the existing save records.

Movement includes local separation and sidestepping around other residents. Planning is bounded across the entire group each frame, rather than performing an unrestricted search for each of ten companions. Routes are checked against current collision before applying movement, including after chunks or props change.

This applies to walking and following, moving toward vehicles, settlement routines and local bunker movement. Existing bunker entrance/exit handoffs and vehicle driving remain separate systems. NPCs do not pick locks, destroy doors, jump gaps or teleport through ordinary building obstacles.

Sources: `LWCompanionNavigation.cpp`, `LWCompanionDoors.cpp`, and the integration in `LWSocial.cpp`. Regression fixture: `LWCompanionNavSmoke.inl`, invoked using `-LWV17Smoke -LWCompanionNavSmoke -LWAudioSmoke` with the existing isolated automation save.

## Validation

Validated in Unreal Editor on September 7, 2026. Editor compilation succeeded (`Saved/BuildNav19d.stdout.log`). `Tools/VerifyCompanionNavigation.ps1` passed all three rendered suites:

- 12 navigation checks: offset doorway routing and physical passage, saved door state, closing after passage, holding for the player, preserving an already-open door, respecting a locked door, climbing stairs to an upper landing, avoiding an unreachable drop, rerouting around newly placed furniture, and active companion following after the player relocates.
- 90 existing gameplay checks, including convoy boarding, movement, disembarking and lost-contact recovery.
- 20 menu/dialogue input checks to preserve attack suppression while using UI.

Logs: `Saved/Navigation19_CompanionNav.log`, `Saved/Navigation19_V18.log`, and `Saved/Navigation19_UIClick.log`. No cooking or packaging was performed.

The planner uses 70 cm grid spacing with multiple height layers, 30 cm floor samples along edges, capsule/headroom checks, and the character's actual step-height and floor-slope limits. A shared search budget allows at most 96 expanded nodes and approximately 3 ms of search work per frame (plus the current bounded expansion). Searches are bounded to 6,500 nodes and a 65 m local radius, with safe partial-route progress and retries for longer or obstructed journeys. Dynamic collision is checked again during movement. These are bounded collision-based routes; they do not require a prebuilt navigation mesh for the procedural world.
