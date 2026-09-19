# Build 0.15 validation

September 7, 2026. Unreal Engine 5.8.1 editor target. No packaging/cooking/staging was run.

- Editor compile succeeded: `Saved/Build15g.stdout.log`.
- Logo, six original synthesized audio cues, character and effect materials imported. Five original Blender meshes passed the existing strict material-slot and 0.05 cm bounds checks: `Saved/Content15Verified.log`.
- All 45 automation tests passed: `Saved/Automation15.log`. Includes the new category-budget and cinematic-script test plus the existing weapons, inventory, navigation, quests, cards, encounters and persistence tests.
- Final opening/creator gameplay test: 42 checks, zero failures: `Saved/Regression15Verified.log`, `AAM_V15_DONE`. Covers all nine scene assets/audio, pause, natural completion, skip, real paused keyboard entry, balanced and invalid allocations, customization, cancel/save safety, seed preservation, bunker spawn and continued-save identity/category restoration.
- Compatibility suite: 2,209 checks, zero failures (`Saved/Compatibility15.log`, `LW_V9_DONE`). Covers POI furniture and roofs, vehicle interiors, weather/wipers, sleeping/sitting, keyboard seed entry and saving/restoring customized world settings through the new opening/creator flow.
- Final screenshots retained in `Docs/ScreenshotsV15`. Inspected main/pause menu branding, cinematic city and bunker shots, blast, character preview, bunker HUD and outdoor combat HUD. Fixed the camera clipping and framing found in earlier screenshots.

The final compile changed only the keyboard regression's input-frame timing after the 45-test automation run; the gameplay implementation was unchanged. The complete `Tools/Verify.ps1` multi-version sequence was not run.

This is a subtitled in-engine opening with synthesized sound design; it has no recorded spoken narration. The character preview and appearance data are original low-poly assets, with clothing color applied to existing first-person sleeves. Internal module/project-file and survivor-save identifiers remain stable to preserve compatibility with earlier saves.
