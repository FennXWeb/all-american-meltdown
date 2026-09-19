# Arsenal and lighting — update 24

Editor/source update. Do not package automatically.

## Weapons

| Weapon | Behavior | Ammunition / reload |
| --- | --- | --- |
| Missile launcher | Swept travelling missile; blast falloff, line-of-sight shielding, ragdoll forces and vehicle damage | Individual missile canisters; refill with missiles in inventory |
| Minigun | Six rotating barrels; roughly 0.67-second spin-up, automatic fire, heat limit and cooldown | 150-round 7.62 feed boxes |
| Sawed-off shotgun | Two independently spent barrels, wider spread and shorter effective range | Break action; spent-shell ejection and individual 12-gauge shell insertion |
| Desert Eagle | Heavy .50 AE semiautomatic pistol | Seven-round detachable magazines |
| M4 | Select-fire 5.56 carbine; compatible optics, light, laser and grips | Existing 30-round rifle magazines |
| Taser | Short-range electrical hit; temporary immobilization, shorter against massive enemies | Replaceable single-shot dart-pair cartridges |
| Flamethrower | Short-lived moving flame volumes, contact damage and lingering burn damage | Refillable 100-unit fuel tanks |

The existing four equipment hotkeys still select primary, secondary, sidearm and melee. Reload with R; drag matching ammunition onto a magazine, canister, cartridge or tank in inventory. Reload transactions preserve magazine identities and already seated ammunition if interrupted. Weapons and supplies appear in military, depot and trader loot; generated stock is not retroactively inserted into containers already opened and saved.

## Legendary effects and finishes

The highest tier receives one persistent effect: Two Shot, Life Steal, Toxic, Rapid Fire, Recoil Free or Explosive. Two Shot doubles outgoing shots without spending an extra cartridge. Life Steal restores health from actual damage to living enemies. Toxic adds damage over time; Rapid Fire reduces firing intervals to 40%; Recoil Free eliminates input recoil; Explosive adds a blast to impacts (and boosts missile blast damage/radius). Existing legendary weapons without a stored effect use a stable identity-based fallback.

Rare, epic and legendary weapons each have ten tier-specific textures: Shatter, Circuit, Tiger, Topographic, Hazard, Hex, Marble, Stars, Ember and Stripes. Each eligible generated weapon has a 45% finish chance. Rarity, effect and finish survive serialization and transfers. The finish covers receiver and moving metal surfaces while retaining rubber and glass materials. Inventory inspection identifies the finish and legendary effect.

## Lighting

Settings → Lighting cycles Performance, Lumen, and Hardware Ray Tracing when supported by the active RHI. The selection saves independently of resolution. Lumen is the default; hardware ray tracing is opt-in, with automatic Lumen fallback on unsupported hardware. Performance uses screen-space reflections.

Project settings compile ray tracing and distance-field support for DX12/SM6. Lumen provides indirect lighting and reflections; hardware mode uses ray-traced Lumen and hit lighting. Windshields, puddles and breakable windows use surface-forward translucency/front-layer reflections. Animated wet normals retain existing drying and wiper inputs. The retro camera filter remains available. This is real-time Lumen, not an offline path tracer.

New sound slots are in `/Game/Audio/DA_AudioCatalog`: MissileFire24, MinigunFire24, SawedOffFire24, DeagleFire24, M4Fire24, TaserFire24 and FlameFire24. Generated reports use the established stable local-player playback path.

## Asset and verification scripts

- `Tools/make_models_v24.py` — Blender source and 15 FBX meshes.
- `Tools/make_finishes_v24.py` — 30 textures and seven normalized mono reports.
- `Tools/build_v24_content.py` — Unreal imports, materials, and additive catalog updates.
- `Tools/VerifyArsenal24.ps1` — editor automation and offscreen gameplay checks. No packaging.

## Verified

Unreal 5.8 Editor compilation succeeded (`Saved/Build24g.log`). All 50 automation tests and 75 offscreen gameplay/rendering checks passed (`Saved/Arsenal24_Automation.log`, `Saved/Arsenal24_Runtime.log`). Screenshots were inspected after shader completion, including reload parts, readable finishes with the flashlight on, transparent glass, puddles and hardware ray tracing. Test screenshots are in `Saved/ScreenshotsV17/*24*`. No packaged build was produced.
