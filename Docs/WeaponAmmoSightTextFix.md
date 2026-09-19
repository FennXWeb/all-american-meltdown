# Weapon ammo, sight aiming, and world text edits

No compilation, packaging, asset generation, or editor launch was performed. Runtime visual verification remains outstanding.

## Inventory

- `Source/LethalWorld/LWInventoryHUD.cpp`: removed attachment/finish overwrites of the two ammo detail lines. The inspector now places ammo/chamber at Y551, magazine/feed at Y567, attachment shortcuts at Y583, damage/finish at Y599, condition/price at Y615, and actions at Y632 within the existing panel. Attachment order is deterministic (optic, light, laser, grip).
- Added readable caliber abbreviations (.357, 7.62, 5.56, .50 AE), missile and fuel labels. Loaded weapons show the magazine's display name and count/capacity; unloaded weapons identify the required magazine. Grid status distinguishes no magazine and direct-load missile/battery capacity.
- Cylinder/barrel details include ammo caliber; both double-barrel and sawed-off use BARRELS. Pump shotgun reports its actual five-round tube plus chamber. Loose shells/.357 give direct reload instructions.

## Aiming

- `Source/LethalWorld/LWWeaponMods.h`: added `OpenSight` and `SightHeight` helpers. Reflex aperture center is 2.85 cm above its mount; holo is 3.4 cm, derived from `Tools/make_models_v14.py`.
- `Source/LethalWorld/LWWeapons.cpp`: in the final viewmodel pose, open-sight ADS compensates for the weapon mesh transform and aperture position, aligning the aperture with the camera/fire ray. Cancels visual angular sway while aligned; retains pose interpolation. Reload, action-presentation, and bash poses bypass alignment. Projectile/fire logic is unchanged.
- `Source/LethalWorld/LWHUD.cpp`: optic crosshair suppression now requires a compatible known optic. Reflex/holo dot is a bright 3x3 center with a dark 6x6 backing for contrast.

## World text

- Added `Source/LethalWorld/LWWorldTextComponent.h` and `.cpp`: movable, non-colliding TextRender subclass. The original per-label 0.1-second ticks have now been replaced by a world subsystem with a maximum of 128 label visits per frame. Labels beyond 120 m or their shorter authored cull distance are skipped. Flips local yaw 180 degrees when the viewer is behind the glyphs, preserving the authored sign plane. The rotation now uses the rendered bounds center, compensating the component location for left/right alignment. A 2 cm dead zone avoids edge-on flutter. Uses the renderer's verified +X front-face convention; it does not continuously billboard toward the camera. This targets the existing single-local-player view.
- Replaced only the TextRender include and `NewObject` type in these world label factories:
  - `Source/LethalWorld/LWBunkerV16.cpp`
  - `Source/LethalWorld/LWCardTable.cpp`
  - `Source/LethalWorld/LWCasino.cpp`
  - `Source/LethalWorld/LWCommunities.cpp`
  - `Source/LethalWorld/LWDungeonBuilding.cpp`
  - `Source/LethalWorld/LWLandmarkBuilding.cpp`
  - `Source/LethalWorld/LWPOI.cpp`
  - `Source/LethalWorld/LWPOIV18.cpp`
  - `Source/LethalWorld/LWPOIExpansion.cpp`
  - `Source/LethalWorld/LWSlotMachine.cpp`
  - `Source/LethalWorld/LWUrban.cpp`
  - `Source/LethalWorld/LWWorldObject.cpp`
- `Source/LethalWorld/LWUnderground.cpp`: same factory substitution, plus entrance text local Y changed from -800 to -810. Its backing occupies Y[-807.5,-772.5], so the new position clears the front by 2.5 cm.

## Verification and ownership

- Read local UE TextRender geometry source and authored optic geometry; checked weapon mount/mesh transforms and camera-based firing.
- Passed 26 independent arithmetic cases covering eight supported weapon mount heights with two apertures, and five sign orientations viewed from both sides. These are geometry sanity checks, not compiled C++ tests.
- Static checks confirm no remaining raw TextRender creation sites, no attachment overwrite of ammo lines, inspector row bounds, and clean CRLF-aware diff whitespace checks.
- Did not edit `LWInventory.cpp`, `LWInventory.h`, `LWCharacter.cpp`, `LWCharacter.h`, `LWHUD.h`, MapHUD, build configuration, or assets.
- Preserved concurrent edits, including the dungeon guardian label. The newly appearing POI expansion factory received only the text helper substitution.
- Pre-edit copies are under `Saved/WeaponTextFixBaseline/`. They are review references, not rollback replacements: other agents may have subsequently changed those files.

Recommended later runtime checks: ADS reflex/holo on shotgun, revolver, sniper, SMG, rifle, LMG, Desert Eagle and M4; inventory inspection of magazine-fed, tube-fed, barrel/cylinder, missile and battery weapons; approach signs from both sides and inspect underground entrance text.

## Follow-up: fuel UI

- `LWInventoryHUD.cpp`: includes the existing `LWFuel.h` and displays selected gas can `FUEL Rounds/20 L` using `LWFuel::CanCapacity`, plus park/engine-off guidance.
- `LWSecurityHUD.cpp`: vehicle overlay reads `FuelLitres()` and `FuelCapacity()`, shows a proportional gauge and numeric litres, LOW at 15% or less and EMPTY at zero. VIN moved to Y675 to make room. Six independent gauge arithmetic cases and panel bounds checks passed; runtime not executed.

## Follow-up: performance and standalone smoke tests

- `LWWorldTextComponent.h/.cpp`: labels register/unregister weak references with `ULWWorldTextSubsystem`; no individual component ticks. One camera lookup per world update, bounded round-robin visits, and expired-reference removal. Transform/bounds work occurs only for nearby back-facing labels. The cap bounds frame cost but means a full refresh takes ceil(label count / 128) frames; unusually large populations can have visible orientation latency. Registration/removal costs occur on component lifecycle events, not every frame.
- `LWWeaponMods.h`: extracted production aperture alignment into `AlignOpenSight`; `LWWeapons.cpp` calls it at the same pose location.
- Added `Source/LethalWorld/LWWeaponTextTests.cpp` with standalone EditorContext automation tests. No game mode, vehicle fixture, or world fixture wiring. Text components can calculate glyph bounds without registration in a world.
- `LethalWorld.Weapons.OpenSightApertureSmoke`: eight compatible weapons, reflex/holo, identity and translated/rotated/nonuniformly-scaled mesh transforms. Verifies independently authored aperture center on the camera ray, forward bore alignment, near-plane clearance, and rejection of incompatible/unarmed optics.
- `LethalWorld.World.TextFacingAndBoundsSmoke`: real text component glyph bounds, three horizontal alignments and four yaws. Verifies no per-label tick, distant/dead-zone suppression, readable back face, fixed center/extents, and front/back round-trip without transform drift.
- Both automation tests are written but **not compiled or run**. No build/package/editor invocation was made. Shared headers and other agents' fixture wiring remain untouched.
