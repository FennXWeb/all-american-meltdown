# Visual library 63

This revision replaces the runtime character/creature and attachment mesh resolution with `/Game/Art/Models63`. Existing appearance indices, item IDs, NPC types, save data and attachment mounts are retained.

## Faces and human characters

- Separate male/female face landmark mapping (eyes, nose and mouth), with the facial feather carried through morphs.
- Iris UVs are isolated from facial projection and sample the iris area; an eye-specific tint palette replaces the clothing palette that made default eyes black.
- All 146 approved anatomical/clothing/hair modules are carried into the rebuilt library. Existing vertex-weighted hair motion, facial expressions, clothing customization and seven-body ragdolls remain in use.

## NPC library

- Rebuilt surfaces for dogs, moose, titans, deathclaws, scorpions, scooter zombies, mannequins, bears, hornets, rogue robots, traders and border mechs/armor. Behemoths and colossi inherit the updated titan library.
- Titan, mannequin and scooter-zombie parts use approved anatomical geometry; humanoid right limbs have their own mirrored meshes. The titan's animation attachment positions match the revised anatomy. The scooter chassis is preserved.
- World Eater uses annulated organic segments and a hollow, double-ring toothed mouth instead of engine spheres/cones.
- Shared generated atlas: scarred skin, animal fur/hide, chitin, anodized metal, canvas, horn and polymer. Per-part creature mesh budgets and three distance LODs constrain rendering cost. Existing CPU animation/ragdoll meshes remain accessible.

## Attachments and sights

Twenty existing attachment models are replaced. Machined apertures, rails/clamps, screws, scope turrets, adjustment rings, contoured stocks, recoil pads, grips and hollow muzzle components use modeled geometry.

Five new obtainable attachments integrate with the Gunsmith and military/depot/trader loot:

| Item ID | Sight | Aim presentation |
|---|---|---|
| `att_micro63` | Micro reflex | Open 1x dot; pistol-compatible |
| `att_tube63` | Enclosed red dot | Open 1.5x circle-dot |
| `att_prism63` | Prism 3x | Scoped chevron |
| `att_combat63` | Combat scope 6x | Scoped range ladder; long guns |
| `att_marksman63` | Marksman scope 12x | Scoped mil-dot; long guns |

The existing camera-based magnification and reticle renderer are used. Scope lenses have real open mesh apertures. These are fixed magnification sights; no thermal highlighting or switchable zoom is claimed.

## Source/rebuild

Editable assets: `ArtSource/Models63/Models63.blend`, FBXs and `manifest.json`.

1. Blender: `Tools/build_models63.py`.
2. Blender: `Tools/refine_anatomy63.py` (anatomical replacements and fitted details).
3. Blender: `Tools/fit_waists63.py` after rebuilding to close waist/leg seams.
4. Unreal Python: `Tools/import_models63.py` (full import; optional `LW63_GROUP=npc` for creature-only updates).
5. Full Unreal Editor Python: `Tools/finish_models63.py` (LOD subsystem is unavailable in a Python commandlet).

The anatomy retains the recorded MakeHuman CC0 provenance from Characters35. No Fallout or Call of Duty assets are used.

## Validation

- `LethalWorld.Visual63` tests cover new sight compatibility/zoom/alignment and morph UV preservation.
- `-LWV17Smoke -LWUI46Smoke -LWVisual63Smoke` tests every enemy type and the workbench sights.
- `-LWV17Smoke -LWUI46Smoke -LWCharacters61Smoke` exercises updated player/NPC appearance, hair motion and ragdolls.
- No packaging step is part of this workflow.

### Verification results (September 20, 2026)

- Development Editor build succeeded (`Saved/BuildVisual63e.log`); no packaged build.
- 240 source meshes: 146 approved character modules, 69 creature/robot modules, 25 attachments. Full import passed material/bounds validation; final anatomical and waist imports passed. LOD generation succeeded in the full editor (`Models63FinalizeB.log`, `Models63WaistImport.log`).
- All four targeted automation tests passed (`Saved/Visual63Tests.log`).
- Character workflow passed 54 checks, zero failures (`Saved/Visual63CharactersSmoke.log`). Male/female face close-ups visually checked in the runtime, with no new material compile failures.
- NPC and Gunsmith workflow passed 144 checks, zero failures (`Saved/Visual63Smoke.log`); new micro reflex and 12x scope previews visually inspected. Original editor user settings restored and hashes verified after all automated editor instances exited.
