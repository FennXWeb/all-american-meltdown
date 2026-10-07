import bpy,math
from pathlib import Path
from mathutils import Vector
root=Path('X:/LethalWorld/ArtSource/Arsenal62');bpy.ops.wm.open_mainfile(filepath=str(root/'MysticArsenal62.blend'))
for o in bpy.context.scene.objects:o.hide_render=True
scene=bpy.context.scene;scene.render.engine='CYCLES';scene.cycles.samples=24;scene.cycles.use_denoising=True;scene.render.resolution_x=1400;scene.render.resolution_y=850;scene.render.resolution_percentage=100;scene.world.color=(.08,.08,.08)
bpy.ops.object.camera_add(location=(.8,-1.9,1.15));cam=bpy.context.object;cam.rotation_euler=(Vector((.28,0,0))-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.type='ORTHO';cam.data.ortho_scale=1.25;scene.camera=cam
for loc,power,size in [((.4,-1,1.7),190,2),((.4,1,.9),140,1),((-.8,-.1,.3),90,1)]:
 bpy.ops.object.light_add(type='AREA',location=loc);l=bpy.context.object;l.data.energy=power;l.data.shape='DISK';l.data.size=size;l.rotation_euler=(Vector((.3,0,0))-l.location).to_track_quat('-Z','Y').to_euler()
for w in [1,2,5]:
 o=bpy.data.objects['SM_Mystic62_%02d'%w];o.hide_render=False;scene.render.filepath=str(root/('Mystic62_%02d_Preview.png'%w));bpy.ops.render.render(write_still=True);o.hide_render=True
