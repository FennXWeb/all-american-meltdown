import bpy,bmesh,math,random,json
from pathlib import Path
from mathutils import Vector,Matrix
from mathutils.bvhtree import BVHTree
ROOT=Path('X:/LethalWorld');OUT=ROOT/'ArtSource/Arsenal62';OUT.mkdir(exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False);bpy.context.preferences.filepaths.save_version=0
names=['Crowbar','Bat','Shotgun','Revolver','SniperBareV14','SMG','Rifle','LMG','DoubleBarrelV18','MissileLauncher24','Minigun24','SawedOff24','DesertEagle24','M424','Taser24','Flamethrower24','TenBarrel50','GiantGlock50','QuestionableAK50','FingerGuns50','BudgetCut50']
lib={}
for f in ['ModelsV2/LethalWorld_ModelsV2.blend','ModelsV3/LethalWorld_V3.blend','ModelsV14/LethalWorld_V14.blend','ModelsV18/AllAmericanMeltdown_V18.blend','ModelsV24/Arsenal24.blend','ModelsV50/Comedy50.blend']:
 with bpy.data.libraries.load(str(ROOT/'ArtSource'/f),link=False) as (a,b):b.objects=[n for n in a.objects if n.startswith('SM_')]
 for o in b.objects:
  if o:lib[o.name.split('.')[0].removeprefix('SM_')]=o
for n in ['Crowbar','Bat','Shotgun']:
 bpy.ops.import_scene.fbx(filepath=str(ROOT/'ArtSource/Models'/('SM_'+n+'.fbx')))
 o=next(o for o in bpy.context.selected_objects if o.type=='MESH');o.data.transform(o.matrix_world);o.matrix_world=Matrix.Identity(4);lib[n]=o
missing=[n for n in names if n not in lib]
if missing:raise RuntimeError('Missing originals '+str(missing))
colors=[(1,.11,.008),(.1,1,.005),(.48,.025,1),(.03,.55,1),(.38,.003,.007),(.01,.12,1),(1,.55,.025),(.02,.8,.55)]
themes=[2,4,0,2,3,1,0,5,0,6,1,4,6,5,5,1,0,6,2,7,1]
materials={}
atlas=bpy.data.images.load(str(OUT/'T_MysticAtlas62.png'));atlas.pack()
for i,c in enumerate(colors):
 for fx in [False,True]:
  n=('MysticFX62_' if fx else 'Mystic62_')+str(i);m=bpy.data.materials.new('M_'+n);m.use_nodes=True;bs=m.node_tree.nodes.get('Principled BSDF');bs.inputs['Base Color'].default_value=(*c,1) if fx else (.045,.045,.05,1);bs.inputs['Roughness'].default_value=.22 if fx else .6
  if fx:bs.inputs['Emission Color'].default_value=(*c,1);bs.inputs['Emission Strength'].default_value=2
  else:
   tex=m.node_tree.nodes.new('ShaderNodeTexImage');tex.image=atlas;m.node_tree.links.new(tex.outputs['Color'],bs.inputs['Base Color'])
  materials[n]=m
metal=bpy.data.materials.new('M_MysticWire62');metal.diffuse_color=(.22,.23,.24,1);materials['MysticWire62']=metal
records=[];parts=[]
def mesh(n,vs,fs,mat):
 me=bpy.data.meshes.new(n);me.from_pydata(vs,[],fs);me.materials.append(materials[mat]);o=bpy.data.objects.new(n,me);bpy.context.collection.objects.link(o);parts.append(o);return o

def tube(n,points,r,mat,sides=6):
 vs=[];fs=[]
 for j,p in enumerate(points):
  p=Vector(p);d=Vector(points[min(j+1,len(points)-1)])-Vector(points[max(0,j-1)]);d.normalize();a=d.cross(Vector((0,0,1)))
  if a.length<.01:a=d.cross(Vector((0,1,0)))
  a.normalize();b=d.cross(a)
  for k in range(sides):vs.append(p+r*(a*math.cos(k*math.tau/sides)+b*math.sin(k*math.tau/sides)))
 for j in range(len(points)-1):
  for k in range(sides):fs.append((j*sides+k,j*sides+(k+1)%sides,(j+1)*sides+(k+1)%sides,(j+1)*sides+k))
 fs.extend([tuple(range(sides-1,-1,-1)),tuple((len(points)-1)*sides+k for k in range(sides))]);return mesh(n,vs,fs,mat)

