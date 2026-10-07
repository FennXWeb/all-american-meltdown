"""Adapt the approved survivor to the existing modular animation/creator contract.
Writes new versioned assets only; never modifies the approved source or old library.
"""
import bpy,bmesh,math,json,random
from pathlib import Path
from mathutils import Vector,Matrix
from mathutils.bvhtree import BVHTree
ROOT=Path('X:/LethalWorld');OUT=ROOT/'ArtSource/Characters61';OUT.mkdir(exist_ok=True)
random.seed(61)
bpy.ops.wm.open_mainfile(filepath=str(ROOT/'ArtSource/Character61/Survivor61_Approval.blend'))
source={o.name:o for o in bpy.context.scene.objects if o.type=='MESH' and o.name!='Review stage'}
for o in source.values():
 o.parent=None;o.matrix_world=Matrix.Identity(4);o.modifiers.clear();o.hide_render=True
with bpy.data.libraries.load(str(ROOT/'ArtSource/Characters35/AllAmericanMeltdown_Characters35.blend'),link=False) as (a,b):b.objects=[n for n in a.objects if n.startswith('SM_')]
old={o.name:o for o in b.objects if o}
with bpy.data.libraries.load(str(ROOT/'ArtSource/Characters35/AllAmericanMeltdown_HeadsHair38.blend'),link=False) as (a,b):b.objects=[n for n in a.objects if n.startswith('SM_Hair')]
for o in b.objects:
 if o:old[o.name.split('.')[0]]=o
MATS={};RECORDS=[]
specs={'Skin':('atlas',2),'FaceMale':('face',0),'FaceFemale':('face',1),'Cotton':('atlas',0),'Denim':('atlas',0),'Leather':('atlas',1),'BootLeather':('atlas',1),'HairRoot':('atlas',3),'HairCards':('cards',0),'Eye':('constant',[.55,.51,.43]),'Iris':('eye',0),'Pupil':('constant',[.004,.003,.002]),'Metal':('constant',[.16,.15,.11]),'Rubber':('constant',[.025,.023,.021]),'Thread':('constant',[.35,.32,.24]),'Lip':('constant',[.24,.10,.09])}
for k in specs:
 mat=bpy.data.materials.new('M_'+k+'61');mat.diffuse_color=(.3,.3,.3,1);MATS[k]=mat
def classify(name):
 n=name.lower()
 for key,words in [('FaceMale',['blended face']),('Skin',['skin','face']),('BootLeather',['boot','footwear']),('HairCards',['alpha strand']),('HairRoot',['hair','roots']),('Iris',['iris']),('Pupil',['pupil']),('Eye',['sclera','eye']),('Leather',['leather']),('Denim',['denim','charcoal']),('Rubber',['rubber','soles']),('Metal',['metal','hardware']),('Thread',['thread','stitch']),('Lip',['lip'])]:
  if any(w in n for w in words):return key
 return 'Cotton'
def duplicate(o,pred=lambda p:True):
 c=o.copy();c.data=o.data.copy();bpy.context.collection.objects.link(c);c.parent=None;c.matrix_world=Matrix.Identity(4);c.modifiers.clear();c.hide_render=False
 bm=bmesh.new();bm.from_mesh(c.data);bmesh.ops.delete(bm,geom=[f for f in bm.faces if not pred(f.calc_center_median())],context='FACES');bm.to_mesh(c.data);bm.free()
 for i,m in enumerate(c.data.materials):c.data.materials[i]=MATS[classify(m.name)]
 return c
def clipped(o,planes):
 c=duplicate(o);bm=bmesh.new();bm.from_mesh(c.data)
 for axis,value,sign in planes:
  co=Vector((0,0,0));co[axis]=value;normal=Vector((0,0,0));normal[axis]=sign
  bmesh.ops.bisect_plane(bm,geom=list(bm.verts)+list(bm.edges)+list(bm.faces),dist=1e-6,plane_co=co,plane_no=normal,clear_inner=True,clear_outer=False)
 bm.to_mesh(c.data);bm.free();return c

def byclip(prefix,planes):return [clipped(o,planes) for n,o in source.items() if any(n.startswith(s) for s in prefix)]

def bynames(prefix,pred=lambda p:True):return [duplicate(o,pred) for n,o in source.items() if any(n.startswith(s) for s in prefix)]
def join(parts):
 parts=[p for p in parts if len(p.data.polygons)]
 bpy.ops.object.select_all(action='DESELECT')
 for p in parts:p.select_set(True)
 bpy.context.view_layer.objects.active=parts[0];bpy.ops.object.join();return parts[0]
