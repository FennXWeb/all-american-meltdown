# Build 0.7 validation

Windows / Unreal Engine 5.8.1. Editor/source update; no cooking, staging, archiving or packaging.

## Build and assets

- Native game target compiled successfully (`Saved/BuildV7Game.stdout.log`). Later refinements are validated with the editor target.
- Latest native editor build succeeded (`Saved/BuildV7.stdout.log`).
- An isolated project copy under `Saved/ValidationV7` was used while the user's editor held the main DLL. Subsequent checks use the main project after the editor closed.
- Nine original surface sets and normal maps plus two original audio assets were generated and imported. New audio slots preserve existing catalog assignments.

## Regression coverage

New automation tests cover all eleven vehicle profiles, weighted rarity, steering direction, zero-speed yaw, cornering acceleration limits, deterministic regional generation, rural density, all eighteen POI types and parcel separation. Existing inventory, weapon, security, navigation and progression tests remain enabled.

Two old generation fixtures required updates: not every region has a direct north connection, and driveways now attach exactly to road polylines. Tests now verify northward connectivity through the backbone and exact attachments while retaining road-following checks. Settlement foundations also participate in road clearance and terrain flattening.

The live suite drives each vehicle using real W/D input, verifies cargo sizes and passenger capacities, tests police equipment, checks observed mannequin pursuit/attacks, stores partial quest progress, constructs all eighteen POI types and captures their interiors and the regional map. Saves use `LethalWorld_AutomationV7`; the user's survivor save is untouched.

The first mannequin pursuit failure was a fixture issue: the shared harness disables non-target enemies. Assigning the mannequin as `TestEnemy` allows its real movement component to run. Attack behavior already passed. Initial visual review also found missing instanced-mesh material usage and stretched architectural UVs; both were corrected.

Legacy sedan tests now explicitly request sedans rather than relying on the old single-model spawn pool. The older POI suite checks enemy construction independently of starting-area density, and places its explosion fixtures above the sampled terrain. Original mounted business signs were restored with their existing artwork during regression review.

The main-project automation pass completed **33 tests with no failures** (`Saved/AutomationV7.log`). Final live results:

| Suite | Checks | Failures | Log |
|---|---:|---:|---|
| 0.7 vehicles, POIs, progression and mannequins | 131 | 0 | `Saved/RegressionV7_07.log` |
| 0.6 inventory, settlement and companion passengers, including 0.5 security | 124 | 0 | `Saved/RegressionV6_07.log` |
| 0.4 progression, dialogue, settlements and companions | 123 | 0 | `Saved/RegressionV4_07.log` |
| 0.3 POIs, destruction and enemy behaviors | 194 | 0 | `Saved/RegressionV3_07.log` |
| 0.2 weapons, magazines, inventory, trading, relief and survival persistence | 514 | 0 | `Saved/RegressionV2_07.log` |

All 1,086 live checks passed. Clinic relief terminals were also restored after the survival regression detected their absence from the rebuilt clinics. Final captures of all vehicle exteriors and cabins, eighteen POIs and the map are in `Docs/ScreenshotsV7`.

Live audio uses an enabled but muted device; these tests do not measure perceived loudness or substitute for extended manual driving and playtesting.
