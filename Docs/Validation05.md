# Build 0.5 validation

Windows, Unreal Engine 5.8.1, Visual Studio 2022 / MSVC 14.44. September 6, 2026.

## Editor and content

- Editor C++ compilation succeeded (`Saved/BuildV5.stdout.log`).
- Six original sedan meshes imported with verified centimeter bounds and material slots (`Saved/ContentV5.log`).
- Eight original sounds imported; 55 existing sounds preserved. The editable audio catalog has 63 slots and validated with zero warnings.
- All **29 Unreal automation tests passed** (`Saved/AutomationV5.log`), covering existing world generation, roads, navigation, inventory, magazines, loot and progression, plus lock tolerance and save serialization of VIN-bound keys, vehicle location/security/damage and seated-car identity.
- New-feature live gameplay: **121 checks passed, zero failures** (`Saved/GameplayV5.log`).

The live suite verifies inaccessible locked storage, master-tier selection, binding and consuming exactly one pick, retrying, unlocking with real torque input, mixed procedural tiers and their loot rewards, rejecting another car's key, using the matching key to unlock and start, first-person camera placement, combat gating, lights, wipers, signal selection, attached radio playback, physical glovebox motion and inventory transfers, hotwire sequence errors, overheating, successful timed bypass, crosshair/E radio interaction, swept collision against a wall, acceleration, braking, safe exit/storage speed restrictions, occupied save/load, key identity in nested storage, and vehicle unloading/recreation without duplication after relocation.

The harness uses known lock angles and controlled starter-clock phases to test outcomes reproducibly, rather than solving unknown puzzles. It invokes the real player-input torque path and gameplay components. These checks do not replace extended manual driving and difficulty-balancing playtests.

## Visual review

Six actual engine captures are retained in `Docs/ScreenshotsV5`: sedan exterior, lockpicking, driver seat, glovebox grid, hotwiring and driving. Cabin visibility and UI text fit were reviewed at 1280 × 720 with the game's camera filters enabled. The driving HUD displays vehicle controls rather than firearm slots while seated.

The vehicle is an arcade, terrain-following prototype, not a full Chaos vehicle simulation. The limitations and controls are documented in `Docs/Build05.md`.

## Packaged release

- Full Windows build/cook/archive succeeded (`Saved/PackageV5.log`); final code-only restaging/archive also succeeded (`Saved/PackageV5Final.log`).
- Final packaged vehicle/security suite: **132 checks passed, zero failures** (`Saved/PackagedV5Final.log`). Additional checks verify generated glovebox keys match their own VIN and can occur in locked cars. Key placement uses a separate deterministic roll from door locking.
- Packaged survival, settings, inventory, magazines, all weapons, trading, acoustics, ragdolls and death recovery: **518 checks passed**, zero failures (`Saved/PackagedV2_Release05.log`).
- Packaged POIs, signs, doors, windows, pump explosions, vehicle cargo and enemy behavior: **194 checks passed**, zero failures (`Saved/PackagedV3_Release05.log`). The cargo fixture uses G to search the car; E now enters its driver seat.
- Packaged progression, quests, dialogue, settlements, bunker and companions: **123 checks passed**, zero failures (`Saved/PackagedV4_Release05.log`).

The three legacy suites ran before the final independent key-placement adjustment; the complete vehicle suite was rerun against the final archive afterward. All gameplay runs use isolated automation save slots. The playable build is `Builds/Windows/LethalWorld.exe`.