def retarget(o,kind,side=1):
 for v in o.data.vertices:
  p=v.co.copy()
  if kind=='head':p-=Vector((.06,0,1.704))
  elif kind=='torso':p-=Vector((.06,0,1.51))
  elif kind=='waist':p-=Vector((.06,0,1.00))
  elif kind=='arm':
   p-=Vector((.06,side*.18,1.46));p=Matrix.Rotation(math.radians(-side*31),3,'X')@p
  elif kind=='leg':
   p-=Vector((.06,side*.09,.87));p=Matrix.Rotation(math.radians(-side*10),3,'X')@p
  v.co=p
 o.data.update()
def export(name,o):
 o.name='SM_'+name
 # All materials use UV0; tattoo UV1 is filled by the existing creator. UV2 carries hair root stiffness.
 while len(o.data.uv_layers)<3:o.data.uv_layers.new()
 for p in o.data.polygons:p.use_smooth=True
 # Clean unreferenced points and degenerate edges before convex collision generation.
 bm=bmesh.new();bm.from_mesh(o.data);bmesh.ops.dissolve_degenerate(bm,dist=1e-7,edges=list(bm.edges));bmesh.ops.delete(bm,geom=[v for v in bm.verts if not v.link_faces],context='VERTS');bmesh.ops.recalc_face_normals(bm,faces=list(bm.faces));bm.to_mesh(o.data);bm.free()
 coords=[v.co for v in o.data.vertices];bounds={'min':[min(v[i] for v in coords) for i in range(3)],'max':[max(v[i] for v in coords) for i in range(3)]}
 # Match the project's verified Unreal FBX coordinate convention.
 copy=o.copy();copy.data=o.data.copy();bpy.context.collection.objects.link(copy)
 for v in copy.data.vertices:v.co.y*=-1
 bm=bmesh.new();bm.from_mesh(copy.data);bmesh.ops.reverse_faces(bm,faces=list(bm.faces));bm.to_mesh(copy.data);bm.free()
 bpy.ops.object.select_all(action='DESELECT');copy.select_set(True);bpy.context.view_layer.objects.active=copy
 bpy.ops.export_scene.fbx(filepath=str(OUT/(name+'.fbx')),use_selection=True,object_types={'MESH'},global_scale=1,apply_unit_scale=True,axis_forward='-Y',axis_up='Z',bake_anim=False,mesh_smooth_type='FACE',path_mode='RELATIVE')
 bpy.data.objects.remove(copy,do_unlink=True)
 used={p.material_index for p in o.data.polygons};slots=sorted({o.data.materials[i].name for i in used})
 RECORDS.append({'name':name,'fbx':name+'.fbx','slots':slots,'bounds':bounds,'triangles':sum(len(p.vertices)-2 for p in o.data.polygons)})
 o.hide_render=True
 print('CHAR61_EXPORT',name,flush=True)

# Full anatomical skin for uncovered sleeves/tops. The approval file deliberately removed covered faces.
vs=[];fs=[];g=''
for line in (ROOT/'ArtSource/Characters35/Reference/base.obj').read_text().splitlines():
 if line.startswith('v '):x,z,y=map(float,line.split()[1:]);vs.append((y*.108,x*.108,(z+8.1676)*.108))
 elif line.startswith('g '):g=line[2:]
 elif line.startswith('f ') and g=='body':fs.append(tuple(int(s.split('/')[0])-1 for s in line.split()[1:]))
me=bpy.data.meshes.new('FullAnatomy61');me.from_pydata(vs,[],fs);me.materials.append(MATS['Skin']);skin=bpy.data.objects.new('FullAnatomy61',me)
uv=me.uv_layers.new(name='MaterialUV')
for l in me.loops:p=me.vertices[l.vertex_index].co;uv.data[l.index].uv=(p.y*4+.5,p.z*4)

