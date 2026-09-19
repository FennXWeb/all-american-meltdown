# Character model rebuild (32)

Player and humanoid NPCs use a new PS2-inspired character library: shaped facial planes, painted facial detail, jacket collars and seams, raised pockets, belts, boots with soles and laces, modelled hands, and ten fitted hair styles. Male/female player bodies retain height, build, shoulder, face, skin, outfit and hair controls. Named story residents and generated residents use deterministic appearance variation, based on identity text rather than process-dependent name hashes.

86 mesh assets were created/imported. Resident, female resident, zombie, raider, mannequin, player bodies, heads, limbs, hair and first-person hands are rebuilt. Existing creature bodies have been remeshed/relaxed and resurfaced while retaining their special anatomy and mechanical components. Trader robot surfaces are updated. The procedural World Eater retains its existing construction.

## Integration

- Runtime mesh families are `SM_*32` under `/Game/Art/Meshes`.
- Character materials are `M_*32` under `/Game/Materials`, with two shared 1024px atlases in `/Game/Art/Textures`.
- Player gun, reload, melee and unarmed views use `RightHand32` and `LeftHand32`.
- Character materials keep their texture parent when applying appearance colors. Story armor replaces clothing slots without covering faces.
- Hair attaches to the head segment, follows its animation/ragdoll and is removed when changing enemy kind.
- Existing segmented animation, seven-body ragdolls, joint pivots, collisions and dismemberment remain in use. This is an asset/appearance rebuild, not a replacement skeletal animation system.
- Existing pixel/CRT presentation is retained.

## Editable source and regeneration

`ArtSource/Characters32/AllAmericanMeltdown_Characters32.blend` contains the editable meshes and materials. FBXs, atlases, bounds/material manifest, and neutral studio preview renders are alongside it.

Run Blender 4.2 in background with `Tools/make_characters32.py` to rebuild source meshes. Run Unreal Editor Python with `Tools/import_characters32.py` to rebuild character materials and import/validate the mesh library. `Tools/render_characters32.py` renders a neutral four-person lineup and portrait. Geometry is authored in metres, with +X forward and existing joint pivots retained. The import converts to Unreal units and validates bounds and material assignments. Vertex colors carry the facial projection blend mask.

## Generated texture provenance

Backend: built-in image generation (no API key or external image service). Generated PNGs were copied into the project without raster modification. Face positioning and edge blending are performed by authored UVs and the material shader.

- `T_CharacterSurface32.png`: shared 2x2 diffuse atlas. Prompt brief: PS2 survival-horror character texture atlas, exact equal quadrants; neutral pale beige skin without facial features; gray worn cotton fabric; dark brown leather; gray vertically combed hair. Flat diffuse lighting, subtle wear, no borders, labels, objects, perspective, or baked highlights.
- `T_CharacterFaces32.png`: four diffuse face projections. Prompt brief: exact equal 2x2 atlas of symmetrical front-facing male/female faces in light olive and deep brown skin, neutral closed-mouth expressions, realistic painted PS2 facial planes, pores and creases, no hair or shoulders. UV-ready frontal faces; eyes at approximately 30/70 percent X, requested 45 percent Y, nostrils 64 percent and mouth 78 percent. Actual generated feature placement is accommodated in the mesh UV mapping.

Original tool outputs: `exec-392b9cab-6964-41e6-8301-92f7afffbe24.png` (surface) and `exec-93949cec-be0a-4169-9eac-a1fc942fbf17.png` (faces), in the task's Codex generated_images directory. These are generated textures, not extracted assets from another game.

## Validation

- All 86 imported meshes passed bounds and material-slot validation (`Saved/Characters32_ImportC.log`).
- Unreal Editor Development build passed (`Saved/BuildCharacters32c.log`; final fitting check recorded in `BuildCharacters32d.log`).
- Existing automation suite: 70 passed, zero failed, zero warning results (`Saved/Characters32Report/index.json`).
- Initial offscreen character test: 126 checks passed, zero failures. Covers both creator bodies, resident/raider/zombie segments, creature assets, seven simulating ragdoll pieces, and first-person mesh resolution.
- Final offscreen test: 128 checks passed, zero failures, including melee hands and the corrected creator neck fit (`Saved/Characters32_RuntimeFinal.log`). Captures are in `Saved/ScreenshotsV17/Characters32_*.png`. Nighttime weapon captures remain dark under the existing separate weapon lighting channel; first-person mesh assignments are verified, while studio/creator captures provide the material review.

No packaged build was produced.
