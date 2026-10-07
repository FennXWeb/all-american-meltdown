import bpy,math
from pathlib import Path
from mathutils import Vector
root=Path(__file__).resolve().parents[1];out=root/'ArtSource/ModelsV57'
bpy.ops.wm.open_mainfile(filepath=str(out/'Expansion57.blend'))
for o in bpy.data.objects:o.hide_render=True
def show(name,p=(0,0,0),rot=0):
 src=bpy.data.objects['SM_'+name];o=src.copy();o.data=src.data;bpy.context.collection.objects.link(o);o.hide_render=False;o.location=p;o.rotation_euler.z=rot;return o
for i,k in enumerate(['apc','armoredtruck','technical','helicopter']):
 p=Vector((0,i*10,0));show('Vehicle42_'+k,p);show('Cabin42_'+k,p);show('Windshield42_'+k,p)
 if k in ['apc','technical']:
  t=show('Turret57',p+Vector((-1.7,0,2.25 if k=='apc' else 1.43)));t.scale=(1,1,1) if k=='apc' else (.65,.65,.65);b=show('Cannon57' if k=='apc' else 'MachineGun57',p+Vector((-1.4,0,2.40 if k=='apc' else 1.53)));b.scale=t.scale
 if k=='helicopter':show('Rotor57',p+Vector((-1,0,3.05)))
bpy.ops.object.camera_add(location=(16,-20,23));cam=bpy.context.object;cam.rotation_euler=(Vector((-1,14,1))-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.type='ORTHO';cam.data.ortho_scale=39;bpy.context.scene.camera=cam
for p,power,size in [((3,10,22),5500,14),((-10,20,14),4000,10),((8,-10,8),3000,8)]:
 bpy.ops.object.light_add(type='AREA',location=p);l=bpy.context.object;l.data.energy=power;l.data.shape='DISK';l.data.size=size;l.rotation_euler=(Vector((0,14,0))-l.location).to_track_quat('-Z','Y').to_euler()
s=bpy.context.scene;s.world.color=(.3,.3,.3);s.render.engine='CYCLES';s.cycles.samples=24;s.render.resolution_x=1600;s.render.resolution_y=1400;s.render.resolution_percentage=100;s.render.filepath=str(out/'vehicle_preview57.png');bpy.ops.render.render(write_still=True)
