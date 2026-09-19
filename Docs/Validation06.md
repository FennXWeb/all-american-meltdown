# Build 0.6 validation

Windows, Unreal Engine 5.8.1, MSVC 14.44. September 6, 2026.

## Completed editor checks

- Editor C++ build succeeded (`Saved/BuildV6.stdout.log`).
- **31 automation tests passed** (`Saved/AutomationV6.log`), including the existing inventory, ammunition, navigation, generation, progression and security suites.
- New inventory tests verify round conservation, stack merging, distinct pick durability, valid sorted footprints, linked-magazine transfer, rejection of independent loaded-magazine transfers, and unchanged source contents when the target cannot fit an item.
- New generation coverage samples road footprints against building foundations over 147 seed/region combinations and confirms that the world remains populated.
- **168 live gameplay checks passed**, zero failures (`Saved/GameplayV6.log`). This includes the prior vehicle/security suite plus exact equipment-slot selection, empty slots, two instances of the same weapon type, real Shift+click input, storage merging, sorting, shared tab navigation and retained container access, ambient residents moving between activities, non-interactive civilian dialogue, three distinct passenger seats, disabled passenger/chassis collision, a full car driving with real W-key input, overflow companions returning home, detachment on exit, loose follow targets, and persistence of inactive companions.

Three older navigation fixtures initially failed because they treated building centers as paved road endpoints or classified any driveway without a matching center as the bunker driveway. The fixtures now target the actual external road entrances and identify the bunker explicitly. Their road-following, connectivity and T-junction assertions remain enabled. No navigation-runtime assertions were weakened.

The overflow test advances the eight-second separation timer directly after placing the companion beyond 120 metres; it verifies the transition without waiting the full duration. The inherited security tests use known puzzle solutions and controlled timing phases. These tests complement, rather than replace, extended manual playtesting.

## Visual review

Engine screenshots cover the tabbed inventory and skills screens, settlement courtyard, passenger seat and inherited driving/security scenes. Review found and corrected faint old header text beneath the tabs, duplicate close controls, and missing inventory credit visibility. The original feedback labels and inventory occupancy summaries were removed.

## Packaged release

The Windows Development build was compiled, cooked, staged and archived successfully with Unreal Automation Tool (`Saved/PackageV6.log`, exit code 0). The release is available at `Builds/Windows/LethalWorld.exe`.

Exported-game suites run offscreen with real rendering and audio enabled, each using an isolated automation save:

| Suite | Checks | Failures | Log |
|---|---:|---:|---|
| 0.6 convenience, settlements, companions; includes 0.5 security/vehicles | 168 | 0 | `Saved/PackagedV6_Release06.log` |
| 0.4 progression, quests, dialogue, bunker and crew | 123 | 0 | `Saved/PackagedV4_Release06.log` |
| 0.3 POIs, interactions and enemy types | 194 | 0 | `Saved/PackagedV3_Release06.log` |
| 0.2 weapons, ammunition, inventory, loot, navigation and survival | 518 | 0 | `Saved/PackagedV2_Release06.log` |

All four exported-game processes exited successfully: **1,003 gameplay checks, zero failures**. Final packaged screenshots, including the shared map tab, are retained in `Docs/ScreenshotsV6`.
