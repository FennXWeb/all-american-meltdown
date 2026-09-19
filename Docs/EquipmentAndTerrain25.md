# Equipment and terrain update 25

No packaged build is produced. Open and compile LethalWorldEditor as usual.

## Field equipment

Base carried inventory stays 12 × 10 for existing saves. Equip one backpack to extend it; scroll over the carried grid or use its scrollbar to reach extra rows.

| Pack | Extra slots | Total carried slots |
| --- | ---: | ---: |
| Sling pack | 24 | 144 |
| Backpack (existing starter pack) | 48 | 168 |
| Hiking backpack | 72 | 192 |
| Military rucksack | 96 | 216 |
| Expedition pack | 120 | 240 |

Capacity is respected by pickups, transfers, sorting, magazine swaps, unloading and attachment removal. Clear extra rows before removing or swapping to a smaller pack. An invalid removal rolls back without losing items or charging money.

Equip a chest rig to unlock Rig Primary, compatible with primary weapons. Key **5** selects this weapon; keys 1–4 retain their existing slots. Clear Rig Primary before removing the rig.

Night Vision Goggles occupy **Helmet** and toggle with **N**. Green monochrome image amplification, grain and vignette accompany a shadowed local illuminator. Removal or death switches them off. Goggles do not supply helmet armor. Existing NPC perception does not treat the illuminator as a player noise event.

The new packs and goggles have individual meshes, saved Blender sources, inventory icons, prices and weighted loot entries. Civilian packs appear in motel/road/depot/trader tables; larger packs and goggles in military/depot/trader tables. Previously generated containers retain their saved contents.

## Terrain and vehicles

Terrain flattening covers a full coarse-cell diagonal beyond developed plots and the entire casino parking approach. Tower foundations sit below first-floor landings. Casino driveway and lot slabs meet without overlapping top faces.

Vehicle suspension samples a longer vertical range and fits height at the chassis origin using the axle midpoint. This permits actual downward movement on slopes. Overhang support includes asphalt surfaces while walls and wrecks remain obstacles.

## Verification

`Tools/VerifyGear25.ps1` runs the full automation suite and offscreen equipment / downhill-bus gameplay checks. `Tools/make_gear25.py` regenerates Blender assets; `Tools/build_gear25_content.py` imports meshes and updates item/loot catalogs without rebuilding maps or packaging.

Verified in Unreal Engine 5.8: Editor target compiled (`Saved/Build25f.log`); 52 automation tests passed (`Saved/Gear25_Automation.log`); 26 gameplay/render checks passed (`Saved/Gear25_Runtime.log`). The bus sustained movement down both 18-degree and 36-degree asphalt slopes. Inventory and night-vision captures were inspected in `Saved/ScreenshotsV17/Gear25_*.png`. No packaging was run.
