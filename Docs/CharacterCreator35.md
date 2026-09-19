# Character models and survivor creator, revision 35

The humanoid character library uses shaped polygon surfaces with anatomical profiles, orbital recesses, nose/jaw/cheek geometry, fitted garment profiles, collars, sewn pockets, hands, boots, continuous hair surfaces, and conformal facial hair. These assets replace the previous humanoid library for residents, companions, raiders, ordinary zombies and the player's body and weapon hands. Specialized creature models keep their existing assets.

The mesh library contains 96 source/export meshes. A dressed character is roughly 8–11k triangles depending on clothing and grooming. This is a moderate polygon budget; the implementation uses the existing segmented collision/ragdoll system and procedural deformation, not a new commercial character-creation framework.

## Creator controls

- 24 body and face sliders: height, weight, shoulders, muscle, chest, waist, hips, arms, thighs, neck, head width/height, jaw, chin, cheeks, nose width/length, eye spacing/size, brow, mouth width, lip fullness, ears and face depth.
- 20 hairstyle choices including bald, and 11 facial-hair choices including clean-shaven. Hair and facial-hair colors are independent.
- Male and female body bases, six skin tones and selectable eye colors.
- Nine tops including an underlayer, six bottoms, four footwear choices, five headwear pieces plus none, three eyewear pieces plus none, and three glove styles plus bare hands.
- Twelve color choices for garments and grooming. Clothing is cosmetic; armor and equipment still use the inventory system.
- Six independent tattoo zones with 16 ink designs plus none; adjustable size, horizontal/vertical placement, rotation and overall ink strength. Tattoos are applied to skin, so clothing can cover them. The underlayer exposes the body for inspecting placement.
- Seven creator tabs, draggable sliders, rotation and three preview distances, appearance randomization, shape reset, five saved appearance presets and starting attribute allocation.

Creator previews use neutral lighting without the gameplay camera-decay filter. Gameplay retains the user's graphics/filter settings. Character creation still commits the selected appearance and starts the player in the bunker.

## Persistence and compatibility

Customization is stored in the existing FLWIdentity save record as separate reflected fields. Legacy identities are normalized before use. Presets store appearance only, in AAM_Appearance35_1 through AAM_Appearance35_5; they do not copy or overwrite world state.

LWNPCLife continues to handle facial/limb animation and defers to injuries and ragdolls. Its nearby render copies preserve the new customization masks and material overrides. Humanoid head landmarks and leg joints are calibrated to the new proportions.

## Editable assets and tools

- ArtSource/Characters35/AllAmericanMeltdown_Characters35.blend
- ArtSource/Characters35/models_35_manifest.json and SM_*.fbx
- Tools/make_characters35.py: builds the editable geometry library and exports.
- Tools/import_characters35.py: imports textures/materials/meshes, validates material connections and bounds, then enables CPU access for customization.
- Tools/render_characters35.py: untextured geometry review. For the actual game appearance, use the runtime captures.

New bitmap atlases were made with the built-in image-generation tool and copied into the project:

- ArtSource/Characters35/T_CharacterMaterials35.png: an eight-cell skin, cotton, denim, leather, hair, knit, rubber and canvas material atlas.
- ArtSource/Characters35/T_Tattoos35.png: a transparent 4×4 blackwork tattoo decal atlas.

The existing CharacterFaces32 atlas supplies fitted facial diffuse detail. The anatomical heads are derived from the CC0 MakeHuman hm08 graphical base mesh, reduced and remapped to the game. Source provenance, asset hashes and the full asset license are in ArtSource/Characters35/Reference. No MakeHuman application code is included.

Generation prompts are recorded in ArtSource/Characters35/TexturePrompts.md.

## Verification

LethalWorld.Creator35.MorphsAndSavedAppearance exercises geometry changes, bounds on customization values, deterministic NPC variation and an actual Unreal SaveGame memory round trip for clothing, grooming, tattoo and face values.

Run -LWV17Smoke -LWCreator35Smoke -LWAudioSmoke on /Game/Maps/Wasteland for the creator gallery, tattoo/wardrobe previews, appearance commit and world-NPC checks. Screenshots go to Saved/ScreenshotsV17/Creator35_*. The NPC animation smoke also checks the rebuilt rendering layer and ragdoll compatibility.

No game package is produced by these tools.

## Verified results, 2026-09-10

- LethalWorldEditor Win64 Development compiled successfully (Saved/BuildCharacters35Verified.log).
- All 96 current manifest meshes imported with validated bounds and material assignments (Saved/Characters35_FinalImport.log).
- 74 automation tests passed; zero failures or warnings in the test report (Saved/Creator35VerifiedReport/index.json).
- Creator runtime: 23 checks passed (Saved/Creator35_VerifiedRuntime.log).
- NPC animation, injuries and ragdolls: 15 checks passed (Saved/Creator35_NPCValidation.log).
- Reviewed creator face, wardrobe, tattoo and NPC speech/idle captures in Saved/ScreenshotsV17.
- Cook-directory configuration already includes /Game/Art and /Game/Materials. No game package was created.
