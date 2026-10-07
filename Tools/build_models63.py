"""Revision 63: landmark-fitted heads, rebuilt NPC surfaces and machined attachments.
Units metres; existing joint origins and save appearance indices remain stable.
"""
import bpy,bmesh,math,json,random
from pathlib import Path
from mathutils import Vector,Matrix
ROOT=Path('X:/LethalWorld');OUT=ROOT/'ArtSource/Models63';OUT.mkdir(exist_ok=True)
bpy.ops.object.select_all(action='SELECT');bpy.ops.object.delete(use_global=False);bpy.context.preferences.filepaths.save_version=0
random.seed(63);records=[];parts=[];mats={}
for name,color in [('Metal',(.065,.075,.08)),('Steel',(.35,.39,.42)),('Rubber',(.025,.023,.02)),('Lens',(.04,.20,.23)),('Glow',(.8,.05,.008)),('Skin',(.38,.42,.29)),('Fur',(.24,.12,.045)),('Hide',(.30,.23,.12)),('Chitin',(.24,.14,.028)),('Cloth',(.24,.29,.18)),('Bone',(.73,.67,.48)),('Polymer',(.6,.53,.43))]:
 m=bpy.data.materials.new('M_'+name+'63');m.use_nodes=True;m.diffuse_color=(*color,1);bs=m.node_tree.nodes.get('Principled BSDF');bs.inputs['Base Color'].default_value=(*color,1);bs.inputs['Roughness'].default_value=.7 if name in ['Skin','Fur','Hide','Cloth'] else .3;mats[name]=m

def activate(o):
 bpy.ops.object.select_all(action='DESELECT');o.select_set(True);bpy.context.view_layer.objects.active=o

def load(path,predicate):
 with bpy.data.libraries.load(str(ROOT/'ArtSource'/path),link=False) as (a,b):b.objects=[n for n in a.objects if predicate(n)]
 return [o for o in b.objects if o and o.type=='MESH']

def duplicate(o):
 c=o.copy();c.data=o.data.copy();c.parent=None;c.matrix_world=Matrix.Identity(4);c.hide_render=False;c.hide_viewport=False;bpy.context.collection.objects.link(c);parts.append(c);return c

def mesh(n,vs,fs,mat):
 me=bpy.data.meshes.new(n);me.from_pydata(vs,[],fs);me.update();me.materials.append(mats[mat]);o=bpy.data.objects.new(n,me);bpy.context.collection.objects.link(o);parts.append(o);return o

def tube(points,r,mat='Steel',sides=10,radii=None):
 vs=[];fs=[]
 for j,p in enumerate(points):
  p=Vector(p);d=(Vector(points[min(j+1,len(points)-1)])-Vector(points[max(0,j-1)])).normalized();a=d.cross(Vector((0,0,1)))
  if a.length<.01:a=d.cross(Vector((0,1,0)))
  a.normalize();b=d.cross(a);radius=radii[j] if radii else r
  for k in range(sides):vs.append(p+radius*(a*math.cos(k*math.tau/sides)+b*math.sin(k*math.tau/sides)))
 for j in range(len(points)-1):
  for k in range(sides):fs.append((j*sides+k,j*sides+(k+1)%sides,(j+1)*sides+(k+1)%sides,(j+1)*sides+k))
 fs.extend([tuple(range(sides-1,-1,-1)),tuple((len(points)-1)*sides+k for k in range(sides))]);return mesh('Curved surface',vs,fs,mat)

def box(p,size,mat='Metal',bevel=.001):
 bpy.ops.mesh.primitive_cube_add(size=1,location=p);o=bpy.context.object;o.scale=size;bpy.ops.object.transform_apply(location=False,rotation=False,scale=True);o.data.materials.append(mats[mat]);parts.append(o)
 if bevel:
  b=o.modifiers.new('Machined chamfer','BEVEL');b.width=bevel;b.segments=3;bpy.ops.object.modifier_apply(modifier=b.name)
 return o

def ring(x,z,outer,inner,depth,mat='Metal',sides=40):
 vs=[];fs=[]
 for xx,rr in [(x-depth/2,outer),(x+depth/2,outer),(x-depth/2,inner),(x+depth/2,inner)]:
  vs.extend((xx,math.cos(k*math.tau/sides)*rr,z+math.sin(k*math.tau/sides)*rr) for k in range(sides))
 for k in range(sides):
  n=(k+1)%sides
  for a,b in [(0,1),(2,0),(1,3),(3,2)]:fs.append((a*sides+k,a*sides+n,b*sides+n,b*sides+k))
 return mesh('Hollow machined aperture',vs,fs,mat)

