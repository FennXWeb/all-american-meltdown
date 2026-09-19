# Build 0.16 validation — 2026-09-07

Editor/source update only. No packaging or shipping cook was run.

## Results

| Check | Result | Evidence |
|---|---|---|
| Unreal 5.8.1 Development Editor compile | Succeeded | `Saved/Build16e.stdout.log` |
| Import original models, alpha logo and celestial materials | Succeeded; 17 mesh bounds/material assignments checked | `Saved/Content16c.log` |
| LethalWorld automation suite | 45 passed, zero failures | `Saved/Automation16.log` |
| Build 16 rendered regression | 71 passed, zero failures | `Saved/Regression16_V16.log` |
| Equipment, motorhome and chauffeur regression | 266 passed, zero failures | `Saved/Regression16_V14.log` |
| Opening, creator and save-flow regression | 42 passed, zero failures | `Saved/Regression16_V15.log` |

The final suites were run sequentially by `Tools/Verify16.ps1`, with offscreen rendering at 1280×720 for gameplay and isolated automation save slots. The regular survivor save was not used.

Build 16 checks cover ten imported hairstyles, independent body controls, appearance save/load, the 06:00 bunker start, first-person body construction, storage records, all ten bedrooms, locker placement, the hatch wall plane, daytime and moon intensity, settlement defense and hostility scope/persistence, companion data, and the six-wheel/five-seat coach with its standing cabin. Map markers and XP rendering were also inspected visually.

The larger RV initially failed the existing chauffeur movement assertion because its chassis struck terrain. The final implementation corrects suspension height and terrain clearance under the overhang; the same displacement assertion now passes. The vehicle suite also confirms waypoint arrival, controlled driver exit, barricade stopping, all vehicle seats, and camper fixture interactions. Diagnostic logging has been removed.

## Visual review

Screenshots are retained in `Docs/ScreenshotsV16`:

- `Menu16.png` and `Pause16.png`: alpha-cutout logos over the live menu/pause backgrounds.
- `CreatorFemale16.png` and `CreatorMale16.png`: creator layout and original low-poly bodies.
- `BunkerHall16.png`, `Hatch16.png`, and `Room16_0.png` through `Room16_4.png`: hallway, flush hatch, bedroom and service-room layouts.
- `Sky16_6.png`, `Sky16_12.png`, and `Sky16_0.png`: sunrise, day and night.
- `CrewXP16.png`: cyan companion marker and partial XP bar.
- `Motorhome16.png` and `CamperInterior16.png`: longer exterior and usable living cabin.

The source logo is RGBA with genuine transparent pixels. Its main-menu motion is rendered in engine with heat shimmer and rising embers; a still screenshot only verifies compositing, not the motion itself.

## Limits

These are authored low-poly static body parts with proportional customization, not a skeletal character pipeline or facial animation system. Morphs are cosmetic and do not alter collision or stats. Validation is an automated editor smoke/regression pass with visual inspection, not an extended manual playthrough. Existing saves retain their clock, appearance defaults and stored items. Packaging remains the user's next step.
