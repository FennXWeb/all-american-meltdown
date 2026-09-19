# Build 0.3 validation

Verified September 4, 2026, using Unreal Engine 5.8.1 on Windows. The Windows Development package is complete at `Builds/Windows/LethalWorld.exe`.

| Verification | Result | Evidence |
| --- | --- | --- |
| Native automation: world generation, roads/navigation, inventory, ammunition, loot and audio contracts | 24 passed, 0 failed | `Saved/AutomationV3.log` |
| Existing-feature gameplay regression in the editor runtime | 494 checks passed | `Saved/RegressionV3.log` |
| Final new-feature gameplay pass in the editor runtime | 170 checks passed | `Saved/GameplayV3.log` |
| Exported Windows executable, new-feature gameplay pass | 170 checks passed | `Saved/PackagedGameplayV3.log` |
| New FBX dimensions and material assignments | All 24 meshes validated; bounds tolerance 0.05 cm | `Saved/ContentV3.log` |
| Asset availability | All referenced meshes and 52 sound slots loaded | Gameplay logs |
| Windows build, cook, stage and archive | BUILD SUCCESSFUL | `Saved/PackageV3.log` |
| Authoring/build script syntax | Five Python scripts and both PowerShell scripts passed | Local parser checks |

The integrated v3 checks exercise all five generated POI types, hinged door state, window bullet collision before/after damage, persistent broken panes, vehicle grid access and non-rerolling cargo, save/load of changed props, adjacent-pump chain explosions, prevention of repeated detonations, an actual player shotgun detonation, shooter blast damage with distance falloff, raider firing, dog biting, mannequin gaze detection/freeze/approach/provocation, and seven-body constrained ragdolls for all three new enemies. The harness uses an isolated automation save slot and transiently mutes its live audio device.

Eighteen screenshots were captured from the packaged game and copied into `Docs/ScreenshotsV3`. These are actual Unreal frames, with the game's crust filter enabled. Exterior sign readability, representative interiors and enemy/ragdoll appearance were visually inspected. The depot interior capture is close to a shelf; it is not a complete room survey.

![Last Light forecourt](ScreenshotsV3/POI_0_Exterior.png)

![Dead End diner](ScreenshotsV3/POI_4_Interior.png)

![Mannequin](ScreenshotsV3/Enemy_3.png)

## Reproduction

Run `Tools/Build.ps1 -Content -Package` to compile, import and package. Source FBXs, generated PNGs and WAVs are retained in ArtSource. `Tools/Verify.ps1` runs native automation and both gameplay suites. To test the packaged game, launch the Windows executable with `-LWV3Smoke -LWAudioSmoke -RenderOffscreen -unattended -abslog=<absolute log path>` and require `LW_V3_DONE failures=0` in that log. The test process exit code alone does not establish that gameplay assertions passed.

## Artifact identity

Native game executable: `Builds/Windows/LethalWorld/Binaries/Win64/LethalWorld.exe`

Size: 332,551,168 bytes.

SHA-256: `9CAC75F320C4B9B4EE0C0AFCAFF61E8F3CDB357C5F60DB44622E2B89B6168692`

Use the bootstrap executable at `Builds/Windows/LethalWorld.exe` to play. Keep its packaged Engine and LethalWorld directories alongside it.

The tests validate this prototype's implemented systems; they are not a long-duration balance or performance certification. Character animation, effects and synthesized audio remain prototype assets. See [the build guide](Build03.md) for controls, authoring paths and save compatibility notes.
