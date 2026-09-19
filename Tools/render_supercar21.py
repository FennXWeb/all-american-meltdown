import bpy,math
from mathutils import Vector
from pathlib import Path
root=Path('X:/LethalWorld');bpy.ops.wm.open_mainfile(filepath=str(root/'ArtSource/ModelsV21/CasinoSupercarV21.blend'))
for o in bpy.data.objects:o.hide_render=o.name!='SM_SupercarV21'
wheel=bpy.data.objects['SM_SuperWheelV21']
for x in [1.32,-1.38]:
 for y in [-.92,.92]:
  o=wheel.copy();o.data=wheel.data;bpy.context.collection.objects.link(o);o.location=(x,y,.35);o.hide_render=False
bpy.ops.mesh.primitive_plane_add(size=200);plane=bpy.context.object
mat=bpy.data.materials.new('Ground');mat.diffuse_color=(.065,.07,.075,1);plane.data.materials.append(mat)
for loc,power,size in [((3,-4,7),1800,5),((-4,-2,5),1300,4),((0,5,6),2200,5)]:
 bpy.ops.object.light_add(type='AREA',location=loc);o=bpy.context.object;o.data.energy=power;o.data.shape='DISK';o.data.size=size;o.rotation_euler=(Vector((0,0,.6))-o.location).to_track_quat('-Z','Y').to_euler()
bpy.ops.object.camera_add(location=(6.2,-6.7,3.1));cam=bpy.context.object;cam.rotation_euler=(Vector((0,0,.75))-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.lens=52
sc=bpy.context.scene;sc.camera=cam;sc.render.engine='CYCLES';sc.cycles.samples=24;sc.render.resolution_x=1200;sc.render.resolution_y=800;sc.render.resolution_percentage=100;sc.world.color=(.15,.15,.15);sc.render.filepath=str(root/'ArtSource/ModelsV21/Supercar_preview.png');bpy.ops.render.render(write_still=True)
