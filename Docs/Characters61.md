# Character library 61

Approved survivor integrated into the player creator, human residents, companions and humanoid enemies. This is a modular static/procedural mesh library adapted to the existing animation and seven-body ragdoll system, not a replacement skeletal animation framework.

## Assets

- 146 versioned meshes in `/Game/Art/Characters61`; editable source `ArtSource/Characters61/Characters61_Runtime.blend`, FBXs and `manifest.json`.
- Male/female heads and clothing bodies adapted from the approved survivor, distinct left/right limbs, nine upper clothing choices, six lower choices and four footwear choices.
- Nineteen hairstyles retain the existing customization indices, with layered alpha strands, opaque roots and runtime spring deformation. Headwear suppresses hair. Hats are resized for the new cranium.
- New cloth/leather/skin/hair atlas, male and female facial textures, and hair-card alpha texture. Existing tattoo, glove and color controls remain supported.
- Accessories and first-person grip hands reuse authored geometry with the revised materials; they are not new anatomy models.

`ArtSource/Character61` remains the original approved sample. Base anatomy provenance/license is recorded under `ArtSource/Characters35/Reference` (MakeHuman CC0). No Fallout assets are included.

## Runtime

`LWAppearance::Mesh61` prefers this library and falls back to legacy assets if a named mesh has no replacement. Existing save appearance indices remain unchanged. The CPU-access flag supports the existing facial/body deformation and ragdoll paths.

`ULWHair61` preserves imported UV2.X strand compliance when copying the mesh. Roots stay fixed; tips respond to acceleration, rotation and a restrained ambient movement. Deformation is bounded to two centimetres, uses spring substeps and is evaluated at up to 30 Hz within 25 metres of the camera. It also runs while the creator pauses gameplay. This is lightweight cosmetic hair motion, not a collision-driven strand groom.

## Rebuild and import

Run `Tools/build_characters61_runtime.py` using Blender in background mode. It reads the approved source plus legacy accessory/style assets, and writes the new runtime library only. Run `Tools/import_characters61.py` with Unreal's Python commandlet. It validates imported bounds and material slots and enables CPU access. Optional environment variable `LW_CHAR61_PREFIX` restricts a reimport to a named mesh prefix (for example `Hat`).

## Verification

Build the Development Editor target, not a packaged game. Run the game with `-LWV17Smoke -LWUI46Smoke -LWCharacters61Smoke` for asset, creator, actual weighted hair deformation, NPC, first-person and ragdoll checks; screenshots are written under `Saved/ScreenshotsV17/Characters61_*`. The separate `LethalWorld.Creator35.MorphsAndSavedAppearance` automation test covers creator morph bounds and appearance serialization.

Development Editor build passed (`Saved/BuildCharacters61d.log`). Final runtime smoke E passed 54/54 checks, including fixed roots and moving weighted strand vertices (`Saved/Characters61SmokeE.log`). Appearance serialization/morph automation passed (`Saved/Characters61CreatorTest.log`). Final screenshots were inspected after the seam and hair crown corrections. All 146 meshes passed import bounds/material validation; the final 19 hairstyles passed their subsequent reimport. No packaged build was created.
