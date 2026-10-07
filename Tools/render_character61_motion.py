"""Render the actual sample's baked spring-driven hair, without image generation."""
import bpy, math
from pathlib import Path
from mathutils import Vector
out=Path('X:/LethalWorld/ArtSource/Character61')
bpy.ops.wm.open_mainfile(filepath=str(out/'Survivor61_Approval.blend'))
sc=bpy.context.scene;cam=sc.camera
cam.location=(2.7,-1.65,1.93);cam.rotation_euler=(Vector((.07,0,1.70))-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.ortho_scale=.40
sc.render.engine='CYCLES';sc.cycles.samples=12;sc.render.resolution_x=560;sc.render.resolution_y=560;sc.render.resolution_percentage=100
(out/'MotionFrames').mkdir(exist_ok=True)
for frame in range(1,97,4):
 sc.frame_set(frame);sc.render.filepath=str(out/'MotionFrames'/('%03d.png'%frame));bpy.ops.render.render(write_still=True)
print('HAIR_MOTION61_COMPLETE')