def export(name,group='npc',unwrap=True):
 global parts
 if not parts:return
 bpy.ops.object.select_all(action='DESELECT')
 for o in parts:o.hide_set(False);o.select_set(True)
 bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();o=bpy.context.object;o.name='SM_'+name
 # Apply locations of individual details while retaining the original part origin.
 o.data.transform(o.matrix_world);o.matrix_world=Matrix.Identity(4)
 bm=bmesh.new();bm.from_mesh(o.data);bmesh.ops.dissolve_degenerate(bm,dist=1e-7,edges=list(bm.edges));bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(o.data);bm.free()
 if group=="npc":
  budget=8000 if "Head" in name else 6000 if "Torso" in name else 4000
  tris=sum(len(p.vertices)-2 for p in o.data.polygons)
  if tris>budget:
   activate(o);dec=o.modifiers.new("NPC triangle budget","DECIMATE");dec.ratio=budget/tris;bpy.ops.object.modifier_apply(modifier=dec.name)
 if unwrap:
  activate(o);bpy.ops.object.mode_set(mode='EDIT');bpy.ops.mesh.select_all(action='SELECT');bpy.ops.uv.smart_project(angle_limit=1.1,island_margin=.012);bpy.ops.object.mode_set(mode='OBJECT')
 for f in o.data.polygons:f.use_smooth=True
 coords=[v.co for v in o.data.vertices];bounds={'min':[min(v[i] for v in coords) for i in range(3)],'max':[max(v[i] for v in coords) for i in range(3)]}
 c=o.copy();c.data=o.data.copy();bpy.context.collection.objects.link(c)
 for v in c.data.vertices:v.co.y*=-1
 bm=bmesh.new();bm.from_mesh(c.data);bmesh.ops.reverse_faces(bm,faces=list(bm.faces));bm.to_mesh(c.data);bm.free();activate(c)
 bpy.ops.export_scene.fbx(filepath=str(OUT/(name+'.fbx')),use_selection=True,object_types={'MESH'},axis_forward='-Y',axis_up='Z',global_scale=1,apply_unit_scale=True,bake_anim=False,mesh_smooth_type='FACE');bpy.data.objects.remove(c,do_unlink=True)
 records.append(dict(name=name,group=group,bounds=bounds,triangles=sum(len(p.vertices)-2 for p in o.data.polygons)));parts=[];o.hide_render=True;print('MODEL63',name,flush=True)

# Preserve the approved anatomy and tailoring; rebuild the exported surface library.
humans=load('Characters61/Characters61_Runtime.blend',lambda n:n.startswith('SM_'))
expected={r['name'] for r in json.loads((ROOT/'ArtSource/Characters61/manifest.json').read_text())['assets']};seen=set()
for source in humans:
 name=source.name.split('.')[0][3:]
 if name not in expected or name in seen:continue
 seen.add(name);o=duplicate(source)
 if 'AnatomicalHead' in name:
  female='Female' in name
  # Geometry and texture landmarks have sex-specific positions. Preserve UVs on eyes.
  rows=[(-.20,0),(-.16,.045),(-.111,.218 if female else .232),(-.078,.350 if female else .365),(-.035212,.553 if female else .556),(.096,1)]
  def facev(z):
   for a,b in zip(rows,rows[1:]):
    if a[0]<=z<=b[0]:return a[1]+(b[1]-a[1])*(z-a[0])/(b[0]-a[0])
   return max(0,min(1,(z+.20)/.296))
  for p in o.data.polygons:
   if 'Face' not in o.data.materials[p.material_index].name:continue
   for li in p.loop_indices:
    v=o.data.vertices[o.data.loops[li].vertex_index].co
    # Keep texture eye centres at the measured anatomical eye centres, independently of nose width.
    o.data.uv_layers[0].data[li].uv=(.5-v.y/(.200 if female else .206),facev(v.z))
  # Correct iris UVs: full radial iris material is separate from the face portrait.
  for p in o.data.polygons:
   if 'Iris' not in o.data.materials[p.material_index].name:continue
   for li in p.loop_indices:
    v=o.data.uv_layers[0].data[li].uv;v.x=(v.x-.346)/.05+.5;v.y=(v.y-.558)/.05+.5
  # A single, stable face feather at neck/ears, carried with vertices through morphs.
  colors=o.data.color_attributes.active_color
  if colors:
   for li,l in enumerate(o.data.loops):
    v=o.data.vertices[l.vertex_index].co
    def smooth(a,b,x):t=max(0,min(1,(x-a)/(b-a)));return t*t*(3-2*t)
    w=smooth(.005,.055,v.x)*(1-smooth(.052,.078,abs(v.y)))*smooth(-.185,-.145,v.z)
    colors.data[li].color=(w,0,0,1)
 # Finer tailored surface follows existing silhouette without changing joints/indices.
 if any(k in name for k in ['Top','Sleeve','Trousers','NPCLeg']):
  for v in o.data.vertices:
   if v.co.z<-.06:v.co+=v.normal*(.00045*math.sin(v.co.z*170+v.co.y*31))
 export(name,'human',False)
