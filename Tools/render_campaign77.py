import bpy,math
from pathlib import Path
from mathutils import Vector
root=Path('X:/LethalWorld/ArtSource/Campaign77');bpy.ops.wm.open_mainfile(filepath=str(root/'Campaign77.blend'))
lib={o.name[3:]:o for o in bpy.context.scene.objects if o.type=='MESH' and o.name.startswith('SM_')}
for o in bpy.context.scene.objects:o.hide_render=True
scene=bpy.context.scene;scene.render.engine='CYCLES';scene.cycles.samples=20;scene.render.resolution_x=1400;scene.render.resolution_y=1000;scene.render.resolution_percentage=100;scene.world.color=(.18,.18,.18)
bpy.ops.object.camera_add(location=(11,-15,10));cam=bpy.context.object;scene.camera=cam;cam.data.type='ORTHO'
for pos,power,size in [((2,-7,13),2600,9),((-8,2,9),2200,7),((5,9,10),2200,7)]:
 bpy.ops.object.light_add(type='AREA',location=pos);l=bpy.context.object;l.data.energy=power;l.data.size=size;l.rotation_euler=(-l.location).to_track_quat('-Z','Y').to_euler()
bpy.ops.mesh.primitive_plane_add(size=200,location=(0,0,-1.4));ground=bpy.context.object;m=bpy.data.materials.new('Review floor');m.diffuse_color=(.08,.09,.1,1);ground.data.materials.append(m)
carrier=[('CarrierHull77',(0,0,0),(0,0,0)),('CarrierSide77',(-2.48,0,1.88),(0,0,0)),('CarrierSide77',(2.48,0,1.88),(0,0,0)),('CarrierShutter77',(0,4.5,3.14),(0,0,0)),('CarrierShutter77',(0,-4.5,3.14),(0,0,0)),('CarrierRoof77',(0,0,4.5),(0,0,0)),('CarrierTurret77',(0,-2.5,4.66),(0,0,0)),('ExecutiveDesk77',(0,.9,1.9),(0,0,0)),('ExecutiveChair77',(0,2.2,1.9),(0,0,0)),('ContinuityFlag77',(-2.05,3.6,1.9),(0,0,0))]
for filename,models,scale in [('Carrier77_Exterior',carrier,17),('Carrier77_Office',[x for x in carrier if x[0] not in ['CarrierSide77','CarrierRoof77','CarrierTurret77','CarrierShutter77']],15),('Ferry77_Review',[('Ferry77',(0,0,0),(0,0,0)),('Gangway77',(0,-7.2,.1),(0,0,0))],20)]:
 copies=[]
 for name,pos,rot in models:
  src=lib[name];o=src.copy();o.data=src.data;bpy.context.collection.objects.link(o);o.location=pos;o.rotation_euler=tuple(math.radians(x) for x in rot);o.hide_render=False;copies.append(o)
 cam.rotation_euler=(Vector((0,0,1.5))-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.ortho_scale=scale;scene.render.filepath=str(root/(filename+'.png'));bpy.ops.render.render(write_still=True)
 for o in copies:bpy.data.objects.remove(o,do_unlink=True)
