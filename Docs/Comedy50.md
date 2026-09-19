# Comedy arsenal and driving seat controls — update 50

Five weapons are available through depot/military loot and trader stock. Existing loot overrides are retained; Tools/import_comedy50.py adds only missing entries.

| Weapon | Console item ID | Ammunition | Behavior |
|---|---|---|---|
| 10-Barrel Shotgun | tenbarrel | Ten loose 12-gauge shells | 100 pellets in one discharge (ten shotgun loads), severe backward launch, constrained physics ragdoll and automatic recovery. Requires all ten shells to reload. |
| Giant Glock | giant_glock | .50 AE, mag_giant50 (12) | Oversized pistol frame, heavy recoil, detachable magazine. |
| Questionable AK | questionable_ak | 7.62, mag_ak75 (75) | Automatic fire, extended curved magazine. Support hand retains the magazine while the receiver is thrown away and replaced. A loaded spare is swapped when available. Without a spare it retains existing ammunition; changing receivers never creates ammunition. Refill magazines through inventory. |
| Finger Guns | finger_guns | None | Two posed hands, low damage, spinning flourish reload with no ammo requirements. |
| Budget Cut AR-15 | budget_cut | 5.56, mag_rifle30 | Plastic receiver and independently moving barrel. Barrel droops and sways; holding fire raises the support hand and straightens the barrel before the first shot. |

Example: `give tenbarrel 1`, then `give ammo_12g 20`. Equip through inventory. No changes to the starting loadout.

All five use existing rarity/legendary modifiers and finishes. Nine additional workbench parts provide recipes for dismantling and rebuilding. Glock, AK, and finger actions include integral emitters; ten-barrel and plastic barrel assemblies are separate. Workbench changes retain ammunition and magazine identity.

## Driver seat

While in the driver seat and outside menus, hold **Alt**:

- Up / Down: raise / lower the viewpoint.
- Right / Left: move forward / backward.

Adjustment is continuous, capped at +20/-24 cm forward/back and +/-15 cm vertically. It affects the player viewpoint only. Each currently loaded vehicle remembers its adjustment; offsets reset when that vehicle is unloaded or the game restarts.

## Assets and implementation

- Tools/make_comedy50.py generates nine original mesh assets with authored pivots, bores, sights, controls and moving mechanisms.
- ArtSource/ModelsV50 contains FBX sources, manifest and editable Comedy50.blend.
- Tools/import_comedy50.py validates mesh dimensions/material slots and merges item/loot entries.
- LWRecoil50.cpp uses seven physical bodies with limited joints, CCD, camera tracking and a collision-checked stand-up position. The player's damage capsule follows the ragdoll.
- LWComedy50Smoke.inl runs isolated production-path checks. Workbench tests include all 21 weapon recipes.

No packaged build is produced.

## Verification

- Editor target compiled successfully: Saved/Build50Complete.log.
- Final isolated in-engine run: **92 checks, zero failures** (Saved/Comedy50Complete.log).
- Workbench regression suite: **5 tests passed**, covering all 21 recipes, ammunition round trips, failure atomicity, freeform effects/serialization, and all 59 parts in loot (Saved/Workbench50Final.log).
- Reviewed in-engine screenshots for the five weapons and the AK throw pose; final screenshots wait for shader completion.
- Weapon wheel now cycles through the full weapon catalog, including these five additions.
