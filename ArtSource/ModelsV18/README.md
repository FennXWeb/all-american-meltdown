# Update 18 mesh sources

Regenerate with Blender 4.2 using `Tools/make_models_v18.py`. The editable scene is `AllAmericanMeltdown_V18.blend`. Import into Unreal with `Tools/build_v18_content.py` after compiling the editor target.

The 35 exported meshes comprise two shotgun assemblies, eight furniture/detail models, and five multipart creatures with five unique meshes each. Body part meshes are reused for paired limbs. All FBX exports retain authored origins. The manifest records bounds and material assignments for import validation.

- Furniture is floor-anchored, in metres in Blender and centimetres after FBX import; actor scale is one. Furniture uses complex static collision.
- Creature parts use convex collision for death physics. Runtime transforms, body proportions, capsule sizes and limb animation are in `Source/LethalWorld/LWCreatures.cpp`. Creature actors face positive X.
- The shotgun receiver and barrel assembly are separate. Runtime hinge transform and hand poses are in `Source/LethalWorld/LWWeapons.cpp`. The moving barrel pivot is ten centimetres forward of the receiver origin.
- The arcade mural diffuse texture is in `ArtSource/TexturesV18`. Its generation prompt is recorded in `Docs/Update18.md`.

These are original low-poly meshes designed for the game's degraded visual style.
