import bpy,math,json
from pathlib import Path
from mathutils import Vector
root=Path('X:/LethalWorld/ArtSource/Models63');bpy.ops.wm.open_mainfile(filepath=str(root/'Models63.blend'))
lib={o.name.split('.')[0][3:]:o for o in bpy.context.scene.objects if o.type=='MESH' and o.name.startswith('SM_')}
for o in bpy.context.scene.objects:o.hide_render=True
atlas=bpy.data.images.load(str(root/'T_Surface63.png'))
for key,tile in {'Skin':0,'Fur':1,'Hide':2,'Chitin':3,'Metal':4,'Cloth':5,'Bone':6,'Polymer':7}.items():
 m=bpy.data.materials.get('M_'+key+'63');m.use_nodes=True;n=m.node_tree.nodes;l=m.node_tree.links;bs=n.get('Principled BSDF');uv=n.new('ShaderNodeTexCoord');mapping=n.new('ShaderNodeVectorMath');mapping.operation='MULTIPLY_ADD';mapping.inputs[1].default_value=(.244,.488,1);mapping.inputs[2].default_value=(tile%4*.25+.003,(1-tile//4)*.5+.006,0);tex=n.new('ShaderNodeTexImage');tex.image=atlas;l.new(uv.outputs['UV'],mapping.inputs[0]);l.new(mapping.outputs[0],tex.inputs[0]);l.new(tex.outputs['Color'],bs.inputs['Base Color'])
sets=[('Dog','32',[(0,0,0),(.4,0,.06),(-.28,0,0),(.25,-.13,-.03),(.25,.13,-.03),(-.28,-.12,-.03),(-.28,.12,-.03)]),('Moose','32',[(0,0,.2),(.75,0,.55),(-.65,0,.15),(.6,-.3,.05),(.6,.3,.05),(-.7,-.28,.05),(-.7,.28,.05)]),('Titan','32',[(0,0,.63),(0,0,.824),(0,0,.12),(0,-.243,.58),(0,.243,.58),(0,-.1215,-.01),(0,.1215,-.01)]),('Deathclaw','32',[(0,0,.45),(0,0,.8),(0,0,0),(0,-.33,.55),(0,.33,.55),(0,-.16,-.2),(0,.16,-.2)]),('Scorpion','32',[(0,0,-.2),(.7,0,-.15),(-.6,0,-.2),(.6,-.45,-.2),(.6,.45,-.2),(-.2,-.3,-.2),(-.2,.3,-.2)]),('Bear','57',[(0,0,0),(.65,0,.15),(-.7,0,-.05),(.38,-.38,-.10),(.38,.38,-.1),(-.65,-.33,-.05),(-.65,.33,-.05)]),('Rogue','57',[(0,0,.37),(0,0,.88),(0,0,.07),(0,-.31,.55),(0,.31,.55),(0,-.14,-.07),(0,.14,-.07)]),('Hornet','57',[(0,0,0),(.32,0,0),(-.39,0,0),(.08,-.23,-.12),(.08,.23,-.12),(-.2,-.2,-.12),(-.2,.2,-.12)])]
# Render individual turntable-like views; each uses the actual exported modular rest pose.
scene=bpy.context.scene;scene.render.engine='CYCLES';scene.cycles.samples=24;scene.render.resolution_x=960;scene.render.resolution_y=960;scene.render.resolution_percentage=100;scene.world.color=(.13,.13,.13)
bpy.ops.object.camera_add(location=(4,-6,2.8));cam=bpy.context.object;scene.camera=cam;cam.data.type='ORTHO'
for pos,power,size in [((3,-4,5),850,5),((-2,3,3),1100,4),((0,0,5),500,3)]:
 bpy.ops.object.light_add(type='AREA',location=pos);lamp=bpy.context.object;lamp.data.energy=power;lamp.data.shape='DISK';lamp.data.size=size;lamp.rotation_euler=(-lamp.location).to_track_quat('-Z','Y').to_euler()
for kind,suffix,positions in sets:
 objs=[]
 for index,(part,at) in enumerate(zip(['Torso','Head','Pelvis','Arm','Arm','Leg','Leg'],positions)):
  if kind=='Titan' and index in [4,6]:part+='R'
  src=lib[kind+part+suffix];o=src.copy();o.data=src.data;bpy.context.collection.objects.link(o);o.location=at;o.hide_render=False;objs.append(o)
 coords=[o.matrix_world@Vector(v) for o in objs for v in o.bound_box];bpy.context.view_layer.update();coords=[o.matrix_world@Vector(v) for o in objs for v in o.bound_box];center=sum(coords,Vector())/len(coords);height=max(v.z for v in coords)-min(v.z for v in coords);width=max(v.x for v in coords)-min(v.x for v in coords);cam.location=center+Vector((4,-6,2.6));cam.rotation_euler=(center-cam.location).to_track_quat('-Z','Y').to_euler();cam.data.ortho_scale=max(height,width,1)*1.5
 scene.render.filepath=str(root/(kind+'_Review63.png'));bpy.ops.render.render(write_still=True)
 for o in objs:bpy.data.objects.remove(o,do_unlink=True)
