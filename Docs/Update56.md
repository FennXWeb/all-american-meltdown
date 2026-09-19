# Update 56 — controls, widescreen menus, subtitles and RV cockpit

Main and pause menus use the full viewport width, including animated background and dimming. Fixed-layout tool screens retain a centered safe frame.

Default controls: Q/E lean, C toggles crouch, sprint+C slides, hold C for 0.4 seconds to prone, F interacts (or exits a vehicle when no control is targeted), H flashlight/headlights, M map. Left Ctrl heals; P remains an alternate prone toggle. Settings reset restores these defaults. Existing bindings migrate once through a collision-free permutation of the affected physical keys; unrelated bindings stay intact. Map key opens the map from player tabs and closes it when already open.

Nearby NPC speech appears in a bottom-centered caption with speaker name, wrapping and fade. Nearby obstructed speech remains captioned; distant speech through walls is excluded. Full conversation and story cutscene subtitles remain in their existing screens.

Dropped bags use visibility-query collision only: still selectable, but no pawn/vehicle blocking, physics blocking, overlap events, or navigation obstruction. Applied in common configuration, including restored bags.

RV: the bonnet now starts outside the cockpit rather than cutting through it; cabin floor extends forward to the bulkhead. Enclosed wheel tubs cover the inboard tyre faces and tops. Runtime axle locations match the authored wheel arches. Source corrections live in make_vehicle42_models.py; make_rv56.py produces the focused two-mesh update and import_rv56.py imports it.

Validation: Editor compilation succeeded (Saved/Build56Complete.log). Focused live gameplay/input suite: 17/17 passed at 2560x1080 (Saved/Update56Final.log). Menu/dialogue click safety regression: 20/20 passed at 1280x720 (Saved/Update56Click.log). Imported mesh bounds and material assignments validated. Main menu, pause menu, subtitles, and corrected RV cockpit screenshots inspected in Saved/ScreenshotsV17/UI56_*.png. No packaging performed.
