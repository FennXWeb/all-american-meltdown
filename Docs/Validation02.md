# Build 0.2 validation

Validated on September 4, 2026 (EDT), using Unreal Engine 5.8.1 and the Windows Development build.

| Check | Result | Evidence |
| --- | --- | --- |
| Native editor compilation | Passed | `Saved/BuildV2.log` |
| Unreal automation | 24 passed, 0 failed | `Saved/AutomationV2.log` |
| Rendered editor gameplay | 463 passed, 0 failed; 103.3 seconds | `Saved/GameplayV2.log` |
| Windows build, cook, stage, archive | BUILD SUCCESSFUL | `Saved/PackageV2.log` |
| Packaged game, launched through `Builds/Windows/LethalWorld.exe` | 463 passed, 0 failed; 104.1 seconds | `Saved/PackagedGameplayV2.log` |
| New modular model import | All 23 meshes passed bounds/material validation | `Saved/ContentV2.log` |
| Audio catalog import and runtime loading | All 46 slots resolved | `Saved/ContentV2.log`, packaged gameplay log |
| Rendered captures | 34 captured in the packaged run | `Builds/Windows/LethalWorld/Saved/ScreenshotsV2` |

Automation covers deterministic chunk generation, negative and distant coordinates, road intersections and driveway attachments, item placement and rotation, irregular footprints, equipment restrictions, atomic transfers, magazine ownership and ammunition conservation, loot weights and guarantees, and save serialization. The bunker driveway has a separate service-street attachment assertion; it is not treated as an arterial curve fixture.

The gameplay suite performs actual attacks and timed reloads for all eight weapons, checks magazine identity and chamber/cylinder state, runs automatic fire, observes seven simulated zombie bodies and their six constraints, buys and sells through the trader, tests protected stash access, drops every carried record on death, respawns without replacement gear, and reloads the persisted state. It also verifies chunk streaming, bunker entrance collision and interaction, and live robot patrol movement.

Video validation applies a real 1600 × 900 viewport and observes the 15-second automatic rollback while paused. Captures were reviewed at 16:9 and at the packaged run's 3440 × 1369 window size. The audio device is initialized and muted during the suite; wind, drone and music are checked after the complete session. Tests use `LethalWorld_AutomationV2`, separate from the survivor save. These are automated checks and visual review, not a long manual playthrough or an audio mix audition.

Representative unedited captures copied from the packaged game:

- [Title](ScreenshotsV2/01_Menu.png), [settings](ScreenshotsV2/02_Settings.png), [video confirmation](ScreenshotsV2/02A_VideoConfirmation.png)
- [Inventory and secured stash](ScreenshotsV2/07_Stash.png), [field map](ScreenshotsV2/05_Map.png), [bunker entrance](ScreenshotsV2/05A_BunkerEntrance.png)
- [Revolver reload](ScreenshotsV2/33_Revolver_Reload.png), [LMG reload](ScreenshotsV2/37_LMG_Reload.png), [ragdoll](ScreenshotsV2/40_Ragdoll.png)
- [Trader encounter](ScreenshotsV2/40A_TraderEncounter.png), [trade inventory](ScreenshotsV2/41_Trader.png), [death screen](ScreenshotsV2/43_Death.png)

The tested runtime executable is `Builds/Windows/LethalWorld/Binaries/Win64/LethalWorld.exe` (332,496,384 bytes). SHA-256:

```text
ADCBD0E4D87B673DA6919FD88278443E3A4123EBBE6C86E3FF91AB1BBC100A45
```

To repeat project verification, run `Tools/Verify.ps1`. It requires at least 24 successful automation tests and rejects any failed test before running gameplay. To test a packaged build, launch its executable with `-LWV2Smoke -LWAudioSmoke -RenderOffscreen -windowed -ResX=1280 -ResY=720 -unattended`; inspect its log for `LW_V2_DONE failures=0`.
