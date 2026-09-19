# Build 0.4 validation

Environment: Windows, Unreal Engine 5.8.1, Visual Studio 2022 / MSVC 14.44. Tests run on September 6, 2026.

## Completed checks

- Editor C++ build: succeeded.
- Unreal content import: seven new furniture meshes, three speech sources, RPG data asset; `LW_V4_CONTENT_COMPLETE`.
- Native automation: **27 tests passed**, including XP thresholds, multiple-level awards, point conservation, rank/attribute gates, companion capacity, catalog integrity and acyclic quest prerequisites, plus the existing inventory, magazine, loot, world and road-navigation tests.
- New-feature editor gameplay: **123 checks passed, zero failures**. `Saved/GameplayV4.log` ends with `LW_V4_DONE failures=0 checks=123`.

The gameplay suite exercises actual HUD purchase clicks, branching dialogue selection, prerequisite and duplicate quest rejection, atomic supply hand-ins, one-time rewards, journal routing, merchant inventory, medic payment, passing subtitles, recruitment cost/capacity, reserve followers returning home, skill-unlocked followers rejoining, ten bedrooms, protected-room floors, bed/water interactions, cooldowns, locker deposits and retrieval, door collision/opening, radio quest progress, and save/load of progress, gear and crew.

Live combat checks verify companion visibility traces damage a hostile, companion kills credit the player and advance a quest, downed companions recover, and player death drops equipment while preserving progression and recruited survivors. The recovery-duration test shortens the timer after verifying the initial downed state; it tests the timer transition rather than waiting through a complete 30-second interval.

The initial gameplay run exposed a test fixture opening a locker beyond the real 550 cm access limit. The fixture now approaches the locker and performs real inventory transfers. Runtime proximity enforcement remains intact.

## Render review

Twenty-one engine screenshots are retained under `Docs/ScreenshotsV4`: eleven new-feature views (waystation, skills, contract conversation, journal, merchant grid, passing speech, crew panel, bunker hall, bedroom, workshop and companion combat), plus interior/exterior views of all five POI types. Screenshots were inspected for text fit, visible models, interior lighting and door coverage. These are engine captures, not concept images.

## Release checks

- Existing survival, settings, inventory, magazine, weapon, trading, acoustic, ragdoll and death-recovery suite: **504 checks passed**, zero failures (`Saved/RegressionV2.log`). Its stash fixture now approaches the stash's new position; distance-denial tests still enforce the original 550 cm access limit.
- Existing POI, door, sign, window, exploding-pump, vehicle and enemy suite: **180 checks passed**, zero failures (`Saved/RegressionV3.log`). Includes rendered views of all five POI interiors and exteriors.
- Windows build/cook/stage/archive: **BUILD SUCCESSFUL**, cook completed with zero errors and warnings (`Saved/PackageV4.log`). Final executable refresh is logged in `Saved/PackageV4Final.log`.
- Final exported Windows executable, including the perk-aware inventory bar/medkit-label correction: **123 new-feature checks passed**, zero failures, process exit code 0 (`Saved/PackagedGameplayV4.log`, completed 10:23 AM local time).

## Limits

These are automated deterministic fixtures plus screenshot inspection, not a long-form campaign playtest. Normal simulation is active for the designated combat enemy and companions; unrelated procedural enemies are suppressed during gameplay fixtures. Smoke saves are isolated from the player's `LethalWorld_Survivor` save. See `Build04.md` for prototype scope and authoring details.
