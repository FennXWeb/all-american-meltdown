"""Anatomical replacement pass; run after build_models63 and before import."""
import bpy,bmesh,math,json,random,ast
from pathlib import Path
from mathutils import Vector,Matrix
from mathutils.bvhtree import BVHTree
ROOT=Path('X:/LethalWorld');OUT=ROOT/'ArtSource/Models63';data=json.loads((OUT/'manifest.json').read_text());records=data['assets'];parts=[]
bpy.ops.wm.open_mainfile(filepath=str(OUT/'Models63.blend'));bpy.context.preferences.filepaths.save_version=0
lib={o.name.split('.')[0][3:]:o for o in bpy.context.scene.objects if o.name.startswith('SM_') and o.type=='MESH'}
mats={k:bpy.data.materials['M_'+k+'63'] for k in ['Metal','Steel','Rubber','Lens','Glow','Skin','Fur','Hide','Chitin','Cloth','Bone','Polymer']}
# Shared export and mesh helpers; no top-level generation is re-executed.
tree=ast.parse((ROOT/'Tools/build_models63.py').read_text());exec(compile(ast.Module(body=[n for n in tree.body if isinstance(n,ast.FunctionDef)],type_ignores=[]),'helpers63','exec'))
def oldbounds(name):return next(r['bounds'] for r in records if r['name']==name)
def fit(o,b):
 vv=[v.co.copy() for v in o.data.vertices];lo=Vector(tuple(min(v[i] for v in vv) for i in range(3)));hi=Vector(tuple(max(v[i] for v in vv) for i in range(3)))
 for v in o.data.vertices:
  v.co=Vector(tuple(b['min'][i]+(v.co[i]-lo[i])/max(1e-5,hi[i]-lo[i])*(b['max'][i]-b['min'][i]) for i in range(3)))
def replace(name,unwrap=False):
 global records
 old=lib[name];bpy.data.objects.remove(old,do_unlink=True);records=[r for r in records if r['name']!=name];export(name,'npc',unwrap);lib[name]=next(o for o in bpy.context.scene.objects if o.name.split('.')[0]=='SM_'+name)
# Actual skin topology, including anatomical shoulders, chest and abdomen.
vs=[];fs=[];g=''
for line in (ROOT/'ArtSource/Characters35/Reference/base.obj').read_text().splitlines():
 if line.startswith('v '):x,z,y=map(float,line.split()[1:]);vs.append((y*.108-.06,x*.108,(z+8.1676)*.108-1.51))
 elif line.startswith('g '):g=line[2:]
 elif line.startswith('f ') and g=='body':fs.append(tuple(int(s.split('/')[0])-1 for s in line.split()[1:]))
for kind in ['Titan','Karen','Mannequin']:
 female=kind!='Titan';sex='Female' if female else 'Male'
 for part,source in [('Head',sex+'AnatomicalHead35'),('Torso',sex+'Top035'),('Pelvis',sex+'Waist135'),('Arm','Sleeve8L35' if kind=='Titan' else 'Sleeve0L35'),('Leg','NPCLeg3L35' if kind=='Titan' else 'NPCLeg0L35')]:
  name=kind+part+'32';bounds=oldbounds(name)
  if kind=='Karen' and part=='Pelvis':continue
  if part=='Torso' and kind in ['Titan','Mannequin']:
   o=mesh('Anatomical torso',vs,fs,'Skin' if kind=='Titan' else 'Polymer');bm=bmesh.new();bm.from_mesh(o.data)
   for co,no in [((0,0,-.46),(0,0,1)),((0,0,.03),(0,0,-1)),((0,-.18,0),(0,1,0)),((0,.18,0),(0,-1,0))]:bmesh.ops.bisect_plane(bm,geom=list(bm.verts)+list(bm.edges)+list(bm.faces),dist=1e-6,plane_co=co,plane_no=no,clear_inner=True)
   bmesh.ops.delete(bm,geom=[v for v in bm.verts if not v.link_faces],context='VERTS');bm.to_mesh(o.data);bm.free();o.data.uv_layers.new()
   for l in o.data.loops:
    p=o.data.vertices[l.vertex_index].co;o.data.uv_layers[0].data[l.index].uv=(p.y*2+.5,p.z*2+.5)
  else:o=duplicate(lib[source])
  if kind=='Mannequin':
   for i in range(len(o.data.materials)):o.data.materials[i]=mats['Polymer']
  # Weight the chest and shoulders rather than scaling primitives into muscles.
  if kind=='Titan' and part=='Torso':
   for v in o.data.vertices:
    p=v.co;p.x+=max(0,p.x)*.28*math.exp(-((p.z+.15)/.15)**2);p.y*=1+.12*math.exp(-((p.z+.04)/.08)**2)
  if kind=='Titan':
   for v in o.data.vertices:
    v.co.x*=1.35;v.co.y*=1.35
  else:fit(o,bounds)
  replace(name)
