import bpy,math
from pathlib import Path
from mathutils import Vector
ROOT=Path(__file__).resolve().parents[1];OUT=ROOT/'Saved/Vehicle42Previews';OUT.mkdir(parents=True,exist_ok=True)
bpy.ops.wm.open_mainfile(filepath=str(ROOT/'ArtSource/ModelsV42/VehicleOverhaul42.blend'))
scene=bpy.context.scene;scene.render.engine='CYCLES';scene.cycles.samples=20;scene.render.resolution_x=1200;scene.render.resolution_y=720;scene.render.resolution_percentage=100
scene.world.color=(.22,.22,.22)
for m in bpy.data.materials:
 sh=m.node_tree.nodes.get('Principled BSDF') if m.use_nodes else None
 if sh:
  if 'Paint' in m.name:sh.inputs['Roughness'].default_value=.33;sh.inputs['Metallic'].default_value=.48
  if 'Glass' in m.name:sh.inputs['Base Color'].default_value=(.07,.11,.13,1);sh.inputs['Alpha'].default_value=.18;sh.inputs['Roughness'].default_value=.18
for o in scene.objects:o.hide_render=True
bpy.ops.mesh.primitive_plane_add(size=200,location=(0,0,-.01));floor=bpy.context.object;floor.hide_render=False
for loc,power,size in [((5,-7,8),2200,7),((-4,5,7),2800,6),((0,-2,5),900,4)]:
 bpy.ops.object.light_add(type='AREA',location=loc);o=bpy.context.object;o.data.energy=power;o.data.shape='DISK';o.data.size=size;o.rotation_euler=(Vector((0,0,1))-o.location).to_track_quat('-Z','Y').to_euler()
bpy.ops.object.camera_add();cam=bpy.context.object;scene.camera=cam;cam.data.lens=47
specs=[('sedan',2.05,2.7,.87),('police',2.3,2.9,.95),('boxtruck',3.8,4.8,1.15),('rv',6.05,7.37,1.28),('bus',4.9,6.4,1.2),('van',2.75,3.4,1.05),('pickup',2.7,3.5,1.05),('dirtbike',1.15,1.45,.34),('suv',2.45,3.,1.03),('muscle',2.35,2.85,1),('supercar',2.25,2.7,1)]
for name,L,wb,w in specs:
 body=bpy.data.objects['SM_Vehicle42_'+name];cab=bpy.data.objects['SM_Cabin42_'+name];body.hide_render=cab.hide_render=False
 glass=bpy.data.objects.get('SM_Windshield42_'+name)
 if glass:glass.hide_render=False
 wheel=bpy.data.objects.get('SM_Wheel42_'+name) or bpy.data.objects['SM_Wheel42_sedan'];clones=[]
 for x in ([.72,-.72] if name=='dirtbike' else [1.32,1.32-wb]+([-7.55] if name=='rv' else [])):
  for side in ([0] if name=='dirtbike' else [-1,1]):
   ob=wheel.copy();ob.data=wheel.data;scene.collection.objects.link(ob);ob.location=(x,side*(w-.08),.43 if name=='rv' else .34);ob.hide_render=False;clones.append(ob)
 center=Vector((2.2-L,0,.95 if L<3 else 1.6));cam.location=center+Vector((L*1.5+1,-L*1.65-1,L*.7+1));cam.rotation_euler=(center-cam.location).to_track_quat('-Z','Y').to_euler();scene.render.filepath=str(OUT/(name+'.png'));bpy.ops.render.render(write_still=True)
 body.hide_render=cab.hide_render=True
 if glass:glass.hide_render=True
 for ob in clones:bpy.data.objects.remove(ob,do_unlink=True)
print('VEHICLE42_PREVIEWS_DONE')
