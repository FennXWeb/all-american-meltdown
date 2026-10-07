import bpy,math
from mathutils import Vector
from pathlib import Path
root=Path('X:/LethalWorld/ArtSource/Interiors65')
bpy.ops.wm.open_mainfile(filepath=str(root/'Interiors65.blend'))
lib={o.name[3:]:o for o in bpy.context.scene.objects if o.type=='MESH' and o.name.startswith('SM_')}
for o in bpy.context.scene.objects:o.hide_render=True
scene=bpy.context.scene;scene.render.engine='CYCLES';scene.cycles.samples=24;scene.render.resolution_x=1600;scene.render.resolution_y=1000;scene.render.resolution_percentage=100;scene.world.color=(.18,.18,.18)
bpy.ops.object.camera_add(location=(5,-8,6));cam=bpy.context.object;scene.camera=cam;cam.data.type='ORTHO';cam.data.ortho_scale=11
for pos,power,size in [((2,-4,7),1600,6),((-5,1,5),1400,5),((3,5,5),1200,5)]:
 bpy.ops.object.light_add(type='AREA',location=pos);l=bpy.context.object;l.data.energy=power;l.data.shape='DISK';l.data.size=size;l.rotation_euler=(-l.location).to_track_quat('-Z','Y').to_euler()
bpy.ops.mesh.primitive_plane_add(size=200);ground=bpy.context.object;ground.location.z=-.01;m=bpy.data.materials.new('Review floor');m.diffuse_color=(.075,.085,.09,1);ground.data.materials.append(m)
sets=[('Interiors65_Collection', [('Workstation65',(-3,-1,0)),('Chair65',(-3,-2,0)),('Sofa65',(0,-1,0)),('CoffeeTable65',(0,-2.3,0)),('Books65',(0,-2.3,.47)),('ToolTrolley65',(2.3,-1,0)),('ExamCart65',(3.6,-1,0)),('Bookcase65',(-3,1.1,0)),('Locker65',(-1.5,1.1,0)),('Sideboard65',(.2,1.1,0)),('Fridge65',(2,1.1,0)),('Stove65',(3,1.1,0)),('Sink65',(4,1.1,0)),('CleaningCart65',(-4,0,0))]),('Interiors65_Lounge',[('Sofa65',(0,.5,0)),('Armchair65',(-1.5,-.8,0)),('CoffeeTable65',(.3,-.7,0)),('Books65',(.3,-.7,.47)),('Sideboard65',(2.1,.7,0)),('CoffeeStation65',(3.4,.7,0)),('Planter65',(-1.8,.8,0))])]
for filename,models in sets:
 copies=[]
 for name,pos in models:
  src=lib[name];o=src.copy();o.data=src.data;bpy.context.collection.objects.link(o);o.location=pos;o.hide_render=False;copies.append(o)
 bpy.context.view_layer.update();cam.location=(6,-10,7);target=Vector((0,0,.6));cam.rotation_euler=(target-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.ortho_scale=12 if len(models)>10 else 8;scene.render.filepath=str(root/(filename+'.png'));bpy.ops.render.render(write_still=True)
 for o in copies:bpy.data.objects.remove(o,do_unlink=True)
