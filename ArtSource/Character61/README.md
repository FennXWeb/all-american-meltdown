# Survivor 61 — single-character approval sample

This is an isolated character art review for All American Meltdown. Existing Unreal character assets, gameplay code and saves have not been replaced. No package was built.

## Review files

- `Survivor61_Full.png`: actual Blender render of the dressed model.
- `Survivor61_Head.png`: actual face/hair close-up.
- `Survivor61_Back.png`: actual rear three-quarter view.
- `Survivor61_HairMotion.gif`: rendered spring-driven hair motion.
- `Survivor61_Approval.blend`: editable model, materials, packed texture images, hair rig and baked wind action.
- `Survivor61_Approval.fbx`: geometry and hair animation interchange export. Blender's layered material graphs are authoritative; production Unreal materials still require baking/import work after approval.
- `manifest.json`: generated geometry counts and scope.

The sample has a continuous anatomical skin mesh, fitted field jacket with modeled collar, four flap pockets, rolled cuffs and seam details, cargo trousers, belt, shaped boots, separate eyes, new face/material textures, and 96 layered hair cards. The anatomy derives from the project's existing CC0 MakeHuman hm08 source; license and provenance remain in `../Characters35/Reference/`.

Hair uses pinned roots and three-joint strands. A damped angular spring simulation is integrated at 120 Hz and baked at 24 fps for the four-second review action. This demonstrates movement on the real hair mesh; it is not yet a live Unreal hair physics component. The body remains in a neutral fitting pose. Body animation, facial expression rigging, gameplay hair collision, runtime material baking, LODs and crowd performance tuning are subsequent integration work, pending the user's approval of this one sample.

## Textures

Three new generated source images are included: face projection, four-material atlas (olive cotton, leather, skin and brown hair), and an eight-lock RGBA hair-card sheet. Face projection is blended into the skin at its edges. The source textures are packed in the Blender file.

## Rebuild

Run Blender 4.2 in background mode with `Tools/make_character61.py`, followed by `Tools/render_character61_motion.py` for motion frames. Only this approval directory is written. The source scripts use real mesh generation and offline rendering; the preview images are not image-generated concept renders.

**Stop after this sample. Do not roll the design out to other NPCs or the player until the user approves.**

## Validation

`Tools/check_character61.py` passed all seven checks: 96 hair locks, actual evaluated-mesh motion, bounded strand displacement, face UV alignment at the eyes, packed source images, finite geometry, and exported FBX presence. Maximum measured strand displacement between frames 1 and 48 was approximately 15 mm. The review GIF contains 24 actual rendered frames at six frames per second. Front, head and rear renders were visually inspected; these checks do not constitute Unreal runtime validation.