assert len(seen)==len(expected),(len(seen),len(expected))

# Rebuild non-human surfaces from the authored silhouettes; preserve all joint pivots.
creatures=[]
for path,prefix in [('Characters32/AllAmericanMeltdown_Characters32.blend',('Dog','Moose','Titan','Deathclaw','Scorpion','Karen','Trader','Mannequin')),('ModelsV57/Expansion57.blend',('Bear','Hornet','Rogue')),('ModelsV51/Border51.blend',('Sentinel','GuardVest','GuardHelmet'))]:
 creatures+=load(path,lambda n:any(n.startswith('SM_'+p) for p in prefix))
seen=set()
for source in creatures:
 name=source.name.split('.')[0][3:]
 if name in seen:continue
 seen.add(name);o=duplicate(source);activate(o)
 animal=any(k in name for k in ['Dog','Moose','Bear']);organic=animal or any(k in name for k in ['Titan','Deathclaw','Karen']);robot=any(k in name for k in ['Rogue','Trader','Sentinel','Guard'])
 skin='Fur' if 'Bear' in name else 'Hide' if animal else 'Skin' if organic else 'Polymer' if 'Mannequin' in name else 'Metal' if robot else 'Chitin'
 for i,mat in enumerate(o.data.materials):
  n=mat.name.lower();key='Bone' if any(k in n for k in ['bone','tooth','horn']) else 'Steel' if any(k in n for k in ['steel','metal']) else 'Rubber' if 'rubber' in n else 'Glow' if 'red' in n and robot else 'Cloth' if any(k in n for k in ['cloth','pants','shirt']) else skin
  o.data.materials[i]=mats[key]
 if len(o.data.polygons)<22000:
  sub=o.modifiers.new('Refined contour topology','SUBSURF');sub.subdivision_type='SIMPLE';sub.levels=1;bpy.ops.object.modifier_apply(modifier=sub.name)
 if organic:
  smooth=o.modifiers.new('Anatomical surface relaxation','SMOOTH');smooth.factor=.18;smooth.iterations=2;bpy.ops.object.modifier_apply(modifier=smooth.name)
  for v in o.data.vertices:
   p=v.co;f=.0008 if animal else .0016
   # Scar relief and fine muscle/skin folds baked into mesh, not runtime tessellation.
   v.co+=v.normal*f*(math.sin(p.z*53+p.y*23)*math.cos(p.x*37)+.35*math.sin(p.z*147+p.y*93))
 coords=[v.co for v in o.data.vertices];lo=Vector(tuple(min(v[i] for v in coords) for i in range(3)));hi=Vector(tuple(max(v[i] for v in coords) for i in range(3)));span=hi-lo
 if robot:
  # Inset access covers, paired hydraulic lines, edge bolts, cooling louvres.
  z=lo.z+span.z*.6;front=hi.x
  box((front+.003,0,z),(.008,max(.025,span.y*.42),max(.025,span.z*.24)),'Metal',.003)
  for sign in [-1,1]:
   y=sign*span.y*.27
   tube([(front-.004,y,lo.z+span.z*.25),(front+.008,y,z),(front-.008,y,lo.z+span.z*.85)],.005,'Steel')
   for zz in [z-span.z*.08,z+span.z*.08]:tube([(front+.006,y,zz),(front+.010,y,zz)],.004,'Steel',6)
  for j in range(5):box((front+.009,0,z+(j-2)*span.z*.025),(.003,span.y*.24,.002),'Rubber',.0004)
 elif 'Mannequin' in name:
  # Mold seams and joint socket trim follow the sculpted anatomy.
  if 'Head' not in name:
   tube([(lo.x+span.x*.5,span.y*.47,lo.z+span.z*t) for t in [.1,.3,.5,.7,.9]],.0018,'Steel',6)
 elif 'Scorpion' in name or 'Hornet' in name:
  if 'Torso' in name or 'Pelvis' in name:
   for j in range(5):
    x=lo.x+span.x*(.17+j*.14);width=span.y*.42*math.sin(math.pi*(.17+j*.14))
    tube([(x,math.cos(a*math.pi/12)*width,lo.z+span.z*.55+math.sin(a*math.pi/12)*span.z*.43) for a in range(13)],.007,'Bone',8)
 elif 'Deathclaw' in name or 'Titan' in name:
  if 'Torso' in name:
   for sign in [-1,1]:
    for j in range(5):
     z=lo.z+span.z*(.25+j*.105)
     tube([(hi.x-.018,sign*span.y*.1,z),(hi.x+.005,sign*span.y*.25,z+.017),(hi.x-.04,sign*span.y*.38,z+.028)],.007,'Skin',10)
  if 'Deathclaw' in name and ('Head' in name or 'Torso' in name):
   for j in range(4):
    x=lo.x+span.x*(.2+j*.17);z=hi.z-.015
    tube([(x,0,z),(x-.025,0,z+.065),(x-.08,0,z+.12)],0,'Bone',12,[.025,.014,.001])
 export(name)

