# Build 0.8 validation

Unreal Engine 5.8.1 / Windows. Native editor compilation and content import succeeded. No cooking, staging or packaging was performed.

The cabin mesh was regenerated in Blender, then imported through the existing bounds/material validation pipeline. Rain and thunder assets and a parameterized sky material were imported successfully (`Saved/Content08.log`).

The expanded live suite checks each vehicle's driving and passenger capacity, constructs all eighteen POI types, verifies storage records and the absence of floating labels/random radios, checks one-time migration of old contents and persistent empty storage, exercises all five weather conditions, and verifies midnight rollover and saved day counts.

Initial validation caught a stationary sunlight component, which prevented rotation, and missing guaranteed lock-tier rewards in the new furniture container path. Both were corrected before the final regression run.

Existing survival, magazine/inventory, progression, companion and security suites remain enabled. Tests use isolated automation saves; the survivor save is not reset. Audio runs on an enabled but muted device, so perceived loudness is not measured by these tests.

## Results

| Suite | Passed | Failed | Log |
|---|---:|---:|---|
| Automation | 33 | 0 | `Saved/Automation08.log` |
| Expanded vehicle, POI, storage and weather checks | 1713 | 0 | `Saved/RegressionV7_08.log` |
| Vehicle security, companions and inventory controls | 128 | 0 | `Saved/RegressionV6_08.log` |
| Progression, settlements and dialogue | 123 | 0 | `Saved/RegressionV4_08.log` |
| POI destruction and enemy behaviors | 196 | 0 | `Saved/RegressionV3_08.log` |
| Weapons, magazines, trading, storage and survival saves | 516 | 0 | `Saved/RegressionV2_08.log` |

Final visual captures are saved under `Docs/ScreenshotsV8`, including the revised bedrooms, vehicle cabins, daytime weather and night lighting. The final weather run produces no stationary-sun mobility warnings.
