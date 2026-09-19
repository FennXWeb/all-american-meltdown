# Update 18 validation

Validated September 7, 2026 with Unreal Engine 5.8.1 on Windows. No cooking, staging or packaging was performed.

## Results

| Check | Result | Evidence |
| --- | --- | --- |
| Development Editor C++ build | Succeeded | `Saved/Build18m.stdout.log` |
| New mesh import | 35 meshes; bounds and material assignments valid | `Saved/Content18c.log` |
| Existing automated tests | 45 passed | `Saved/Automation18.log` |
| Update 18 rendered gameplay | 90 checks passed, zero failures | `Saved/Gameplay18final.log` |
| Dialogue/menu attack suppression | 20 checks passed, zero failures | `Saved/UIClick18.log` |
| Vehicle/equipment/spawn regression | 266 checks passed, zero failures | `Saved/Regression18_V14final.log` |

The 376 rendered checks cover the two-barrel firing sequence, spent-shell ejection, interrupted and partial reloads, audio slot resolution, unique settlement names, hostility expiry and permanent minimum reputation, membership and leadership, shop escrow and premium proceeds, passive income, actual theft and merchant transfers, ten-companion progression, vehicle fire and wreck state, bed and relocated RV respawns, convoy boarding/hotwiring/motion/disembarking/loss-of-contact recovery, crew stationing, save serialization, creature assets, titan height, usable POI storage/doors/windows and lock persistence. The vehicle regression also checks existing equipment attachments, every vehicle seat, cabin interactions, chauffeur travel and road waypoint arrival.

Runtime screenshots of the new gun, five creatures and three rebuilt POIs were inspected in `Saved/ScreenshotsV17`. The new suite reuses the existing V17 capture runner and its isolated `AllAmericanMeltdown_AutomationV17` save slot. The older regression uses `LethalWorld_AutomationV14`; normal survivor saves are not test fixtures.

## Reproduction

Run `Tools/Verify18.ps1` after compiling the editor target. It runs the 45 automated tests and all three rendered suites. `Tools/Build.ps1 -Content` includes the additive Update 18 importer. Packaging remains opt-in for the user.

The initial test passes identified and resolved a world initialization ordering issue, repeated cabinet relocking, a convoy test route blocked by its own parked RV, outdated bed-menu/enemy-weight fixture assumptions, and frame-hitch-dependent timing in the reload interruption test. Final rendered reruns passed after these corrections. Automation and UI-click checks passed before the final convoy recovery and test-fixture changes; those changes were then compiled and exercised by both final rendered suites.

These checks validate bounded gameplay scenarios, not exhaustive travel across every procedural seed. Convoys use road routing and local collision braking; cars that lose contact release their crews to the bunker. New audio effects are replaceable generated placeholders in the audio catalog.