# Organic world-eater segments replace the engine spheres, with a real radial maw.
for head in [False,True]:
 vs=[];fs=[];N=64;R=33
 for j in range(R):
  t=j/(R-1);x=-.5+t;r=(.09+.41*math.sin(math.pi*t)**.32)*(1+.025*math.cos(t*math.tau*9))
  if head:r=.34+.13*math.sin(math.pi*t)
  for k in range(N):
   a=k*math.tau/N;rr=r*(1+.025*math.sin(a*7+t*8));vs.append((x,math.cos(a)*rr,math.sin(a)*rr))
 for j in range(R-1):
  for k in range(N):a=j*N+k;b=j*N+(k+1)%N;fs.append((a,b,b+N,a+N))
 if not head:fs.extend([tuple(range(N-1,-1,-1)),tuple((R-1)*N+k for k in range(N))])
 mesh('Annulated body',vs,fs,'Skin')
 if head:
  ring(.45,0,.35,.23,.12,'Skin',64)
  for tier in range(2):
   for k in range(24):
    a=(k+tier*.5)*math.tau/24;r=.27+tier*.025
    tube([(.48-tier*.13,math.cos(a)*r,math.sin(a)*r),(.54-tier*.13,math.cos(a)*r*.78,math.sin(a)*r*.78),(.56-tier*.13,math.cos(a)*r*.6,math.sin(a)*r*.6)],0,'Bone',8,[.018,.012,.001])
 export('WorldEaterHead63' if head else 'WorldEaterSegment63')

# Purpose-built attachment models. Sights have open apertures, not opaque blocks.
optics={'att_reflexV14':(1,.0285),'att_holoV14':(1,.034),'att_scope4V14':(4,.033),'att_scope8V14':(8,.035),'att_micro63':(1,.023),'att_tube63':(1,.032),'att_prism63':(3,.034),'att_combat63':(6,.037),'att_marksman63':(12,.042)}
for name,(zoom,z) in optics.items():
 box((0,0,.004),(.052,.027,.008),'Metal',.002)
 for y in [-.015,.015]:box((0,y,.008),(.045,.005,.009),'Steel',.001)
 for x in [-.018,.018]:tube([(x,-.019,.008),(x,.019,.008)],.0035,'Steel',8)
 if name in ['att_reflexV14','att_holoV14','att_micro63']:
  width=.025 if 'holo' in name else .022 if 'reflex' in name else .018;height=z-.008
  # Swept, chamfered hood around empty lens aperture.
  for x in [-.010,.009]:tube([(x,-width,.009),(x,-width,z+height*.65),(x,-width*.7,z+height),(x,width*.7,z+height),(x,width,z+height*.65),(x,width,.009)],.003,'Metal',12)
  box((.014,0,.013),(.018,.031,.015),'Metal',.002)
 else:
  L=.11 if zoom<=3 else .16 if zoom==4 else .21 if zoom==6 else .27 if zoom==8 else .31
  rad=.017 if zoom<=3 else .022 if zoom<=6 else .028
  for j in range(9):ring((j/8-.5)*L,z,rad*(1.13 if j<2 or j>6 else .83),rad*(.87 if j<2 or j>6 else .65),L/8+.0005,'Metal')
  for x in [-L*.26,L*.26]:ring(x,z,rad*1.13,rad*.95,.008,'Rubber');box((x,0,.013),(.014,.032,.021),'Metal')
  for y in [-rad,rad]:tube([(0,y,z),(0,y*1.65,z)],.009,'Steel',20)
  tube([(0,0,z+rad*.7),(0,0,z+rad*1.5)],.01,'Metal',24)
  for k in range(12):
   a=k*math.tau/12;box((L*.4,math.cos(a)*rad*1.16,z+math.sin(a)*rad*1.16),(.013,.002,.002),'Rubber',.0003)
 export(name,'attachment')