# Reptilian head based on real jaw/orbit topology, elongated muzzle and rooted horns.
name='DeathclawHead32';o=duplicate(lib['MaleAnatomicalHead35'])
for i,m in enumerate(o.data.materials):
 if 'Face' in m.name:o.data.materials[i]=mats['Skin']
for v in o.data.vertices:
 p=v.co;front=max(0,min(1,(p.x-.025)/.05));p.x+=front*.08*math.exp(-((p.z+.095)/.055)**2);p.y*=1+.12*math.exp(-((p.z+.12)/.06)**2)
fit(o,{'min':[-.20,-.20,-.227],'max':[.50,.20,.31]});o.data.update()
for sign in [-1,1]:tube([(-.07,sign*.145,.24),(-.13,sign*.20,.37),(-.24,sign*.22,.49),(-.32,sign*.2,.56)],0,'Bone',20,[.06,.047,.025,.001])
replace(name,False)
# Remove floating rib strips. Original body silhouette receives sculpted skin, not attached bars.
name='DeathclawTorso32';o=duplicate(lib[name]);bm=bmesh.new();bm.from_mesh(o.data)
# New detail strips are disconnected and thinner than the body's manifold islands.
visited=set();remove=[]
for v in bm.verts:
 if v in visited:continue
 stack=[v];group=[];visited.add(v)
 while stack:
  a=stack.pop();group.append(a)
  for e in a.link_edges:
   b=e.other_vert(a)
   if b not in visited:visited.add(b);stack.append(b)
 if len(group)<160 and min(v.co.x for v in group)>.15:remove.extend(group)
bmesh.ops.delete(bm,geom=remove,context='VERTS');bm.to_mesh(o.data);bm.free();replace(name,True)
# Dogs lacked any readable eye/nose material: add fitted eyeballs and nasal pad.
for kind in ['Dog','Moose']:
 name=kind+'Head32';o=duplicate(lib[name]);coords=[v.co for v in o.data.vertices];tree=BVHTree.FromPolygons(coords,[tuple(p.vertices) for p in o.data.polygons]);large=kind=='Moose'
 for sign in [-1,1]:
  x=.16 if large else .025;z=.06 if large else .06;hit=tree.ray_cast(Vector((x,sign*2,z)),Vector((0,-sign,0)))
  if hit[0]:
   p=hit[0];direction=Vector((.3,sign,.15)).normalized();radius=.023 if large else .011;tube([p-direction*.005,p+direction*.004],radius,'Rubber',24);tube([p+direction*.004,p+direction*.006],radius*.53,'Bone',24);tube([p+direction*.006,p+direction*.007],radius*.32,'Rubber',20)
 replace(name,True)
# Retain the scooter chassis as part of Karen's pelvis module.
with bpy.data.libraries.load(str(ROOT/'ArtSource/Characters32/AllAmericanMeltdown_Characters32.blend'),link=False) as (a,b):b.objects=[n for n in a.objects if n.startswith('SM_KarenPelvis32')]
o=duplicate(next(o for o in b.objects if o));
for i,m in enumerate(o.data.materials):o.data.materials[i]=mats['Rubber' if 'Leather' in m.name else 'Steel' if 'Metal' in m.name else 'Cloth']
replace('KarenPelvis32',True)
# Distinct mirrored limbs preserve thumb, palm and footwear orientation.
for kind in ['Titan','Karen','Mannequin']:
 for part in ['Arm','Leg']:
  base=kind+part+'32';name=kind+part+'R32';o=duplicate(lib[base])
  for v in o.data.vertices:v.co.y*=-1
  bm=bmesh.new();bm.from_mesh(o.data);bmesh.ops.reverse_faces(bm,faces=list(bm.faces));bm.to_mesh(o.data);bm.free()
  if name in lib:bpy.data.objects.remove(lib[name],do_unlink=True)
  records=[r for r in records if r['name']!=name];export(name,'npc',False)
data['assets']=records;(OUT/'manifest.json').write_text(json.dumps(data,indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'Models63.blend'));print('ANATOMY63_PASS')
