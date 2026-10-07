"""Authored UV sign panel and sewn ridge tent for update 78. Dimensions in metres."""
import bpy, math, bmesh
from mathutils import Vector, Matrix
from pathlib import Path
out=Path('X:/LethalWorld/ArtSource/World78');out.mkdir(exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False)
bpy.context.preferences.filepaths.save_version=0
mats={}
for name,color in [('Sign',(.05,.15,.18,1)),('Frame',(.07,.085,.09,1)),('Canvas',(.28,.32,.19,1)),('Rubber',(.025,.03,.025,1)),('Rope',(.49,.41,.28,1))]:
 m=bpy.data.materials.new('M_'+name+'78');m.diffuse_color=color;m.use_nodes=True;m.node_tree.nodes['Principled BSDF'].inputs['Base Color'].default_value=color;mats[name]=m
parts=[]
def mesh(v,f,mat):
 me=bpy.data.meshes.new(mat);me.from_pydata(v,[],f);me.update();o=bpy.data.objects.new(mat,me);bpy.context.collection.objects.link(o);o.data.materials.append(mats[mat]);parts.append(o);return o
def rod(a,b,r,mat='Frame'):
 a,b=Vector(a),Vector(b);bpy.ops.mesh.primitive_cylinder_add(vertices=8,radius=r,depth=(b-a).length,location=(a+b)/2);o=bpy.context.object;o.rotation_euler=(b-a).to_track_quat('Z','Y').to_euler();o.data.materials.append(mats[mat]);parts.append(o)
def export(name):
 bpy.ops.object.select_all(action='DESELECT')
 for o in parts:o.select_set(True)
 bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();o=bpy.context.object;o.name='SM_'+name;o.data.transform(o.matrix_world);o.matrix_world=Matrix.Identity(4)
 bpy.ops.export_scene.fbx(filepath=str(out/(name+'.fbx')),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',bake_anim=False,mesh_smooth_type='FACE')
 o.hide_set(True);parts.clear();return o
# One plane fitted into a shallow, bevelled painted-steel frame. Front normal +X.
o=mesh([(0,-.5,-.125),(0,.5,-.125),(0,.5,.125),(0,-.5,.125)],[(0,1,2,3)],'Sign')
u=o.data.uv_layers.new();coords=[(0,0),(1,0),(1,1),(0,1)]
for p in o.data.polygons:
 for k,li in enumerate(p.loop_indices):u.data[li].uv=coords[k]
for y in [-.502,.502]:rod((-.005,y,-.127),(-.005,y,.127),.005)
for z in [-.127,.127]:rod((-.005,-.502,z),(-.005,.502,z),.005)
export('SignPanel78')
# Shaped cloth panels with tension and sag, open entrance and sewn pole sleeves.
for side in [-1,1]:
 v=[];f=[]
 for j in range(13):
  y=-1.3+j*2.6/12
  for i in range(9):
   t=i/8;x=side*(1.0*t+.035*math.sin(t*math.pi)*math.cos(y*2));z=1.45*(1-t)+.06-.075*math.sin(t*math.pi)*math.cos(y*.9)
   v.append((x,y,z))
 for j in range(12):
  for i in range(8):a=j*9+i;f.append((a,a+1,a+10,a+9) if side<0 else (a+9,a+10,a+1,a))
 o=mesh(v,f,'Canvas');sol=o.modifiers.new('Double stitched canvas','SOLIDIFY');sol.thickness=.006;bpy.context.view_layer.objects.active=o;o.select_set(True);bpy.ops.object.modifier_apply(modifier=sol.name);o.select_set(False)
mesh([(-1,1.3,.06),(1,1.3,.06),(0,1.3,1.51)],[(0,1,2)],'Canvas')
mesh([(-1,-1.3,.05),(1,-1.3,.05),(1,1.3,.05),(-1,1.3,.05)],[(0,1,2,3)],'Rubber')
# Rolled front flaps leave a usable opening, no solid fake door.
for side in [-1,1]:
 rod((side*.82,-1.305,.18),(side*.24,-1.305,1.16),.075,'Canvas')
 for y in [-1.31,1.31]:
  rod((side*1,y,.05),(0,y,1.52),.012)
  rod((side*.7,y,.48),(side*1.48,y*1.18,.04),.006,'Rope')
  rod((side*1.48,y*1.18,-.06),(side*1.48,y*1.18,.11),.014)
rod((0,-1.31,1.52),(0,1.31,1.52),.014)
# UV all tent parts for existing woven cloth atlas; materials retain individual slots.
for o in parts:
 bpy.ops.object.select_all(action='DESELECT');o.select_set(True);bpy.context.view_layer.objects.active=o;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);bpy.ops.object.mode_set(mode='EDIT');bpy.ops.mesh.select_all(action='SELECT');bpy.ops.uv.smart_project(island_margin=.025);bpy.ops.object.mode_set(mode='OBJECT')
export('TrailTent78')
for o in bpy.data.objects:o.hide_set(False)
bpy.ops.wm.save_as_mainfile(filepath=str(out/'World78.blend'))
print('WORLD78_MESHES_READY')
