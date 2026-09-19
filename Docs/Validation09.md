# Build 0.9 validation

Unreal Engine 5.8.1 / Windows. Editor compilation succeeded, and 19 original Blender meshes passed the FBX bounds, axis, unit and material import checks. New texture/material import completed successfully. No cooking, staging or packaging was performed.

The expanded V9 gameplay suite exercises every vehicle profile and all nineteen POI types, passenger limits, controls, persistent furniture storage, weather, windshield buildup/wiper clearing, upgraded light intensity, gradual surface drying and persistence, sleeping, standing from chairs, mannequin spawn restrictions, real keyboard seed input while paused, and saved world settings.

Screenshots were inspected for vehicle exterior/cockpit containment, diner furnishing, boutique displays, wet and cleared windshield states, and the world setup screen. This is sampled visual coverage, not proof that every generated scene is free of overlap. Light tests check configuration and rendering, not physical photometric measurements.

The first V9 run passed 1,735 checks. A final run follows cockpit detail, first-launch setup and residential lane refinements. Generation automation checks deterministic regions, connectivity, parcel separation and POI diversity. Existing survival and gameplay suites run with separate automation saves; the survivor save is not reset.

See the results table below for the final build.

| Suite | Passed | Failed | Log |
|---|---:|---:|---|
| Automation | 33 | 0 | `Saved/Automation09.log` |
| Vehicle, POI, weather and new-world gameplay | 1735 | 0 | `Saved/RegressionV9_09.log` |
| Vehicle security, companions and inventory controls | 131 | 0 | `Saved/RegressionV6_09.log` |
| Progression, settlements and dialogue | 123 | 0 | `Saved/RegressionV4_09.log` |
| POI destruction and enemy behaviors | 200 | 0 | `Saved/RegressionV3_09.log` |
| Weapons, magazines, trading, storage and survival saves | 520 | 0 | `Saved/RegressionV2_09.log` |

All final suites passed. Final visual captures are retained in `Docs/ScreenshotsV9`. Build/import logs are `Saved/Build09.stdout.log` and `Saved/Content09.log`. PowerShell build/verification scripts parse successfully, and the asset scripts pass Python syntax checks.