for female in [False,True]:
 sex='Female' if female else 'Male'
 pieces=byclip(['Survivor61 | anatomical body'],[(2,1.50,1)])+bynames(['Survivor61 | eye','Survivor61 | iris detail'])
 for o in pieces:
  if 'FaceUV' in o.data.uv_layers:
   face=[tuple(v.uv) for v in o.data.uv_layers['FaceUV'].data]
   for i,v in enumerate(face):o.data.uv_layers[0].data[i].uv=v
  if female:
   for i,m in enumerate(o.data.materials):
    if m==MATS['FaceMale']:o.data.materials[i]=MATS['FaceFemale']
   for v in o.data.vertices:
    z=v.co.z;v.co.y*=1-.06*math.exp(-((z-1.58)/.05)**2);v.co.x-=.003*math.exp(-((z-1.63)/.026)**2)
 head=join(pieces);retarget(head,'head');export(sex+'AnatomicalHead35',head)
 for style in range(9):
  if style==8:
   # Preserve the established covered underlayer instead of exposing anatomy.
   o=duplicate(old['SM_'+sex+'Top835']);export(sex+'Top835',o);continue
  pieces=byclip(['Survivor61 | fitted field jacket'],[(1,-.205,1),(1,.205,-1),(2,1.03,1)])
  if style in [0,3,5,6,7]:pieces+=bynames(['Jacket | bellows','Jacket | folded','Pocket |','Jacket | shaped collar','Jacket | collar piping','Jacket | brass','Jacket | center'])
  o=join(pieces)
  for i,mat in enumerate(o.data.materials):
   if style==3 and mat==MATS['Cotton']:o.data.materials[i]=MATS['Leather']
  for v in o.data.vertices:
   p=v.co
   if female:
    p.y*=1-.05*math.exp(-((p.z-1.16)/.14)**2);p.x+=.008*math.exp(-((p.z-1.35)/.065)**2)
   if style==5 and p.z<1.15:p.z-=.13*(1.15-p.z)/.12
   if style in [1,2] and p.z>1.49:p.z=1.49+(p.z-1.49)*.35
   if style==4:p.x*=1.035;p.y*=1.035
  retarget(o,'torso')
  if style==4:
   hood=duplicate(old['SM_'+sex+'Top435'],lambda p:p.z>-.045);o=join([o,hood])
  export(sex+f'Top{style}35',o)
 for style in range(6):
  o=join(bynames(['Survivor61 | cargo trousers'],lambda p:.855<p.z<1.067)+bynames(['Survivor61 | belt','Belt |']))
  retarget(o,'waist')
  if female:
   for v in o.data.vertices:v.co.y*=1.06
  export(sex+f'Waist{style}35',o)

for style in range(9):
 for side in [-1,1]:
  skin_cut=.17 if style in [1,6,8] else .265 if style==2 else .385
  skinarm=clipped(skin,[(1,side*skin_cut,side),(2,.84,1)])
  pieces=[skinarm]
  if style not in [1,6,8]:
   pieces+=byclip(['Survivor61 | fitted field jacket'],[(1,side*.17,side)]+([(1,side*.28,-side)] if style==2 else []))
   pieces+=bynames(['Jacket | rolled cuff','Jacket | cuff seam'],lambda p:side*p.y>.17) if style!=2 else []
  o=join(pieces);retarget(o,'arm',side)
  if style==3:
   for i,m in enumerate(o.data.materials):
    if m==MATS['Cotton']:o.data.materials[i]=MATS['Leather']
  export(f'Sleeve{style}{"L" if side<0 else "R"}35',o)
  if side<0:export(f'Sleeve{style}35',duplicate(o))

for style in range(6):
 for side in [-1,1]:
  pieces=bynames(['Survivor61 | cargo trousers'],lambda p:p.z<.89 and side*p.y>0 and (style!=3 or p.z>.49))
  if style==3:pieces.append(duplicate(skin,lambda p:.20<p.z<.51 and side*p.y>0))
  if style==1:pieces+=bynames(['Trousers | thigh','Trousers | cargo'],lambda p:side*p.y>0)
  o=join(pieces);retarget(o,'leg',side)
  if style==5:
   for v in o.data.vertices:v.co.x*=.94;v.co.y*=.94
  export(f'Trousers{style}{"L" if side<0 else "R"}35',o)
  if side<0:export(f'Trousers{style}35',duplicate(o))
  shoes=join(bynames(['Boot |'],lambda p:side*p.y>0));retarget(shoes,'leg',side)
  for i,m in enumerate(shoes.data.materials):
   if m==MATS['Leather']:shoes.data.materials[i]=MATS['BootLeather']
  export(f'NPCLeg{style}{"L" if side<0 else "R"}35',join([duplicate(o),shoes]))
  if side<0:export(f'NPCLeg{style}35',duplicate(bpy.data.objects['SM_'+f'NPCLeg{style}L35']))
for style in range(4):
 for side in [-1,1]:
  o=join(bynames(['Boot |'],lambda p:side*p.y>0));retarget(o,'leg',side)
  for i,m in enumerate(o.data.materials):
   if m==MATS['Leather']:o.data.materials[i]=MATS['BootLeather']
  for v in o.data.vertices:
   if style==1 and v.co.z>-.74:v.co.z=-.74+(v.co.z+.74)*.25
   if style==2 and v.co.z>-.74:v.co.z=-.74+(v.co.z+.74)*1.25
  export(f'Footwear{style}{"L" if side<0 else "R"}35',o)
  if side<0:export(f'Footwear{style}35',duplicate(o))