def crystal(p,size,mat,twist=0):
 p=Vector(p);vs=[]
 for ring,z,r in [(0,0,.7),(1,.35,1),(2,.75,.55)]:
  for k in range(5):a=k*math.tau/5+twist;vs.append(p+Vector((math.cos(a)*size*.38,math.sin(a)*size*.38,z*size)))
 vs.append(p+Vector((.15*size,0,size)));fs=[]
 for j in range(2):
  for k in range(5):fs.append((j*5+k,j*5+(k+1)%5,(j+1)*5+(k+1)%5,(j+1)*5+k))
 for k in range(5):fs.append((10+k,10+(k+1)%5,15))
 fs.append(tuple(range(4,-1,-1)));return mesh('Faceted crystal',vs,fs,mat)

def export(name,theme=-1):
 bpy.ops.object.select_all(action='DESELECT')
 for o in parts:o.select_set(True)
 bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();o=bpy.context.object;o.name='SM_'+name
 bm=bmesh.new();bm.from_mesh(o.data);bmesh.ops.dissolve_degenerate(bm,dist=1e-7,edges=list(bm.edges));bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(o.data);bm.free()
 bpy.ops.object.mode_set(mode='EDIT');bpy.ops.mesh.select_all(action='SELECT');bpy.ops.uv.smart_project(island_margin=.012);bpy.ops.object.mode_set(mode='OBJECT')
 # Pack actual material UVs within the selected atlas rectangle; Unreal uses the same source UVs.
 if theme>=0:
  for face in o.data.polygons:
   if o.data.materials[face.material_index].name.startswith('M_Mystic62_'):
    for li in face.loop_indices:
     uv=o.data.uv_layers.active.data[li].uv;uv.x=theme%4*.25+.006+uv.x*.238;uv.y=(1-theme//4)*.5+.008+uv.y*.484
 coords=[v.co.copy() for v in o.data.vertices];bounds={'min':[min(v[i] for v in coords) for i in range(3)],'max':[max(v[i] for v in coords) for i in range(3)]}
 copy=o.copy();copy.data=o.data.copy();bpy.context.collection.objects.link(copy)
 for v in copy.data.vertices:v.co.y*=-1
 bm=bmesh.new();bm.from_mesh(copy.data);bmesh.ops.reverse_faces(bm,faces=list(bm.faces));bm.to_mesh(copy.data);bm.free();bpy.ops.object.select_all(action='DESELECT');copy.select_set(True);bpy.context.view_layer.objects.active=copy
 bpy.ops.export_scene.fbx(filepath=str(OUT/(name+'.fbx')),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',global_scale=1,apply_unit_scale=True,bake_anim=False,mesh_smooth_type='FACE');bpy.data.objects.remove(copy,do_unlink=True)
 records.append({'name':name,'theme':theme,'bounds':bounds,'triangles':sum(len(p.vertices)-2 for p in o.data.polygons)});parts.clear();o.hide_render=True;print('ARSENAL62',name,flush=True)

for w,n in enumerate(names):
 random.seed(620+w);t=themes[w];body='Mystic62_'+str(t);fx='MysticFX62_'+str(t);o=lib[n].copy();o.data=o.data.copy();o.parent=None;o.matrix_world=Matrix.Identity(4);bpy.context.collection.objects.link(o);parts.append(o)
 o.data.materials.clear();o.data.materials.append(materials[body])
 for f in o.data.polygons:f.material_index=0
 # Hand-authored original silhouette is retained at the grip and action; outer armor is newly modeled.
 vs=[v.co for v in o.data.vertices];xmin=min(v.x for v in vs);xmax=max(v.x for v in vs);length=xmax-xmin;ymax=max(abs(v.y) for v in vs);zmax=max(v.z for v in vs)
 if t in [0,3,7]:
  for v in o.data.vertices:
   if v.co.z>-.025:
    v.co+=v.normal*(.002+.003*math.sin(v.co.x*131+v.co.z*87))
 tree=BVHTree.FromPolygons([v.co.copy() for v in o.data.vertices],[tuple(f.vertices) for f in o.data.polygons])
 def surface(x,z,side):
  hit=tree.ray_cast(Vector((x,side*.6,z)),Vector((0,-side,0)))[0]
  if hit is None:hit=tree.find_nearest(Vector((x,side*.06,z)))[0]
  return hit.copy()
 if w==1:
  # Real helical wire around the striking end and paired hooked barbs.
  path=[]
  for j in range(240):
   q=j/239;x=xmin+length*(.42+.53*q);a=q*math.tau*11;rad=.022+.026*min(1,max(0,(x-.29)/.35))+.005;path.append((x,math.cos(a)*rad,math.sin(a)*rad))
  tube('Wrapped wire',path,.0018,'MysticWire62')
  for j in range(8,232,12):
   c=Vector(path[j]);out=Vector((0,c.y,c.z)).normalized()
   for s in [-1,1]:tube('Hooked barb',[c-Vector((.008*s,0,0)),c+out*.02+Vector((.01*s,0,0)),c+out*.014+Vector((.018*s,0,0))],.0012,'MysticWire62')
 else:
  # Distinct armor ribs, raised conduits and crest silhouettes for each weapon.
  count=5+w%5
  for j in range(count):
   q=(j+1)/(count+1);x=xmin+length*(.2+.6*q)
   for side in [-1,1]:
    point=surface(x,min(zmax,.09)*.48,side);x,y,z=point.x,point.y,point.z
    tube('Raised conduit',[(x-.018,y,z-.016),(x,y*1.12,z+.025),(x+.022,y,z+.01)],.0025,fx)
    if t in [0,2,3,7]:crystal((x,y,z-.007),.025+(w%4)*.008,body if t==0 else fx,j*.7)
    else:
     tube('Armor cage',[(x-.026,y,z-.03),(x-.018,y*1.1,z+.032),(x+.021,y*1.1,z+.034),(x+.031,y,z-.02)],.005,body,8)
  if t in [0,3,7]:
   for side in [-1,1]:
    strip=[]
    for j in range(13):
     x=xmin+length*(.16+.70*j/12);p=surface(x,.02,side)
     for row in range(3):strip.append(p+Vector((0,side*(.003+(.011 if row==1 else 0)+random.random()*.003),(row-1)*.017)))
    faces=[]
    for j in range(12):
     for row in range(2):a=j*3+row;faces.extend([(a,a+1,a+3),(a+1,a+4,a+3)])
    mesh('Continuous fractured mantle',strip,faces,body)
  # Cores and orbital collars are offset from the sight line and grip.
  for side in [-1,1]:
   center=surface(xmin+length*.45,0,side)+Vector((0,side*.003,0))
   loop=[center+Vector((math.cos(a*math.tau/40)*.025,0,math.sin(a*math.tau/40)*.025)) for a in range(41)];tube('Power seal',loop,.0025,fx,6)
   crystal(center,.032,fx,w*.2)
 export('Mystic62_%02d'%w,t)
# Tapered liquid drop/rune particle, one centimetre at its widest point.
mesh('Liquid drop',[(math.cos(k*math.tau/10)*r,math.sin(k*math.tau/10)*r,z) for z,r in [(-.008,0),(-.004,.004),(0,.005),(.006,.002),(.018,0)] for k in range(10)],[(j*10+k,j*10+(k+1)%10,(j+1)*10+(k+1)%10,(j+1)*10+k) for j in range(4) for k in range(10)],'MysticFX62_0');export('MysticDrop62')
# Fitted attachment meshes with custom profiles and rail geometry.
for name,group in [('suppressor','Muzzle'),('brake','Muzzle'),('comp','Muzzle'),('heavy','Barrel'),('short','Barrel'),('fluted','Barrel'),('precision','Stock'),('lightstock','Stock'),('padded','Stock'),('tape','RearGrip'),('rubber','RearGrip'),('skeleton','RearGrip')]:
 if group in ['Muzzle','Barrel']:
  length=.16 if name=='suppressor' else .10 if group=='Barrel' else .07
  # Annular muzzle with a visible bore, no solid muzzle cap.
  vs=[];fs=[]
  for x,r in [(0,.014),(length*.12,.019),(length*.88,.019),(length,.015),(length,.008),(0,.008)]:
   for k in range(16):vs.append((x,math.cos(k*math.tau/16)*r,math.sin(k*math.tau/16)*r))
  for j in range(6):
   for k in range(16):fs.append((j*16+k,j*16+(k+1)%16,((j+1)%6)*16+(k+1)%16,((j+1)%6)*16+k))
  mesh(name,vs,fs,'MysticWire62')
  for j in range(4):tube('Machined rib',[(length*(j+1)/5,math.cos(k*math.tau/24)*.021,math.sin(k*math.tau/24)*.021) for k in range(25)],.0015,'MysticWire62')
 else:
  length=.14 if group=='Stock' else .06;profile=[(0,0),(-length*.8,.018),(-length,.0),(-length,-.09),(-length*.65,-.08),(-length*.55,-.045),(0,-.025)]
  vs=[(x,y,z) for y in [-.019,.019] for x,z in profile];k=len(profile);fs=[tuple(range(k-1,-1,-1)),tuple(range(k,k*2))]+[(i,(i+1)%k,(i+1)%k+k,i+k) for i in range(k)];mesh(name,vs,fs,'MysticWire62')
  for j in range(5):tube('Grip rib',[(-length*.7,-.021,-j*.014),(-length*.7,.021,-j*.014)],.002,'MysticWire62')
 export('att_'+name+'62')
(OUT/'manifest.json').write_text(json.dumps(records,indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'MysticArsenal62.blend'));print('ARSENAL62_DONE',len(records))