ids=['att_lightV14','att_laserV14','att_verticalV14','att_angledV14','att_suppressor62','att_brake62','att_comp62','att_heavy62','att_short62','att_fluted62','att_precision62','att_lightstock62','att_padded62','att_tape62','att_rubber62','att_skeleton62']
for name in ids:
 if any(k in name for k in ['lightV','laserV']):
  rad=.015 if 'lightV' in name else .009
  for j in range(7):ring(.007+j*.008,0,rad*(1.1 if j>4 else 1),rad*.8,.008,'Metal',32)
  tube([(.062,0,0),(.063,0,0)],rad*.77,'Lens',32);box((.025,0,-rad),(.045,.027,.008));box((.01,rad,.005),(.017,.007,.008),'Rubber')
 elif any(k in name for k in ['suppressor','brake','comp','heavy','short','fluted']):
  L=.20 if 'suppressor' in name else .08 if 'brake' in name else .07 if 'comp' in name else .25 if 'heavy' in name else .15 if 'short' in name else .28
  rad=.022 if 'suppressor' in name else .014
  for j in range(11):ring(j*L/10,0,rad*(1.04 if j%2==0 else 1),.007,L/10+.0002,'Metal',32)
  if any(k in name for k in ['brake','comp']):
   for j in range(3):
    for y in [-rad,rad]:box((L*(.3+j*.22),y,0),(.011,.002,.012),'Rubber',.001)
  if 'fluted' in name:
   for k in range(8):
    a=k*math.tau/8;tube([(.02,math.cos(a)*rad,math.sin(a)*rad),(L-.02,math.cos(a)*rad,math.sin(a)*rad)],.002,'Steel',6)
 elif any(k in name for k in ['stock','precision','padded']):
  # Extruded shaped buttplate, cheek riser, buffer tube and adjustment lever.
  tube([(.03,0,0),(-.18,0,0)],.016,'Steel',24)
  profile=[(-.20,-.10),(-.22,-.08),(-.22,.035),(-.19,.05),(-.09,.035),(-.04,-.005),(-.13,-.055)]
  vs=[(x,y,z) for y in [-.019,.019] for x,z in profile];n=len(profile);fs=[tuple(range(n-1,-1,-1)),tuple(range(n,2*n))]+[(i,(i+1)%n,(i+1)%n+n,i+n) for i in range(n)]
  mesh('Contoured stock shell',vs,fs,'Metal');box((-.22,0,-.025),(.009,.043,.14),'Rubber',.003);box((-.135,0,.04),(.105,.044,.012),'Rubber',.003)
  tube([(-.13,-.025,-.028),(-.10,-.025,-.045),(-.075,-.025,-.028)],.004,'Steel')
 else:
  angled='angled' in name;L=.08 if 'vertical' in name else .07
  tube([(0,0,0),(.006 if angled else -.006,0,-L*.35),(.035 if angled else -.018,0,-L)],0,'Rubber',20,[.017,.018,.012])
  box((0,0,.002),(.05,.028,.006))
  for j in range(7):tube([(-.015+j*.001,-.017,-.012-j*.008),(.018+j*.001,-.017,-.012-j*.008)],.0015,'Steel',6)
 export(name,'attachment')
(OUT/'manifest.json').write_text(json.dumps({'assets':records,'optics':optics},indent=2));bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'Models63.blend'));print('MODELS63_PASS',len(records),flush=True)