# Preserve accessory variety and first-person grip landmarks; new material treatment is shared.
for name,o in old.items():
 if any(name.startswith('SM_'+p) for p in ['Hat','Eyewear','Beard','RightHand','LeftHand']):
  c=duplicate(o)
  if name.startswith('SM_Hat'):
   backward=name.startswith('SM_Hat3');pivot=.014 if backward else -.014
   for v in c.data.vertices:
    p=v.co;p.x=(p.x-pivot)*1.18+.018;p.y*=1.12;p.z=.025+(p.z-.025)*1.4
  if name.startswith('SM_Beard'):
   for v in c.data.vertices:v.co.x+=.003
  export(name[3:],c)

# Every hairstyle keeps its distinct authored silhouette, with new strand textures,
# tapered cards and exported root-to-tip compliance for live physics.
for style in range(1,20):
 src=old['SM_Hair%02d35'%style];root=duplicate(src)
 for i,m in enumerate(root.data.materials):root.data.materials[i]=MATS['HairRoot']
 top=max(v.co.z for v in root.data.vertices);bottom=min(v.co.z for v in root.data.vertices)
 tree=BVHTree.FromPolygons([v.co for v in root.data.vertices],[tuple(p.vertices) for p in root.data.polygons])
 parts=[root]
 for layer in range(2 if style in [1,2,14] else 3):
  for k in range(32):
   angle=k*math.tau/32+layer*.065
   if style==8 and abs(math.sin(angle))>.30:continue
   length=top-max(bottom,.008-.06*(1-max(0,math.cos(angle))))
   if style in [5,6,9,11,12,15,18,19] and math.cos(angle)<.35:length=top-bottom
   if style==1:length*=.5
   vv=[];uvs=[];compliance=[]
   for j in range(9):
    t=j/8;z=top-.005-length*t;a=angle+(.5 if style in [3,4,13] else .15)*t
    radial=Vector((math.cos(a),math.sin(a),0));hit=tree.ray_cast(radial*.45+Vector((0,0,z)),-radial)
    if hit[0] and hit[0].dot(radial)>.045:center=hit[0]+radial*(.0018+layer*.0008)
    else:
     nearest=tree.find_nearest(radial*.085+Vector((0,0,z)))[0]
     center=nearest.copy() if nearest is not None else radial*.085+Vector((0,0,z))
     # A ray through an open fringe can hit the far side. Never bridge it across the face/neck.
     if z<top-.045:
      center.z=z
      radius=max(.075,center.dot(radial));center=radial*radius+Vector((0,0,z))
     else:center+=radial*.002
    if j and z<top-.045 and (center-vv[-2]).length>.045:
     previous=vv[-2];center=Vector((previous.x,previous.y,z))+radial*.0005
    sidevec=Vector((-math.sin(a),math.cos(a),0));width=.007 if style in [1,2,14] else .012
    center.z-=.011*t**5
    for c in range(3):vv.append(center+sidevec*(c-1)*width*(1-.70*t**5));uvs.append(((k%8+.06+c*.44)/8,1-t*.99));compliance.append(t*t*min(1,length/.15))
   me=bpy.data.meshes.new('HairCard61');me.from_pydata(vv,[],[(j*3+c,j*3+c+1,(j+1)*3+c+1,(j+1)*3+c) for j in range(8) for c in range(2)]);me.materials.append(MATS['HairCards']);card=bpy.data.objects.new('HairCard61',me);bpy.context.collection.objects.link(card)
   for _ in range(3):me.uv_layers.new()
   for l in me.loops:me.uv_layers[0].data[l.index].uv=uvs[l.vertex_index];me.uv_layers[2].data[l.index].uv=(compliance[l.vertex_index],0)
   parts.append(card)
 while len(root.data.uv_layers)<3:root.data.uv_layers.new()
 for l in root.data.loops:
  p=root.data.vertices[l.vertex_index].co;root.data.uv_layers[0].data[l.index].uv=(math.atan2(p.y,p.x)/math.tau+.5,(top-p.z)/max(.01,top-bottom));root.data.uv_layers[2].data[l.index].uv=(0,0)
 o=join(parts)
 for v in o.data.vertices:v.co.z+=.018
 export('Hair%02d35'%style,o)

(OUT/'manifest.json').write_text(json.dumps({'assets':RECORDS,'materials':specs,'head_anchor_cm':170.4,'source':'Approved Character61; CC0 anatomy; existing original wardrobe silhouettes'},indent=2))
bpy.ops.wm.save_as_mainfile(filepath=str(OUT/'Characters61_Runtime.blend'))
print('CHARACTERS61_BUILT',len(RECORDS),flush=True)
