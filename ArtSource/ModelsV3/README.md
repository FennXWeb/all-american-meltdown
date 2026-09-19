# Original v3 model library

24 original meshes authored in Blender by Tools/make_models_v3.py. LethalWorld_V3.blend is the editable source; the manifest records pivots, geometry bounds, material slots and collision intent. Units are metres, with +X facing forward. The exporter corrects Unreal FBX handedness. Unreal imports validate bounds within 0.05 cm and resolve existing authored materials.

POIDoor uses a hinge at the origin and a 1.60 m panel along +X. Sign frames face +X; the procedural texture face mounts at +7 cm, spans Y +/-100 cm and Z +/-50 cm. Frame, illuminated marquee and pylon variants share this face convention. Room furniture has floor-level pivots. Each enemy has seven runtime components assembled from five independently exported body parts; left/right limb instances share meshes. Their local pivots are the articulation points used by physics constraints.

Regenerate with Blender 4.2: `blender --background --python Tools/make_models_v3.py`. Import through `Tools/build_v3_content.py` after compiling the game module. Do not run the original content rebuild just to update this library.
